#include <cstdint>
#include <iostream>
#include <string>

#include <parser.h>
#include <state.h>
#include <net.h>
#include <crypto.h>

using namespace std;
using namespace libmeshenger;

void PrintMessage(Packet& p)
{
	if (p.type() == 0x01) {
		ClearMessage m(p);

		/* Print the message (Fully implemented) */
		cout << "\033[1;32m[Cleartext Message received!]\033[0m ";
		cout << m.bodyString() << endl;
	}
}

/* CryptoEngine (used by CB)*/
CryptoEngine cryptoEngine;
void PrintEncryptedMessage(Packet& p)
{
	if (p.type() == 0x02) {
		EncryptedMessage em(p);

		cout << "\033[1;32m[Encrypted Message Received!]\033[0m ";

		if (cryptoEngine.tryDecrypt(em)) {
			if (em.trusted()) {
				vector<uint8_t> body = em.decryptedBody();
				cout << "\033[1;32m[" << cryptoEngine.buddy(em.sender()).name();
			} else {
				cout << "\033[1;31m[NOT TRUSTED\033[0m";
			}
			cout << "]\033[0m" << endl;
			cout << string((char *) em.decryptedBody().data()) << endl;
		} else {
			cout << "\033[1;31mUnable to decrypt!\033[0m" << endl;
		}
	}
}

/* Sort of a closure, needed for callback magic */
static Net net(5555, 5556);
void ForwardPacketToPeers(Packet& p)
{
	cout << "\033[1;32m[Forwarding packet to peers!]\033[0m" << endl;
	/* Encapsulate message in packet */
	net.sendToAllPeers(p);
}

int main(int argc, char** argv)
{

	if (argc < 2) {
		cout << "Usage: TestNode <privkeyfile> [buddy] ..." << endl;
		return -1;
	}

	/* Instantiate a packet engine
	 *
	 * This is 100% functional */
	PacketEngine engine;

	/* Set the private key */
	cryptoEngine.setPrivateKeyFromFile(argv[1]);

	/* Register the two callbacks
	 *
	 * This is currently 100% functional*/
	engine.AddCallback(PrintMessage);
	engine.AddCallback(ForwardPacketToPeers);
	engine.AddCallback(PrintEncryptedMessage);

	/* Start listening asynchronously */
    net.discoveryListen();
    net.discoverPeers();

	/* Add buddies */
	for(int i = 2; i < argc; i++) {
		string buddy_name = argv[i];
		string filename = buddy_name + ".pub";
		CryptoPP::RSA::PublicKey pubkey = CryptoEngine::pubkeyFromFile(filename);
		cryptoEngine.addBuddy(Buddy(pubkey, buddy_name));
		cout << "Added buddy: " << buddy_name << ". Pubkey: " << endl;
		cout << CryptoEngine::pubkeyToBase64(pubkey) << endl;
	}

    net.startListen();
	//net.receivePacket();
    net.run();
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
		uint16_t numPackets = net.receivePacket();
		if (numPackets) {
			for (int i = 0; i < numPackets; i++) {
				Packet p = net.getPacket();
				engine.ProcessPacket(p);
			}
        }

		/* Give message to the engine. If it's a new message, it will
		 * be passed to the callbacks (Print and SendToAllPeers), otherwise
		 * nothing will be done with it.
		 *
		 * This is currently 100% functional */
	}

}
