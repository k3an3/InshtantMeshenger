#include <cstdint>
#include <iostream>
#include <string>

#include <parser.h>
#include <net.h>
#include <crypto.h>
/* Whatever the net library is */
// #include <libmeshenger_net.h>

using namespace std;
using namespace libmeshenger;

int main(int argc, char ** argv)
{
	if (argc < 3){
		cout << "Usage: TestSender <peer> 'message'" << endl;
		cout << "       TestSender <peer> 'message' <privkey> <pubkey>" << endl;
		return 1;
	}


	string s = string(argv[2]);

	/* Use string -> encoded message constructor */
	ClearMessage m(s);

	/* Use Message -> Packet constructor */
	Packet p(m);

	/* Do encrypted message */
	if (argc == 5) {
		string pubkey_name = argv[4];
		string privkey_name = argv[3];
		cout << "Sending message from " << privkey_name << " to " << pubkey_name << endl;
		CryptoEngine e;
		e.setPrivateKey(CryptoEngine::privkeyFromFile(privkey_name));
		cout << "Sending an encrypted message" << endl;
		EncryptedMessage em(s);
		e.encryptMessage(em, CryptoEngine::pubkeyFromFile(pubkey_name));

		p = Packet(em);
	}

	/* Use message.idString method */
	cout << "Message " << p.idString() << endl;
	cout << "    `" << s << "`" << endl;
	

	string peer(argv[1]);

	/* UDP port, TCP port */
	Net net(5555, 5556);

	net.addPeer(peer);
	net.sendToAllPeers(p);

	cout << "MessageSent!" << endl;
}
