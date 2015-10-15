#include <cstdint>
#include <iostream>
#include <string>

#include <parser.h>
#include <state.h>
#include <net.h>

using namespace std;
using namespace libmeshenger;

void PrintMessage(ClearMessage& m)
{
	/* Print the message (Fully implemented) */
	cout << m.bodyString() << endl;
}

/* Sort of a closure, needed for callback magic */
static Net net(5555, 5556);
void ForwardMessageToPeers(ClearMessage& m)
{
	/* Encapsulate message in packet */
	Packet p(m);
	net.sendToAllPeers(p);
}

int main(int argc, char** argv)
{

	/* Instantiate a net. The net is static because it
	 * is needed by the "closure" ForwardMessage */

	/* add a few peers */
	for (int i = 1; i < argv; i++) {
		net.addPeer(argv[i]);
	}

	/* Instantiate a packet engine
	 *
	 * This is 100% functional */
	PacketEngine engine;

	/* Register the two callbacks
	 *
	 * This is currently 100% functional*/
	engine.AddCallback(PrintMessage);
	engine.AddCallback(ForwardMessageToPeers);

	/* Start listening asynchronously */
	//net.StartListen();
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
		if (net.receivePacket()) {
			Packet p = net.getPacket();
            engine.ProcessPacket(p);
        }

		/* Give message to the engine. If it's a new message, it will
		 * be passed to the callbacks (Print and SendToAllPeers), otherwise
		 * nothing will be done with it.
		 *
		 * This is currently 100% functional */
	}

}
