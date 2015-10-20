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
    Net net(5555, 5556);

    net.discoveryListen();
    net.discoverPeers();
    net.run();
}
