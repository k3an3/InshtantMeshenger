#include <cstdint>
#include <iostream>
#include <string>

#include <parser.h>
/* Whatever the net library is */
// #include <libmeshenger_net.h>

using namespace std;
using namespace libmeshenger;

void PrintMessage(Message m)
{
	/* Print the message (Fully implemented) */
	cout << m.BodyString() << endl;
}

/* Sort of a closure, needed for callback magic */
static Net net;
void ForwardMessageToPeers(Message m)
{
	/* Encapsulate message in packet */
	Packet p(m);
	net.SendToAllPeers(p);
}

int main(int argc, char * argv)
{
	if (argc < 3) {
		cout << "Usage: TestNode <some args>" << endl;
		return 1;
	}

	/* Instantiate a net. The net is static because it
	 * is needed by the "closure" ForwardMessage */
	net = Net(constructorArgs);

	/* add a few peers */
	net.AddPeer("10.0.0.0");
	net.AddPeer("128.8.8.8");

	/* Instantiate a packet engine
	 *
	 * This is 100% functional */
	PacketEngine engine();

	/* Register the two callbacks 
	 *
	 * This is currently 100% functional*/
	MessageState.AddCallback(PrintMessage);
	MessageState.AddCallback(ForwardMessage);

	/* Start listening asynchronously */
	net.StartListen();
	while (true) {
		/* Main loop */

		/* Check for any inbound connections 
		 * This will accept() any connections and
		 * recv() from them (not sure what boost does to
		 * abstract those two functions). Once the connection
		 * hangs up, the net will parse it through ValidatePacket,
		 * check the packet type (should be 0x01), then send it to
		 * the Packet constructor and store it somewhere. GetPacket()
		 * returns a stored packet */
		Message m;
		if (net.CheckAndReceive())
			p = net.GetPacket();

		/* Give message to the engine. If it's a new message, it will
		 * be passed to the callbacks (Print and SendToAllPeers), otherwise
		 * nothing will be done with it.
		 *
		 * This is currently 100% functional */
		engine.ProcessPacket(p);
	}

}
