#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <cstdlib>
#include <cstdint>

#include <net.h>

using boost::asio::ip::udp;
int32_t MAX_LENGTH = 1024;

namespace libmeshenger
{
	/* Net class methods */

	/* Default constructor. Initializes io_service */
	Net::Net(boost::asio::io_service& io_service, uint16_t udp_port,
			uint16_t tcp_port)
		: io_service(io_service),
		udp_port(udp_port),
		tcp_port(tcp_port),
		/* Initialize UDP listen socket on all interfaces */
		listen_socket(io_service, udp::endpoint(udp::v4(), udp_port))
	{

	}

	void
	Net::discoveryListen()
	{
		/* Handle any incoming connections asynchronously */
		listen_socket.async_receive_from(
			boost::asio::buffer(data, MAX_LENGTH), remote_endpoint,
			/* Bind connection to acceptConn method */
			boost::bind(&Net::acceptConn, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}

	void
	Net::acceptConn(const boost::system::error_code& error, size_t len)
	{
		/* Bind handler for new connections. */
		/* TODO: Construct node object if node is previously unseen */
		listen_socket.async_send_to(
			boost::asio::buffer(data, MAX_LENGTH), remote_endpoint,
			boost::bind(&Net::send_discovery_reply, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}

	void
	Net::send_discovery_reply(const boost::system::error_code& error, size_t len)
	{
		/* Sends a discovery reply to a remote node */
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
		}
	}

	/* Peer class methods */
	Peer::Peer(boost::asio::ip::address ip_addr)
		: ip_addr(ip_addr)
	{

	}
}
