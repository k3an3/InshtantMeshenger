#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <cstdlib>
#include <cstdint>

#include <net.h>

using boost::asio::udp;

namespace libmeshenger
{
	/* Class methods */
	Net::Net(boost::asio::io_service& io_service)
		: io_service(io_service),
	{

	}

	void
	Net::discoveryListen(u_int16_t port)
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
}
