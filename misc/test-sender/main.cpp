#include <cstdint>
#include <iostream>
#include <string>

#include <parser.h>
/* Whatever the net library is */
// #include <libmeshenger_net.h>

using namespace std;
using namespace libmeshenger;

int main(int argc, char * argv)
{
	if (argc < 3){
		cout << "Usage: TestSender <peer> 'message'" << endl;
		return 1;
	}


	string s = argv[2];

	/* Use string -> encoded message constructor */
	Message m(s);

	/* Use Message -> Packet constructor */
	Packet p(s);

	/* Use message.idString method */
	cout << "Message " << m.idString() << endl;
	cout << "    `" << s << "`" << endl;
	

	/* Depends on the following unipmelement funtionality:
	 *  - a LibMeshenger::Net class with
	 *  	- addPeer(std::string)
	 *  	- SendToAllPeers(LibMeshenger::Packet)
	 * */
	string peer(argv[1]);
	Net net(NetConstructorArgs);
	net.AddPeer(peer);
	net.SendToAllPeers(p);

	cout << "MessageSent!" << endl;
}
