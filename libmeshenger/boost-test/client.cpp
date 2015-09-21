#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <cstdlib>

using boost::asio::ip::tcp;

const int PORT = 1234;

int
main()
{
    boost::asio::io_service io_service;
    tcp::socket socket(io_service);
    tcp::resolver resolver(io_service);
    boost::asio::connect(socket, resolver.resolve({"127.0.0.1", PORT}));
}
