#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <cstdlib>
#include <cstdint>
#include <vector>
#include <string>

#include <parser.h>
#include <net.h>
#include <parser.h>

#define NET_DEBUG true

using boost::asio::ip::udp;
using boost::asio::ip::tcp;
using namespace std;

const int32_t MAX_LENGTH = 1024;
const uint8_t RESP[] = "meshenger-discovery-reply";
const uint8_t MSG[] = "meshenger-discovery-probe";

namespace libmeshenger
{
	void
	netDebugPrint(string s, int color)
	{
		if (NET_DEBUG)
			cout << "\033[1;31m[libmeshenger-net]\033[0m-> " <<
				"\033[1;" << color << "m" << s << "\033[0m" << endl;
	}

	/* Net class methods */

	/* Default constructor for Net. Port numbers are used to bind to. */
	Net::Net(uint16_t udp_port,
			uint16_t tcp_port)
		: udp_port(udp_port),
		io_service(),
		tcp_port(tcp_port),
		/* Initialize UDP listen socket on all interfaces */
		listen_socket(io_service, udp::endpoint(udp::v4(), udp_port)),
		msg_socket(io_service),
		msg_acceptor(io_service, tcp::endpoint(tcp::v4(), tcp_port))
	{
	}

	/* Run the io_service object in a new thread to facilitate async operations */
	void
	Net::run()
	{
		boost::thread thread(boost::bind(&boost::asio::io_service::run, &io_service));
	}

	void
	Net::actuallyRun()
	{
		io_service.run();
	}

