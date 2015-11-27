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

			void reportPacket(/* TODO */);
			void reportNode();
			void reportHop(/* TODO */);

		private:
			std::string server_hostname;
			std::string name;

			void doHTTPPost(std::string json_data, std::string route);
	};
}

