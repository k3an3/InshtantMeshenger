#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <cstdlib>
#include <cstdint>

#include <net.h>

using boost::asio::udp;

namespace libmeshenger
{
	/* Net class methods */

	/* Default constructor. Initializes io_service */
	Net::Net(boost::asio::io_service& io_service)
		: io_service(io_service),
	{

	}

	void
	Net::discoveryListen(u_int16_t port)
		/* Initialize UDP listen socket on all interfaces */
		: listen_socket(io_service, udp::endpoint(udp::v4(), port))
	{
		/* Handle any incoming connections asynchronously */
		listen_socket.async_receive_from(
			boost::asio::buffer(data, MAX_LENGTH), remote_endpoint,
			/* Bind connection to accept_conn method */
			boost::bind(&Net::accept_conn, this,
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
			boost::bind(&DiscoveryListener::send_discovery_reply, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}

	void
	Net::send_discovery_reply(const boost::system::error_code& error, size_t len)
	{
		/* Sends a discovery reply to a remote node */
	}

	/* Peer class methods */
	Peer::Peer(boost::asio::ip::address ip_addr)
		: ip_addr(ip_addr){

		}
}
