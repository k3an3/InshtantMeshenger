#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <cstdlib>
#include <cstdint>
#include <vector>
#include <string>

#include <net.h>

using boost::asio::ip::udp;
using namespace std;

const int32_t MAX_LENGTH = 1024;
const uint8_t RESP[] = "meshenger-discovery-response\n";

namespace libmeshenger
{
	void
	netVerbosePrint(string s)
	{
		cout << "\033[1;31m[libmeshenger-net]\033[0m-> " << s << std::endl;
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
		data("")
	{

	}

	void
	Net::discoveryListen()
	{
		/* Handle any incoming connections asynchronously */
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
		if (!strcmp((char*) data, "meshenger-discovery-probe\n")) {
			netVerbosePrint("Received probe from peer " +
					remote_endpoint.address().to_string());
			if (!peerExistsByAddress(remote_endpoint.address())) {
				peers.insert(peers.end(), Peer(remote_endpoint.address()));
				netVerbosePrint("Found new peer at " +
						remote_endpoint.address().to_string());
			}
		}
		if (recv_len > 0)
			listen_socket.async_send_to(
				boost::asio::buffer(RESP, MAX_LENGTH), remote_endpoint,
				boost::bind(&Net::send_discovery_reply, this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
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
	Net::send_discovery_reply(const boost::system::error_code& error, size_t send_len)
	{
		/* Sends a discovery reply to a remote node */
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

		/* Create the socket that will send UDP broadcast */
		udp::socket socket(io_service, udp::endpoint(udp::v4(), 0));

		/* Set socket options so that it can use the host's address and send
		* broadcast */
		socket.set_option(udp::socket::reuse_address(true));
		socket.set_option(udp::socket::broadcast(true));

		/* Create endpoint for the connections */
		udp::endpoint endpoint(boost::asio::ip::address_v4::broadcast(), udp_port);

		/* Send discovery packet */
		socket.send_to(boost::asio::buffer(data, MAX_LENGTH), endpoint);

		while(1) {
			size_t recv_length = socket.receive_from(boost::asio::buffer(data, MAX_LENGTH), endpoint);
			cout << data << "\n";
		}
	}

	/* Peer class methods */
	Peer::Peer(boost::asio::ip::address ip_addr)
		: ip_addr(ip_addr)
	{

	}
}
