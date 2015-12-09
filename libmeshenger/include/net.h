#ifndef NET_H
#define NET_H
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <cstdlib>
#include <cstdint>
#include <vector>
#include <mutex>

#include <parser.h>
#include <tracker.h>

using boost::asio::ip::udp;
using boost::asio::ip::tcp;

const int32_t MAX_LENGTH = 1048576;
const uint8_t RESP[] = "meshenger-discovery-reply";
const uint8_t MSG[] = "meshenger-discovery-probe";
const uint8_t DIS[] = "meshenger-disconnect";

namespace libmeshenger
{

        void netDebugPrint(std::string s, int color);

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
			/* ## Private member variables ## */

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
			/* TCP port number to listen on for message packets*/
			std::uint16_t tcp_port;
			/* Data received on the sockets */
			std::uint8_t data[MAX_LENGTH], msg[MAX_LENGTH];
			/* Thread used to run the io_service */
			boost::thread thread;
			/* Mutex for threading safety */
			std::mutex io_mutex;
			/* Peer list */
			std::vector<Peer> peers;
			/* Packet list */
			std::vector<Packet> packets;
			/* Tracker */
			Tracker tracker;

			/* ## Private member functions ## */

			/* ### Networking functions ### */

			/* Sends a meshenger discovery probe to all hosts on the network
			* using the broadcast address */
			void sendUDPBroadcast(const uint8_t* message, uint32_t length);
			/* Sends a discovery reply to a specific peer. */
			void handleDiscoveryReply(const boost::system::error_code& error, size_t len);

			/* Given an IP address, returns a matching Peer object (if it exists) */
			std::uint32_t getPeerByAddress(boost::asio::ip::address ip_addr);

			/* Method that continuiously runs and handles incoming discovery packets */
			void acceptDiscoveryConn(const boost::system::error_code& error, size_t len);

			/* ### Helper functions ### */

			/* Adds a Peer by IP address if they are not already in the peer list */
			void addPeerIfNew(boost::asio::ip::address ip_addr);

			/* Returns whether or not a peer is already in the peer list,
			/* given an IP address */
			bool peerExistsByAddress(boost::asio::ip::address ip_addr);

			tcp::endpoint resolveSingle(std::string s);

		public:
			/* ## Public member functions ## */

			/* Default net constructor. Requires UDP and TCP ports to bind to. */
			Net(uint16_t udp_port, uint16_t tcp_port);

			/* Runs the io_service object in a new thread to facilitate
			 * boost asio's async functionality */
			void run();

			/* Sends out a discovery disconnect message and cleans up the
			 * sockets */
			void shutdown();

			/* Enable the tracker */
			void enableTracker(Tracker &tracker_);

			std::string getHostname();
			boost::asio::ip::address get_ifaddr(std::string remote_host);

			/* ### Networking methods ### */

			/* Starts a TCP acceptor to accept incoming packets. Validates and
			 * adds packets to the packet list when they arrive. */
			void startListen();

			/* Starts a UDP listener on the provided port. The listener will
			 * create new peer objects upon new connections and responds to the
			 * remote host. */
			void discoveryListen();

			/* Sends a UDP broadcast to discover peers. Received replies will
			 * be used to construct peer objects. */
			void discoverPeers();

			/* ### Methods for packet and peer list manipulation ### */

			/* Returns the number of packets in the list that are waiting to be
			 * processed */
			uint16_t receivePacket();

			/* Pushes a Peer onto the peer list */
			void addPeer(Peer);

			/* Constructs and pushes a new Peer onto the peer list */
			void addPeer(std::string);

			/* Returns a vector containing the peer list */
			std::vector<Peer> getPeers();

			/* Given a packet, the packet is sent to all peers in the peer list */
			void sendToAllPeers(Packet p);

			/* Pops and returns a packet from the back of the packet vector */
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
#endif
