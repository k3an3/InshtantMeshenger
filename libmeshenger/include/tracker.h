#ifndef TRACKER_H
#define TRACKER_H
#include <boost/network/protocol/http/client.hpp>
#include <string>

#include <parser.h>

namespace libmeshenger
{
	class Tracker final
	{
		public:
			/* Default constructor */
			Tracker(std::string server_hostname, std::string name);

			/* Methods to format various data objects into JSON and pass them
			 * to doHTTPPost() */
			void reportPacket(std::string identifier);
			void reportNode();
			void reportHop(std::string packet_id, std::string depth,
					std::string dest);

		private:
			/* Hostname/address of the tracking server */
			std::string server_hostname;
			/* Hostname/address of this meshenger */
			std::string name;
			/* HTTP client used for connections */
			boost::network::http::client client;

			/* Sends JSON formatted data to the tracking server at the specified
			 * route */
			void doHTTPPost(std::string json_data, std::string route);
	};
}
#endif
