#define BOOST_NETWORK_NO_LIB
#include <boost/network/protocol/http/client.hpp>
#include <string>
#include <iostream>

#include <parser.h>
#include <tracker.h>

using namespace boost::network;
using namespace boost::network::http;

namespace libmeshenger
{
	Tracker::Tracker(std::string server_hostname, std::string name)
		: server_hostname(server_hostname),
		name(name)
	{

	}

	void Tracker::reportPacket(std::string identifier)
	{
		doHTTPPost("{\"packet\":{\"identifier\":\"" + identifier + "\"}}", "packets");
	}

	void Tracker::reportNode()
	{
		doHTTPPost("{\"node\":{\"name\":\"" + name + "\"}}", "nodes");
	}

	void Tracker::reportHop(std::string packet_id, std::string time, std::string depth,
			std::string dest)
	{
		doHTTPPost("{\"hop\": {\"destination\": \"" + dest + "\", "
				"\"origin\": \"" + name + "\", \"packet\": \"" + packet_id + "\", "
				 "\"time\": \"" + time + "\", \"depth\": \"" + depth + "\"}}", "hops");
	}

	void Tracker::doHTTPPost(std::string json_data, std::string route)
	{
		client::request req(server_hostname + "/" + route);
		req << header("Connection", "close");
		req << header("Content-Type", "application/json");
		req << header("Content-Length", std::to_string(json_data.length()));
		req << body(json_data);
		client::response resp = client().post(req); // Do something with this? Or not
	}
}
