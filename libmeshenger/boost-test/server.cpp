#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <cstdlib>
#include <cstdint>

using boost::asio::ip::udp;
using namespace std;

const int PORT = 1234;
const int MAX_LENGTH = 1024;

int
main()
{
	boost::asio::io_service io_service;
	udp::socket socket(io_service, udp::endpoint(udp::v4(), PORT));

	while(1) {
		char data[MAX_LENGTH];
		udp::endpoint remote_endpoint;
		size_t recv_length = socket.receive_from(boost::asio::buffer(data, MAX_LENGTH), remote_endpoint);
		cerr << remote_endpoint.address() << ": " << data << "\n";
		socket.send_to(boost::asio::buffer("hotdog\n", 7), remote_endpoint);
	}
}
