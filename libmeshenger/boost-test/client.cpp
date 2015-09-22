#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <cstdlib>

using boost::asio::ip::udp;
using namespace std;

const int PORT = 1234;
const int MAX_LENGTH = 1024;

int
main()
{
    char data[] = "hello world";
    char response[MAX_LENGTH];
    boost::asio::io_service io_service;
    udp::socket socket(io_service, udp::endpoint(udp::v4(), 0));

    socket.set_option(udp::socket::reuse_address(true));
    socket.set_option(udp::socket::broadcast(true));

    udp::endpoint endpoint(boost::asio::ip::address_v4::broadcast(), PORT);
    socket.send_to(boost::asio::buffer(data, MAX_LENGTH), endpoint);
    size_t recv_length = socket.receive_from(boost::asio::buffer(response, MAX_LENGTH), endpoint);
    cout << response;
    socket.close();
}