	/* Starts a UDP listener on the provided port. The listener will
	 * create new peer objects upon new connections and responds to the
	 * remote host. */
	void
	Net::discoveryListen()
	{
		netDebugPrint("Starting discovery listener...", 43);
		/* Handle any incoming connections asynchronously */
		listen_socket.async_receive_from(
			boost::asio::buffer(data, MAX_LENGTH), remote_endpoint,
			/* Bind connection to acceptDiscoveryConn method */
			boost::bind(&Net::acceptDiscoveryConn, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}

	/* Bind handler for incoming discovery probes */
	void
	Net::acceptDiscoveryConn(const boost::system::error_code& error, size_t recv_len)
	{
		/* Check if we received a discovery packet and if it is from a new peer */
		if (error) netDebugPrint("ERROR", 33);
		if (recv_len > 0) {
			/* Need to reply to the server port, not the one that was used to
			 * connect to us */
			remote_endpoint.port(udp_port);
			udp::socket socket(io_service, udp::endpoint(udp::v4(), 0));
			socket.set_option(udp::socket::reuse_address(true));

		   	if (!strcmp((char*) data, (char*) MSG)) {
				netDebugPrint("Received discovery probe from peer " +
						remote_endpoint.address().to_string(), 32);
				socket.async_send_to(
					boost::asio::buffer(RESP, strlen((char*) RESP) + 1), remote_endpoint,
					boost::bind(&Net::handleDiscoveryReply, this,
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
				addPeerIfNew(remote_endpoint.address());
				return;
			} else if (!strcmp((char*) data, (char*) RESP)) {
				netDebugPrint("Received discovery reply from peer " +
						remote_endpoint.address().to_string(), 33);
				addPeerIfNew(remote_endpoint.address());
			} else
				netDebugPrint("Received invalid probe", 30);
		}
		listen_socket.async_receive_from(
			boost::asio::buffer(data, MAX_LENGTH), remote_endpoint,
			boost::bind(&Net::acceptDiscoveryConn, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}

	void
	Net::addPeerIfNew(boost::asio::ip::address ip_addr)
	{
		if (!peerExistsByAddress(ip_addr)) {
			addPeer(ip_addr);
			netDebugPrint("Added new peer " +
					ip_addr.to_string(), 32);
		} else {
			 netDebugPrint("Peer already exists at " +
					 ip_addr.to_string(), 30);
		}
	}

	/* Not the best way to do this */
	bool
	Net::peerExistsByAddress(boost::asio::ip::address ip_addr)
	{
		for(auto &peer : peers)
			if (peer.ip_addr == ip_addr) return true;
		return false;
	}

	void
	Net::handleDiscoveryReply(const boost::system::error_code& error, size_t send_len)
	{
		/* Simply a bind handler */
		if (error) netDebugPrint("ERROR", 31);
		netDebugPrint("Sending discovery reply to " + remote_endpoint.address().to_string(), 33);
		listen_socket.async_receive_from(
			boost::asio::buffer(data, MAX_LENGTH), remote_endpoint,
			boost::bind(&Net::acceptDiscoveryConn, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}

	void
	Net::discoverPeers()
	{
		/* Discover peers on the LAN using UDP broadcast */

		netDebugPrint("Sending discovery probe...", 42);

		/* Create the socket that will send UDP broadcast */
		udp::socket socket(io_service, udp::endpoint(udp::v4(), 0));

		/* Set socket options so that it can use the host's address and send
		* broadcast */
		socket.set_option(udp::socket::reuse_address(true));
		socket.set_option(udp::socket::broadcast(true));

		/* Create endpoint for the connections */
		udp::endpoint endpoint(boost::asio::ip::address_v4::broadcast(), udp_port);

		/* Send discovery packet */
		socket.async_send_to(boost::asio::buffer(MSG, strlen((char*) MSG) + 1),
				endpoint, boost::bind(&Net::discoveryHandler, this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
	}

	void
	Net::discoveryHandler(const boost::system::error_code& error,
			  size_t bytes_transferred)
	{

	}

	Packet
	Net::getPacket()
	{
		Packet p = packets.back();
		packets.pop_back();
		return p;
	}

	void
	Net::addPeer(Peer p)
	{
		peers.push_back(p);
	}

	void
	Net::addPeer(std::string s)
	{
		peers.push_back(Peer(s));
	}

	std::vector<Peer>
	Net::getPeers()
	{
		return std::vector<Peer>(peers.begin(), peers.end());
	}

	/* Sends a Packet to all previously discovered peers using TCP */
	void
	Net::sendToAllPeers(Packet p)
	{
		// Should probably have addr reuse in here
		/* Cycle through the peers vector and prepare to send */
		for(int i = 0; i < peers.size(); i++) {
			/* Peer IP address */
			boost::asio::ip::address addr = peers[i].ip_addr;

			/* Endpoint for the peer */
			tcp::endpoint endpoint(addr, tcp_port);

			/* Socket used to create the connection */
			tcp::socket sock(io_service);

			/* Try to connect and report any connection errors */
			try {
				sock.connect(endpoint);
				/* Send the data */
				netDebugPrint("Sending message to " +
						endpoint.address().to_string(), 35);
				sock.send(boost::asio::buffer(p.raw().data(), p.raw().size()));
				peers[i].strikes = 0;
			} catch(std::exception &e) {
				/* Handle connection errors */
				netDebugPrint(e.what(), 41);
				netDebugPrint("Peer " + addr.to_string() +
						" is problematic. Strike.", 33);
				peers[i].strikes++;

				/* Remove peer if it fails to be reached 3 times */
				if (peers[i].strikes >= 3) {
					netDebugPrint("Three strikes. Removing.", 31);
					peers.erase(peers.begin() + i);
				}
			}
		}
	}

	void
	Net::startListen()
	{
		messageAcceptor();
	}

	void
	Net::messageAcceptor()
	{
		msg_acceptor.async_accept(msg_socket,
				[this](boost::system::error_code ec)
		{
			if (!ec) {
				size_t bytes = msg_socket.read_some(boost::asio::buffer(msg, MAX_LENGTH));
				vector<uint8_t> v(msg, msg + bytes);
				if (ValidatePacket(v)) {
					netDebugPrint("Packet received from " +
							msg_socket.remote_endpoint().address().to_string(),
						   	36);
					packets.push_back(Packet(v));
				}
			}
			msg_socket.close();

			messageAcceptor();
		});
	}

	/* Accept TCP connections on tcp_port and attempt to create a valid packet
	 * if possible. Adds packet to the packets vector and returns the length
	 * of the packet. */
	uint16_t
	Net::receivePacket()
	{
		/*
		auto self(shared_from_this());
		msg_socket.async_receive(boost::asio::buffer(msg, MAX_LENGTH),
				[this, self](boost::system::error_code error, size_t bytes)
				{
				if (!error) {
					vector<uint8_t> v(msg, msg + bytes);
					if (ValidatePacket(v)) {
						netDebugPrint("Packet received from", 36);
						packets.push_back(Packet(v));
					}
				}

				});
		/*
			size_t bytes = boost::asio::read(socket, boost::asio::buffer(b, MAX_LENGTH), ec);
			/* If the packet is valid, construct and add to packet vector */
			//string s = socket.remote_endpoint().address().to_string();
		return packets.size();
	}

	/* Peer class methods */
	Peer::Peer(boost::asio::ip::address ip_addr)
		: ip_addr(ip_addr),
		strikes(0)
	{

	}

	Peer::Peer(std::string s)
		: strikes(0)
	{
		ip_addr = boost::asio::ip::address::from_string(s);
	}
}
