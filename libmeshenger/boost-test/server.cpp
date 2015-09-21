#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <cstdlib>

using boost::asio::ip::tcp;
using namespace std;

const int PORT = 1234;

int
main()
{
    boost::asio::io_service io_service;
    tcp::acceptor acceptor(io_service);
    tcp::endpoint endpoint(tcp::v4(), PORT);
    acceptor.open(endpoint.protocol());
    acceptor.set_option(tcp::acceptor::reuse_address(true));
    //acceptor.set_option(boost::asio::socket_base::broadcast(true));
    tcp::socket socket(io_service);
    acceptor.bind(endpoint);
    acceptor.listen();

    while(1) {
        char recv[1024];
        acceptor.accept(socket);
        tcp::endpoint e = socket.remote_endpoint();
        socket.read_some(boost::asio::buffer(recv));
        cerr << recv << " Got connection from: " << e.address() << "\n";
        boost::asio::write(socket, boost::asio::buffer("hotdog\n", 7));
        socket.close();
    }
}
