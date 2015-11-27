#include <boost/network/protocol/http/client.hpp>
#include <string>
#include <iostream>

#include <parser.h>
#include <tracker.h>

using namespace std;

namespace libmeshenger
{
    Tracker::Tracker(string server_hostname, string name)
        : server_hostname(server_hostname),
        name(name)
    {

    }

    void Tracker::reportPacket()
    {

    }

    void Tracker::reportNode()
    {

    }

    void Tracker::reportHop()
    {

    }

    void Tracker::doHTTPPost(string json_data, string route)
    {
        boost::network::http::client::request req(server_hostname + "/" + route);
        req << header("Connection", "close");
        req << header("Content-Type", "application/json");
        boost::network::client client;
        boost::network::http::client::response resp = client.post(req); // Do something with this? Or not
    }
}
