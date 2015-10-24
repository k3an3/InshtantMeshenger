#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <cstdlib>
#include <cstdint>
#include <vector>

#include <parser.h>

using boost::asio::ip::udp;
using boost::asio::ip::tcp;

namespace libmeshenger
{

	void netDebugPrint(std::string s, int color = 0);

	/* Peer class */
	class Peer final
	{
		public:
			/* The peer's IP address */
			boost::asio::ip::address ip_addr;
			/* Number of strikes */
			std::uint8_t strikes;

			/* Default constructor */
			Peer(boost::asio::ip::address ip_addr);
			/* Construct from IP address as string */
			Peer(std::string);
	};

	/* Networking class */
	class Net final
		: public std::enable_shared_from_this<Net>
	{
		private:
			/* IO service for async asio operations */
			boost::asio::io_service io_service;
			/* UDP socket the discovery server will listen on*/
			udp::socket listen_socket;
			/* Endpoint for any remote UDP connections */
			udp::endpoint remote_endpoint;
			/* TCP socket */
			tcp::socket msg_socket;
			tcp::acceptor msg_acceptor;
			/* UDP port number to listen on */
			std::uint16_t udp_port;
			/* TCP port number to listen on */
			std::uint16_t tcp_port;
			/* Data received on the socket */
			std::uint8_t data[1024], msg[1024]; // shouldn't be hardcoded
			boost::thread thread;

			std::vector<Peer> peers;
			std::vector<Packet> packets;

			bool peerExistsByAddress(boost::asio::ip::address ip_addr);
			void acceptDiscoveryConn(const boost::system::error_code& error, size_t len);
			void handleDiscoveryReply(const boost::system::error_code& error, size_t len);
			void addPeerIfNew(boost::asio::ip::address ip_addr);
			void discoveryHandler(const boost::system::error_code& error,
					  std::size_t bytes_transferred);
			void actuallyRun();
		public:
			/* Construct a Net */
			Net(uint16_t udp_port, uint16_t tcp_port);

			uint16_t receivePacket();
			void run();
			void actuallyrun();

			void startListen();
			void messageAcceptor();

			/* Starts a UDP listener on the provided port. The listener will
			 * create new peer objects upon new connections and responds to the
			 * remote host. */
			void discoveryListen();

			/* Sends a UDP broadcast to discover peers. Received replies will
			 * be used to construct peer objects. */
			void discoverPeers();

			void addPeer(Peer);
			void addPeer(std::string);

			std::vector<Peer> getPeers();

			void sendToAllPeers(Packet p);

			/* Starts a TCP listener to receive any packets sent on the wire */

			Packet getPacket();
	};

}
