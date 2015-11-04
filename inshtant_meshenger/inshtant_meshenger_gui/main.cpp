#include "mainwindow.h"
#include <QApplication>

#include <parser.h>
#include <state.h>
#include <net.h>

using namespace libmeshenger;
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
    MainWindow w(0, &net);
    win = &w;
    PacketEngine engine;

    engine.AddCallback(ForwardPacketToPeers);
    engine.AddCallback(displayMessageHandler);

    /* Start listening asynchronously */
    net.discoveryListen();
    net.discoverPeers();

    net.startListen();
    net.run();

    w.show();
    a.exec();

    while (true) {
        uint16_t numPackets = net.receivePacket();
        if (numPackets) {
            for (int i = 0; i < numPackets; i++) {
                Packet p = net.getPacket();
                std::cout << "Packet received from net" << std::endl;
                engine.ProcessPacket(p);
            }
        }
    }
    return a.exec();
}
