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
	/* Unimplemented BodyString method */
	cout << m.BodyString() << endl;
}

int main(int argc, char * argv)
{
	if (argc < 3) {
		cout << "Usage: TestNode <some args>" << endl;
		return 1;
	}

	/* Instantiate a net */
	Net net(constructorArgs);

	/* add a few peers */
	net.AddPeer();
	net.AddPeer();

	/* Instantiate a MessageState */
	MessageState engine(args);

	/* Register the two callbacks */
	MessageState.registerCallback(PrintMessage);
	MessageState.registerCallback(net.SendToAllPeers);

	net.StartListen();
	while (true) {
		/* Main loop */

		/* Check for any inbound connections */
		Message m;
		if (net.CheckIncoming())
			m = net.GetMessage();

		/* Give message to the engine. If it's a new message, it will
		 * be passed to the callbacks (Print and SendToAllPeers), otherwise
		 * nothing will be done with it */
		engine.ProcessMessage(m);
	}

}
