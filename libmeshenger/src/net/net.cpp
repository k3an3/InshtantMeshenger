#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <cstdlib>
#include <cstdint>
#include <vector>
#include <string>
#include <mutex>

#include <parser.h>
#include <net.h>
#include <tracker.h>

#define NET_DEBUG true

using boost::asio::ip::udp;
using boost::asio::ip::tcp;
using namespace std;

namespace libmeshenger
{
	void
	netDebugPrint(string s, int color)
	{
		if (NET_DEBUG)
			cout << "\033[1;31m[net]\033[0m-> " <<
				"\033[1;" << color << "m" << s << "\033[0m" << endl;
	}

	/* Net class methods */

	/* Default constructor for Net. Port numbers are used to bind to. */
	Net::Net(uint16_t udp_port,
			uint16_t tcp_port)
		: udp_port(udp_port),
		io_service(),
		tcp_port(tcp_port),
		/* Initialize UDP listen socket on all interfaces */
		udp_listen_socket(io_service),
		tcp_listen_socket(io_service),
		tcp_acceptor(io_service),
		tcp_resolver(io_service),
		tracker("", "")
	{
	}

	/* Run the io_service object in a new thread to facilitate async operations */
	void
	Net::run()
	{
		boost::thread thread(boost::bind(&boost::asio::io_service::run, &io_service));
	}

	void
	Net::enableTracker(Tracker &tracker_)
	{
		tracker = tracker_;
	}

