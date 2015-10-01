#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <cstdlib>
#include <cstdint>
#include <string>

using boost::asio::ip::udp;
using namespace std;

int32_t MAX_LENGTH = 1024;

class Listener
{
    public:
        Listener(boost::asio::io_service& io_service, u_int16_t  port_)
            : io_service(io_service),
			listen_socket(io_service, udp::endpoint(udp::v4(), port))
		{
			listen_socket.async_receive_from(
				boost::asio::buffer(data, MAX_LENGTH), remote_endpoint,
				boost::bind(&Listener::accept_conn, this, 0, 0));
		}

		void accept_conn(const boost::system::error_code& error, size_t len)
		{

		}
	private:
		boost::asio::io_service& io_service;
		udp::socket listen_socket;
		udp::endpoint remote_endpoint;
		u_int16_t port;
		string data;
};
