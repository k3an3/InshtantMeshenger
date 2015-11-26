#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <cstdlib>
#include <cstdint>
#include <vector>
#include <mutex>

#include <parser.h>

using boost::asio::ip::udp;
using boost::asio::ip::tcp;

const int32_t MAX_LENGTH = 1024;
const uint8_t RESP[] = "meshenger-discovery-reply";
const uint8_t MSG[] = "meshenger-discovery-probe";
const uint8_t DIS[] = "meshenger-disconnect";

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
	{
		private:
			/* IO service for async asio operations */
			boost::asio::io_service io_service;
			/* UDP socket the discovery server will listen on*/
			udp::socket udp_listen_socket;
			/* Endpoint for any remote UDP connections */
			udp::endpoint udp_remote_endpoint;
			/* TCP socket for accepting messages */
			tcp::socket tcp_listen_socket;
			/* TCP acceptor used with tcp_listen_socket */
			tcp::acceptor tcp_acceptor;
			/* TCP resolver to resolve hostnames */
			tcp::resolver tcp_resolver;
			/* UDP port number to listen on for discovery*/
			std::uint16_t udp_port;
			/* TCP port number to listen on for packets*/
			std::uint16_t tcp_port;
			/* Data received on the sockets */
			std::uint8_t data[1024], msg[1024]; // shouldn't be hardcoded
			/* Thread used to run the io_service */
			boost::thread thread;
			/* Mutex for threading */
			std::mutex io_mutex;

			/* Peer list */
			std::vector<Peer> peers;
			/* Packet list */
			std::vector<Packet> packets;

			bool peerExistsByAddress(boost::asio::ip::address ip_addr);
			std::uint32_t getPeerByAddress(boost::asio::ip::address ip_addr);
			void acceptDiscoveryConn(const boost::system::error_code& error, size_t len);
			void handleDiscoveryReply(const boost::system::error_code& error, size_t len);
			void addPeerIfNew(boost::asio::ip::address ip_addr);
			void sendUDPBroadcast(const uint8_t* message, uint32_t length);
		public:
			/* Default net constructor */
			Net(uint16_t udp_port, uint16_t tcp_port);

			uint16_t receivePacket();
			void run();

			void startListen();

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

	/* Main Net exception class. Should be thrown when a critical error occurs,
	* such as when there is no network connection or the network is unreachable */
	class NetworkException final : public std::runtime_error
	{
		 public:
			 NetworkException(std::string const& error);
			 NetworkException();

	};

}
