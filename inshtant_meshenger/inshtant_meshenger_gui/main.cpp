#include "mainwindow.h"
#include <QApplication>

#include <parser.h>
#include <state.h>
#include <net.h>

using namespace libmeshenger;
using namespace std;
MainWindow *win;

static Net net(5555, 5556);

void ForwardPacketToPeers(Packet& p)
{
    /* Encapsulate message in packet */
    net.sendToAllPeers(p);
}

void displayMessageHandler(Packet& p)
{
    win->displayMessage(p);
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PacketEngine engine;
	CryptoEngine cryptoEngine;
    MainWindow w(0, net, engine, cryptoEngine);
    win = &w;

    engine.AddCallback(ForwardPacketToPeers);
    engine.AddCallback(displayMessageHandler);

    /* Start listening asynchronously */
    net.discoveryListen();
    net.discoverPeers();

	/* Remove this. Replace with UI elements that add peer dynamically */
    net.addPeer("129.186.205.235");
	
	/* Set your private key. Replace this with UI functionality to set a private
	 * key from Base64 input */
	cryptoEngine.setPrivateKeyFromFile(argv[1]);

	/* Add buddies. Replace this with UI functionality to add buddies from
	 * Base64 public keys */
    for(int i = 2; i < argc; i++) {
        string buddy_name = argv[i];
        string filename = buddy_name + ".pub";
        CryptoPP::RSA::PublicKey pubkey = CryptoEngine::pubkeyFromFile(filename);
        cryptoEngine.addBuddy(Buddy(pubkey, buddy_name));
        cout << "Added buddy: " << buddy_name << ". Pubkey: " << endl;
        cout << CryptoEngine::pubkeyToBase64(pubkey) << endl;
    }

    net.startListen();
    net.run();

    w.show();
    return a.exec();
}