	/* Starts a UDP listener on the provided port. The listener will
	 * create new peer objects upon new connections and responds to the
	 * remote host. */
	void
	Net::discoveryListen()
	{
		udp_listen_socket = udp::socket(io_service, udp::endpoint(udp::v4(), udp_port));
		netDebugPrint("Starting discovery listener...", 43);
		/* Handle any incoming connections asynchronously */
		udp_listen_socket.async_receive_from(
			boost::asio::buffer(data, MAX_LENGTH), udp_remote_endpoint,
			/* Bind connection to acceptDiscoveryConn method */
			boost::bind(&Net::acceptDiscoveryConn, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}

	/* Bind handler for incoming discovery probes */
	void
	Net::acceptDiscoveryConn(const boost::system::error_code& error, size_t recv_len)
	{
		/* Helpful error message */
		if (error) netDebugPrint("ERROR", 33);

		if (recv_len > 0) {
			/* Need to reply to the server port, not the one that was used to
			 * connect to us. Change the remote endpoint's target port. */
			udp_remote_endpoint.port(udp_port);
			/* Socket to send replies with */
			udp::socket socket(io_service, udp::endpoint(udp::v4(), 0));
			socket.set_option(udp::socket::reuse_address(true));

			/* Check if packet is meshenger discovery */
		   	if (!strcmp((char*) data, (char*) MSG)) {
				netDebugPrint("Received discovery probe from peer " +
						udp_remote_endpoint.address().to_string(), 32);
				/* Send a reply to the peer to let it know we exist */
				socket.async_send_to(
					boost::asio::buffer(RESP, strlen((char*) RESP) + 1), udp_remote_endpoint,
					boost::bind(&Net::handleDiscoveryReply, this,
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
				/* Add the peer to the list if it doesn't exist already */
				addPeerIfNew(udp_remote_endpoint.address());
				return;
			/* Check if packet is discovery reply */
			} else if (!strcmp((char*) data, (char*) RESP)) {
				netDebugPrint("Received discovery reply from peer " +
						udp_remote_endpoint.address().to_string(), 33);
				addPeerIfNew(udp_remote_endpoint.address());
			/* Check if packet is discovery disconnect */
			} else if (!strcmp((char*) data, (char*) DIS)) {
				 netDebugPrint("Received discovery disconnect from peer " +
						 udp_remote_endpoint.address().to_string(), 30);
				 peers.erase(peers.begin() + getPeerByAddress(udp_remote_endpoint.address()));
				 netDebugPrint("Removed peer " +
						 udp_remote_endpoint.address().to_string(), 30);
			} else {
				netDebugPrint("Received invalid probe", 30);
			}
			/* Clear the data buffer */
			memset(data, 0, strlen((char*) data));
		}
		/* If we didn't return already, call this function again */
		udp_listen_socket.async_receive_from(
			boost::asio::buffer(data, MAX_LENGTH), udp_remote_endpoint,
			boost::bind(&Net::acceptDiscoveryConn, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}

	void
	Net::addPeerIfNew(boost::asio::ip::address ip_addr)
	{
		/* Check if peer is in the peer list */
		if (!peerExistsByAddress(ip_addr)) {
			/* Add it to the peer list */
			addPeer(ip_addr);
			netDebugPrint("Added new peer " +
					ip_addr.to_string(), 32);
		} else {
			 netDebugPrint("Peer already exists at " +
					 ip_addr.to_string(), 30);
		}
	}

	/* Not the best way to do this */
	bool
	Net::peerExistsByAddress(boost::asio::ip::address ip_addr)
	{
		for(auto &peer : peers)
			if (peer.ip_addr == ip_addr) return true;
		return false;
	}

	/* Again, placeholder; needs optimization. Consider
	 * merging with peerExistsByAddress */
	uint32_t
	Net::getPeerByAddress(boost::asio::ip::address ip_addr)
	{
		for(int i = 0; i< peers.size(); i++)
			if (peers[i].ip_addr == ip_addr) return i;
	}

	/* Bind handler for sending discovery replies */
	void
	Net::handleDiscoveryReply(const boost::system::error_code& error, size_t send_len)
	{
		if (error) netDebugPrint("ERROR", 31);
		netDebugPrint("Sending discovery reply to " + udp_remote_endpoint.address().to_string(), 33);
		/* Send a reply to the remote endpoint and continue listening for discovery */
		udp_listen_socket.async_receive_from(
			boost::asio::buffer(data, MAX_LENGTH), udp_remote_endpoint,
			boost::bind(&Net::acceptDiscoveryConn, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}

	/* Discover peers on the LAN using UDP broadcast */
	void
	Net::discoverPeers()
	{
		netDebugPrint("Sending discovery probe...", 42);
		sendUDPBroadcast(MSG, strlen((char*)MSG));
	}

	void
	Net::sendUDPBroadcast(const uint8_t* message, uint32_t length)
	{
		/* Create the socket that will send UDP broadcast */
		udp::socket socket(io_service, udp::endpoint(udp::v4(), 0));

		/* Set socket options so that it can reuse the host's address and send
		* broadcast */
		socket.set_option(udp::socket::reuse_address(true));
		socket.set_option(udp::socket::broadcast(true));

		/* Create endpoint for the connections */
		udp::endpoint endpoint(boost::asio::ip::address_v4::broadcast(), udp_port);

		/* Send broadcast packet asynchronously */
		socket.async_send_to(boost::asio::buffer(message, length + 1),
				endpoint, [this](boost::system::error_code ec,
					size_t bytes_transferred)
			   	{
					/* Empty lambda function */
				});
		/* Make sure we close the socket, as it is not needed anymore */
		socket.close();
	}

	Packet
	Net::getPacket()
	{
		/* Lock mutex so packet list can't be modified from the other thread */
		io_mutex.lock();
		/* Save and pop a packet from the back of the list */
		Packet p = packets.back();
		packets.pop_back();
		io_mutex.unlock();
		return p;
	}

	void
	Net::addPeer(Peer p)
	{
		/* Add a Peer to the peer list */
		peers.push_back(p);
		tracker.reportPeering(p.ip_addr.to_string());
	}

	void
	Net::addPeer(string hostname)
	{
		/* Add the peer to the peer list */
		peers.push_back(Peer(resolveSingle(hostname).address()));
	}

	tcp::endpoint
	Net::resolveSingle(string s)
	{
		/* Create a resolver so string s can be resolved as either an IP
		 * address or a hostname */
		tcp::resolver::query query(s, "");
		/* Resolve the string into an iterator of endpoint objects */
        try {
            tcp::resolver::iterator iter = tcp_resolver.resolve(query);
            tcp::resolver::iterator end;
            /* Return the first endpoint in the iterator */
            return *iter++;
        } catch (exception &e) {
            netDebugPrint(e.what(), 31);
        }
    }

	std::vector<Peer>
	Net::getPeers()
	{
		return std::vector<Peer>(peers.begin(), peers.end());
	}

    string
    Net::getHostname()
    {
        return boost::asio::ip::host_name();
    }

    boost::asio::ip::address
    Net::get_ifaddr(string remote_host)
    {
		/* Only real portable way to get our IP address. This will most likely
		 * obtain the IP address of the interface that has the default route.
		 * We create a socket, open a connection to something (i.e. MeshTrack or Google),
		 * and get the IP address from the resultant local endpoint. */
		tcp::socket sock(io_service);
        boost::asio::ip::address addr;
        try {
            tcp::endpoint e = resolveSingle(remote_host);
            /* Without modifying the port, connect will try to connect to an arbitrary
             * port, which will fail. */
            e.port(80); // TODO: Default to 80, override if :<portnum> in remote_host
            sock.connect(e);
            addr = sock.local_endpoint().address();
        } catch (exception &e) {
            netDebugPrint(e.what(), 31);
        }
        if (sock.is_open())
            sock.close();
		return addr;
    }

	/* Sends a Packet to all previously discovered peers using TCP */
	void
	Net::sendToAllPeers(Packet p)
	{
		/* Increment the depth of a packet (tx counter) */
		p.setDepth(p.depth() + 1);
		netDebugPrint("Packet now has depth of " + to_string(p.depth()));

		/* Cycle through the peers vector and prepare to send */
		for(int i = 0; i < peers.size(); i++) {
			/* Peer IP address */
			boost::asio::ip::address addr = peers[i].ip_addr;

			/* Endpoint for the peer */
			tcp::endpoint endpoint(addr, tcp_port);

			/* Socket used to create the connection */
			tcp::socket sock(io_service);

			/* Try to connect and report any connection errors */
			netDebugPrint("Sending packet to " +
					endpoint.address().to_string(), 35);
			try {
				/* Open a TCP connection async */
				sock.async_connect(endpoint, [this,&sock,p,i]
						(boost::system::error_code ec)
					{
					});
				/* Send the data (not fun) */
				boost::asio::write(sock, boost::asio::buffer(p.raw().data(),
							p.raw().size()));
				peers[i].strikes = 0;
			} catch(std::exception &e) {
					/* Handle connection errors and remove problematic peers */
					netDebugPrint(e.what(), 41);
					netDebugPrint("Peer " + addr.to_string() +
							" is problematic. Strike.", 33);
					peers[i].strikes++;

					/* Remove peer if it fails to be reached 3 times */
					if (peers[i].strikes >= 3) {
						netDebugPrint("Three strikes. Removing.", 31);
						peers.erase(peers.begin() + i);
					}
					if (sock.is_open())
						sock.close();
			}
		}
	}

	/* Start a TCP acceptor to accept incoming packets */
	void
	Net::startListen()
	{
		/* If the acceptor is closed, open it */
		if (!tcp_acceptor.is_open()) {
			tcp_acceptor = tcp::acceptor(io_service, tcp::endpoint(tcp::v4(), tcp_port));
		}
		/* Start accepting asynchronously */
		tcp_acceptor.async_accept(tcp_listen_socket,
				[this](boost::system::error_code ec)
		{
			try {
				if (!ec) {
					/* Read from the socket and create a packet object from the data */
					size_t bytes = tcp_listen_socket.read_some(boost::asio::buffer(msg, MAX_LENGTH));
					vector<uint8_t> v(msg, msg + bytes);
					if (ValidatePacket(v)) {
						/* Lock the mutex so the other thread doesn't modify
						 * the packet list */
						io_mutex.lock();
						/* Add the packet to the packet list */
						packets.push_back(Packet(v));
						io_mutex.unlock();
					}
				}
			} catch (std::exception &e) {
				cerr << e.what() << endl;
			}

			/* Close the socket if it is still open */
			if (tcp_listen_socket.is_open())
				tcp_listen_socket.close();

			/* Call function again */
			startListen();
		});
	}

	/* Accept TCP connections on tcp_port and attempt to create a valid packet
	 * if possible. Adds packet to the packets vector and returns the length
	 * of the packet. */
	uint16_t
	Net::receivePacket()
	{
		// Change this?
		// This is where the threadsplosion is happening
		// packets is modified both by the main thread (which calls
		// receivePacket() and by the asio thread handling receives
		// with the lambda function defined in start_listen
		//
		// Some mutex/queue magic must be used here to make sure nothing
		// explodes between that lambda (running in the asio thread) and
		// this function (running in the main thread)
		//
		// Also, get rid of all the cout in the lambda function. That's a thread
		// violation too.
		io_mutex.lock();
		uint16_t t = packets.size();
		io_mutex.unlock();
		return t;
	}

	/* Peer class methods */
	Peer::Peer(boost::asio::ip::address ip_addr)
		: ip_addr(ip_addr),
		strikes(0)
	{

	}

	Peer::Peer(std::string s)
		: strikes(0)
	{
		ip_addr = boost::asio::ip::address::from_string(s);
	}
}
