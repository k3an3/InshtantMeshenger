#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <cstdlib>
#include <cstdint>
#include <string>

using boost::asio::ip::udp;
using namespace std;

int32_t MAX_LENGTH = 1024;

namespace libmeshenger
{
	/* Class for the UDP discovery server */
	class DiscoveryListener
	{
		private:
			/* IO service for async asio operations */
			boost::asio::io_service& io_service;
			/* Socket the server will listen on*/
			udp::socket listen_socket;
			/* Endpoint for any remote connections */
			udp::endpoint remote_endpoint;
			/* UDP port number to listen on */
			u_int16_t port;
			/* Temporary/unused: data received on the socket */
			char data[1024];
		public:
			/* Default constructor */
			DiscoveryListener(boost::asio::io_service& io_service, u_int16_t  port)
				/* Initialize io_service and socket */
				: io_service(io_service),
				listen_socket(io_service, udp::endpoint(udp::v4(), port))
			{
				/* Handle any incoming connections asynchronously */
				listen_socket.async_receive_from(
					boost::asio::buffer(data, MAX_LENGTH), remote_endpoint,
					/* Bind connection to accept_conn method */
					boost::bind(&DiscoveryListener::accept_conn, this,
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
			}

			/* Constructs node object from remote endpoint and sends a reply */
			void accept_conn(const boost::system::error_code& error, size_t len)
			{
				/* TODO: Construct node object if node is previously unseen */
				listen_socket.async_send_to(
					boost::asio::buffer(data, MAX_LENGTH), remote_endpoint,
					boost::bind(&DiscoveryListener::send_discovery_reply, this,
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
			}

			/* Sends a discovery reply to a remote node */
			void send_discovery_reply(const boost::system::error_code& error, size_t len)
			{

			}
	};
}
