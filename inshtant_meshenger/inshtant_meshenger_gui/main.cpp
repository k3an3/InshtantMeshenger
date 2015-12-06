#include "mainwindow.h"
#include <QApplication>

#include <parser.h>
#include <state.h>
#include <net.h>
#include <tracker.h>

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

Tracker tracker("http://meshtrack.pqz.us", net.get_ifaddr("meshtrack.pqz.us").to_string());
static void ReportHop(Packet& p)
{
	cout << "\033[132m<Reporting hops\033[0m" << endl;
	for(auto &peer : net.getPeers()) {
		tracker.reportHop(p.idString(), to_string(p.depth()), peer.ip_addr.to_string());
	}
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PacketEngine engine;
	CryptoEngine cryptoEngine;
    MainWindow w(0, net, engine, cryptoEngine, tracker);
    win = &w;

    engine.AddCallback(ForwardPacketToPeers);
    engine.AddCallback(displayMessageHandler);
    engine.AddCallback(ReportHop);

	tracker.reportNode();


    /* Start listening asynchronously */
    net.discoveryListen();
    net.discoverPeers();

	/* Set your private key. Replace this with UI functionality to set a private
	 * key from Base64 input */
	cryptoEngine.setPrivateKeyFromFile(argv[1]);

	/* Add buddies. Replace this with UI functionality to add buddies from
	 * Base64 public keys */
	/* Also adds peers. That should be GUIfied as well */
    for(int i = 2; i < argc; i++) {
        /* It is actually a peer. Parsing args is hard */
        if (string(argv[i], argv[i] + 2) == string("-P")) {
            cout << "Adding peer " << argv[i] + 2 << endl;
            net.addPeer(argv[i] + 2);
        } else {
            string buddy_name = argv[i];
            string filename = buddy_name + ".pub";
            CryptoPP::RSA::PublicKey pubkey = CryptoEngine::pubkeyFromFile(filename);
            cryptoEngine.addBuddy(Buddy(pubkey, buddy_name));
            cout << "Added buddy: " << buddy_name << ". Pubkey: " << endl;
            cout << CryptoEngine::pubkeyToBase64(pubkey) << endl;
        }
    }

    net.startListen();
    net.run();

    w.show();
    return a.exec();
}
