#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <cstdlib>
#include <cstdint>
#include <vector>

using boost::asio::ip::udp;
using namespace std;

namespace libmeshenger
{
	/* Peer class */
	class Peer final
	{
		public:
			/* The peer's IP address */
			boost::asio::ip::address ip_addr;

			/* Default constructor */
			Peer(boost::asio::ip::address ip_addr);
	};

	/* Networking class */
	class Net final
	{
		private:
			/* IO service for async asio operations */
			boost::asio::io_service& io_service;
			/* Socket the server will listen on*/
			udp::socket listen_socket;
			/* Endpoint for any remote connections */
			udp::endpoint remote_endpoint;
			/* UDP port number to listen on */
			uint16_t udp_port;
			/* TCP port number to listen on */
			uint16_t tcp_port;
			/* Temporary/unused: data received on the socket */
			uint8_t data[1024];

			vector<Peer> peers;

			bool peerExistsByAddress(boost::asio::ip::address ip_addr);
			void acceptDiscoveryConn(const boost::system::error_code& error, size_t len);
			void send_discovery_reply(const boost::system::error_code& error, size_t len);
		public:
			/* Construct with io_service object */
			Net(boost::asio::io_service& io_service, uint16_t udp_port, uint16_t tcp_port);

			/* Starts a UDP listener on the provided port. The listener will
			 * create new peer objects upon new connections and responds to the
			 * remote host. */
			void discoveryListen();

			/* Sends a UDP broadcast to discover peers. Received replies will
			 * be used to construct peer objects. */
			void discoverPeers();

	};

}