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

			void reportPacket(std::string identifier);
			void reportNode();
			void reportHop(std::string packet_id, std::string time, std::string depth,
					std::string dest);

		private:
			std::string server_hostname;
			std::string name;

			void doHTTPPost(std::string json_data, std::string route);
	};
}

