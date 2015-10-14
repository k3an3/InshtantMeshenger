#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <cstdlib>
#include <cstdint>
#include <vector>
#include <string>

#include <parser.h>
#include <net.h>

using boost::asio::ip::udp;
using namespace std;

const int32_t MAX_LENGTH = 1024;
const uint8_t RESP[] = "meshenger-discovery-response";
const uint8_t MSG[] = "meshenger-discovery-probe";

namespace libmeshenger
{
	void
	netVerbosePrint(string s, int color)
	{
		cout << "\033[1;31m[libmeshenger-net]\033[0m-> " <<
			"\033[1;" << color << "m" << s << "\033[0m" << endl;
	}

	/* Net class methods */

	/* Default constructor. Initializes io_service */
	Net::Net(boost::asio::io_service& io_service, uint16_t udp_port,
			uint16_t tcp_port)
		: io_service(io_service),
		udp_port(udp_port),
		tcp_port(tcp_port),
		/* Initialize UDP listen socket on all interfaces */
		listen_socket(io_service, udp::endpoint(udp::v4(), udp_port)),
		data()
	{

	}

	void
	Net::discoveryListen()
	{
		/* Handle any incoming connections asynchronously */
		netVerbosePrint("Starting discovery listener", 33);
		listen_socket.async_receive_from(
			boost::asio::buffer(data, MAX_LENGTH), remote_endpoint,
			/* Bind connection to acceptDiscoveryConn method */
			boost::bind(&Net::acceptDiscoveryConn, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}

	void
	Net::acceptDiscoveryConn(const boost::system::error_code& error, size_t recv_len)
	{
		/* Bind handler for new connections. */

		/* Check if we received a discovery packet and if it is from a new peer */
		if (error) netVerbosePrint("ERROR", 31);
		if (recv_len > 0) {
			remote_endpoint.port(1234);
			udp::socket socket(io_service, udp::endpoint(udp::v4(), 0));
			socket.set_option(udp::socket::reuse_address(true));
		   	if (!strcmp((char*) data, (char*) MSG)) {
				netVerbosePrint("Received probe from peer " +
						remote_endpoint.address().to_string());
			//	socket.send_to(boost::asio::buffer(RESP, strlen((char*) RESP)), remote_endpoint);
				socket.async_send_to(
					boost::asio::buffer(RESP, strlen((char*) RESP)), remote_endpoint,
					boost::bind(&Net::handleDiscoveryReply, this,
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
			} else if (!strcmp((char*) data, (char*) RESP)) {
				netVerbosePrint("Received discovery response from peer " +
						remote_endpoint.address().to_string());
				socket.async_send_to(
					boost::asio::buffer("", 0), remote_endpoint,
					boost::bind(&Net::handleDiscoveryReply, this,
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
			}
			if (!peerExistsByAddress(remote_endpoint.address())) {
				peers.insert(peers.end(), Peer(remote_endpoint.address()));
				netVerbosePrint("Found new peer at " +
						remote_endpoint.address().to_string(), 32);
			}
		}
		else
			listen_socket.async_receive_from(
				boost::asio::buffer(data, MAX_LENGTH), remote_endpoint,
				boost::bind(&Net::acceptDiscoveryConn, this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
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
		if (error) netVerbosePrint("ERROR", 31);
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

		netVerbosePrint("Sending discovery probe", 33);

		/* Create the socket that will send UDP broadcast */
		udp::socket socket(io_service, udp::endpoint(udp::v4(), 0));

		/* Set socket options so that it can use the host's address and send
		* broadcast */
		socket.set_option(udp::socket::reuse_address(true));
		socket.set_option(udp::socket::broadcast(true));

		/* Create endpoint for the connections */
		udp::endpoint endpoint(boost::asio::ip::address_v4::broadcast(), udp_port);

		/* Send discovery packet */
		socket.send_to(boost::asio::buffer(MSG, strlen((char*) MSG)), endpoint);

		/*
		size_t recv_length = socket.receive_from(boost::asio::buffer(data, MAX_LENGTH), endpoint);
		if (!strcmp((char*) data, (char*) RESP)) {
			netVerbosePrint("Received discovery response from " + endpoint.address().to_string());
			boost::asio::ip::address addr = endpoint.address();
			if (!peerExistsByAddress(addr)) {
				peers.insert(peers.end(), Peer(addr));
				netVerbosePrint("Found new peer at " +
						addr.to_string(), 32);
			}
		}
		*/
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

	void
	sendToAllPeers(Packet p)
	{
		for(int i = 0; i < peers.size(); i++) {
			boost::asio::ip::address addr = peers[i].ip_addr;

			ip::basic_endpoint endpoint(addr, tcp_port);

			tcp::socket sock(io_svc);

			sock.connect(endpoint);
			sock.send(boost::asio:buffer(p.raw().data(), p.raw().size()));
		}
	}

	bool
	Net::receivePacket()
	{
	    try {
			boost::asio::io_service io_service;
			boost::asio::ip::tcp::acceptor acceptor(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), tcp_port));

			{
				boost::asio::ip::tcp::socket socket(io_service);

				acceptor.accept(socket);

				boost::asio::streambuf sb;
				boost::system::error_code ec;
				boost::asio::read(socket, sb, ec);
				std::cout << "received: '" << &sb << "'\n";// prints the messages
				socket.close();
				if (ec) {
					std::cout << "status: " << ec.message() << "\n";// print the status of everything when all the messages are sent
				}
			}
		}
		catch (std::exception& e)
		{
			std::cerr << "Exception: " << e.what() << std::endl;
		}
	}

	/* Peer class methods */
	Peer::Peer(boost::asio::ip::address ip_addr)
		: ip_addr(ip_addr)
	{

	}

	Peer::Peer(std::string s)
	{
		ip_addr = boost::asio::ip::address::from_string(s);
	}
}
