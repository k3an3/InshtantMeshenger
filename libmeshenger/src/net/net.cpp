#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <cstdlib>
#include <cstdint>
#include <vector>
#include <string>
#include <mutex>

#include <parser.h>
#include <net.h>

#define NET_DEBUG true

using boost::asio::ip::udp;
using boost::asio::ip::tcp;
using namespace std;

const int32_t MAX_LENGTH = 1024;
const uint8_t RESP[] = "meshenger-discovery-reply";
const uint8_t MSG[] = "meshenger-discovery-probe";
const uint8_t DIS[] = "meshenger-disconnect";

namespace libmeshenger
{
	void
	netDebugPrint(string s, int color)
	{
		if (NET_DEBUG)
			cout << "\033[1;31m[net]\033[0m-> " <<
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
		udp_listen_socket(io_service),
		tcp_listen_socket(io_service),
		tcp_acceptor(io_service)
	{
	}

	/* Run the io_service object in a new thread to facilitate async operations */
	void
	Net::run()
	{
		boost::thread thread(boost::bind(&boost::asio::io_service::run, &io_service));
	}

	/* Starts a UDP listener on the provided port. The listener will
	 * create new peer objects upon new connections and responds to the
	 * remote host. */
	void
	Net::discoveryListen()
	{
		udp_listen_socket = udp::socket(io_service, udp::endpoint(udp::v4(), udp_port));
		netDebugPrint("Starting discovery listener...", 43);
		/* Handle any incoming connections asynchronously */
		udp_listen_socket.async_receive_from(
			boost::asio::buffer(data, MAX_LENGTH), udp_remote_endpoint,
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
			udp_remote_endpoint.port(udp_port);
			udp::socket socket(io_service, udp::endpoint(udp::v4(), 0));
			socket.set_option(udp::socket::reuse_address(true));

		   	if (!strcmp((char*) data, (char*) MSG)) {
				netDebugPrint("Received discovery probe from peer " +
						udp_remote_endpoint.address().to_string(), 32);
				socket.async_send_to(
					boost::asio::buffer(RESP, strlen((char*) RESP) + 1), udp_remote_endpoint,
					boost::bind(&Net::handleDiscoveryReply, this,
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
				addPeerIfNew(udp_remote_endpoint.address());
				return;
			} else if (!strcmp((char*) data, (char*) RESP)) {
				netDebugPrint("Received discovery reply from peer " +
						udp_remote_endpoint.address().to_string(), 33);
				addPeerIfNew(udp_remote_endpoint.address());
			} else
				netDebugPrint("Received invalid probe", 30);
		}
		udp_listen_socket.async_receive_from(
			boost::asio::buffer(data, MAX_LENGTH), udp_remote_endpoint,
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

	/* Bind handler for sending discovery replies */
	void
	Net::handleDiscoveryReply(const boost::system::error_code& error, size_t send_len)
	{
		if (error) netDebugPrint("ERROR", 31);
		netDebugPrint("Sending discovery reply to " + udp_remote_endpoint.address().to_string(), 33);
		udp_listen_socket.async_receive_from(
			boost::asio::buffer(data, MAX_LENGTH), udp_remote_endpoint,
			boost::bind(&Net::acceptDiscoveryConn, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}

	/* Discover peers on the LAN using UDP broadcast */
	void
	Net::discoverPeers()
	{
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
				endpoint, [this](boost::system::error_code ec,
					size_t bytes_transferred)
			   	{

				});
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
				netDebugPrint("Sending packet to " +
						endpoint.address().to_string(), 35);
				sock.async_send(boost::asio::buffer(p.raw().data(),
							p.raw().size()), [this](boost::system::error_code ec,
							size_t bytes)
						{
							// Handle something
						});
				peers[i].strikes = 0;
			} catch(std::exception &e) {
				if (!strcmp(e.what(), "connect: Connection refused")) {
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
				}else {
						 netDebugPrint(e.what(), 41);
				}
			}
		}
	}

	/* Start a TCP acceptor to accept incoming packets */
	void
	Net::startListen()
	{
		if (!tcp_acceptor.is_open()) {
			tcp_acceptor = tcp::acceptor(io_service, tcp::endpoint(tcp::v4(), tcp_port));
		}
		tcp_acceptor.async_accept(tcp_listen_socket,
				[this](boost::system::error_code ec)
		{
			if (!ec) {
				size_t bytes = tcp_listen_socket.read_some(boost::asio::buffer(msg, MAX_LENGTH));
				vector<uint8_t> v(msg, msg + bytes);
				if (ValidatePacket(v)) {
				/*
					netDebugPrint("Packet received from " +
							tcp_listen_socket.remote_endpoint().address().to_string(),
						   	36);
							*/
					io_mutex.lock();
					packets.push_back(Packet(v));
					io_mutex.unlock();
				}
			}
			tcp_listen_socket.close();

			startListen();
		});
	}

	/* Accept TCP connections on tcp_port and attempt to create a valid packet
	 * if possible. Adds packet to the packets vector and returns the length
	 * of the packet. */
	uint16_t
	Net::receivePacket()
	{
		// Change this?
		// This is where the threadsplosion is happening
		// packets is modified both by the main thread (which calls
		// receivePacket() and by the asio thread handling receives
		// with the lambda function defined in start_listen
		//
		// Some mutex/queue magic must be used here to make sure nothing
		// explodes between that lambda (running in the asio thread) and
		// this function (running in the main thread)
		//
		// Also, get rid of all the cout in the lambda function. That's a thread
		// violation too.
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
