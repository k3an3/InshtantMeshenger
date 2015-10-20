#include <cstdint>
#include <iostream>
#include <string>

#include <parser.h>
#include <net.h>
/* Whatever the net library is */
// #include <libmeshenger_net.h>

using namespace std;
using namespace libmeshenger;

int main(int argc, char ** argv)
{
	if (argc < 3){
		cout << "Usage: TestSender <peer> 'message'" << endl;
		return 1;
	}


	string s = string(argv[2]);

	/* Use string -> encoded message constructor */
	ClearMessage m(s);

	/* Use Message -> Packet constructor */
	Packet p(m);

	/* Use message.idString method */
	cout << "Message " << m.idString() << endl;
	cout << "    `" << s << "`" << endl;
	

	string peer(argv[1]);

	/* UDP port, TCP port */
	Net net(5555, 5556);

	net.addPeer(peer);
	net.sendToAllPeers(p);

	cout << "MessageSent!" << endl;
}
