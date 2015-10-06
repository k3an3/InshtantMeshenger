#include <iostream>
#include <cstdint>
#include <string>
#include <boost/asio.hpp>

#include <net.h>

using namespace std;
using namespace libmeshenger;

int
main(int argc, char** argv)
{
    cout << "Starting test server...\n";
    boost::asio::io_service io_service;
    Net net(io_service, 1234, 1235);

    net.discoveryListen();
    net.discoverPeers();
    io_service.run();
}
