#include <QFrame>
#include <QPalette>
#include <QtCore>
#include <QtGui>
#include <QTimer>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "addpeer.h"
#include "ui_addpeer.h"
#include "pgp.h"
#include "ui_pgp.h"
#include "addbuddy.h"

#include<vector>
#include <crypto.h>
#include <parser.h>
#include <state.h>
#include <net.h>
#include <tracker.h>
#include <string>

using namespace libmeshenger;
using namespace std;

MainWindow::MainWindow(QWidget *parent, libmeshenger::Net &net_p, libmeshenger::PacketEngine &engine_p, libmeshenger::CryptoEngine &crypto_p, Tracker &tracker_p, QSettings &settings_p):
    QMainWindow(parent),
    ui(new Ui::MainWindow),
	net(net_p),
	engine(engine_p),
	cryptoEngine(crypto_p),
	tracker(tracker_p),
	settings(settings_p)
{
    ui->setupUi(this);
    /* set up a color palette */
    QPalette palette;
    palette.setColor(QPalette::Window, Qt::darkGray);
    palette.setColor(QPalette::Background, Qt::darkGray);
    palette.setColor(QPalette::WindowText, Qt::black);
    palette.setColor(QPalette::Foreground, Qt::black);
    palette.setColor(QPalette::Base, Qt::white);
    palette.setColor(QPalette::AlternateBase, Qt::lightGray);
    palette.setColor(QPalette::ToolTipBase, Qt::darkGray);
    palette.setColor(QPalette::Text, Qt::black);
    palette.setColor(QPalette::Button, Qt::gray);
    palette.setColor(QPalette::ButtonText, Qt::black);
    palette.setColor(QPalette::BrightText, Qt::white);
    ui->centralWidget->setAutoFillBackground(true);
    ui->centralWidget->setPalette(palette);
    palette.setColor(QPalette::Background, Qt::gray);
    /* set up color for chatFrame */
    ui->chatFrame->setFrameStyle(0x0000);
    ui->chatFrame->setAutoFillBackground(true);
    ui->chatFrame->setPalette(palette);
    /* set up color for buddyFrame */
    ui->buddyListFrame->setFrameStyle(0x0000);
    ui->buddyListFrame->setAutoFillBackground(true);
    ui->buddyListFrame->setPalette(palette);
    ui->tabWidget->removeTab(1);

	/* Periodic timer to check for incoming packets */
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(checkForPackets()));
	timer->start(100);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sendMessage()
{
	string msg = ui->messageToSendLineEdit->text().toStdString();
	if (msg.length() > 0) {
		/* Choose which buddy to send it to (default unencrypted) */
		/* Format: '-e buddyname message contents go here' */
		if ((msg[0] == '-') && (msg[1] == 'e')) {
			int i = 0;
			for (i = 3; i < msg.length(); i++) {
				if (msg[i] == ' ')
					break;
			}
			EncryptedMessage em(string(msg.c_str()+i));
			string buddyname(msg.c_str()+3, msg.c_str()+i);

			/* Encrypt to a buddy */
			try {
			cryptoEngine.encryptMessage(em, buddyname);
			Packet p(em);
			tracker.reportPacket(p.idString());
			tracker.reportHop(p.idString(), "0",
					net.get_ifaddr("meshtrack.pqz.us").to_string());
			ui->textEdit->append("[self] " + ui->messageToSendLineEdit->text());
			net.sendToAllPeers(p);
			} catch (std::runtime_error &e) {
				ui->textEdit->append("[error] Buddy doesn't exist!");
			}
			ui->messageToSendLineEdit->clear();
		} else {
			/* Such duplicate code */
			ClearMessage m(msg);
			Packet p(m);
			tracker.reportPacket(p.idString());
			tracker.reportHop(p.idString(), "0",
					net.get_ifaddr("meshtrack.pqz.us").to_string());
			ui->textEdit->append("[self] " + ui->messageToSendLineEdit->text());
			net.sendToAllPeers(p);
			ui->messageToSendLineEdit->clear();
		}
	}
}

void MainWindow::on_messageToSendLineEdit_returnPressed()
{
	sendMessage();
}

void MainWindow::on_sendPushButton_clicked()
{
	sendMessage();
}

void MainWindow::checkForPackets()
{
    // get messages
    uint16_t numPackets = net.receivePacket();
    if (numPackets) {
        for (int i = 0; i < numPackets; i++) {
            Packet p = net.getPacket();

            std::cout << "Packet received from net" << std::endl;
            engine.ProcessPacket(p);
        }
    }
}

void MainWindow::displayMessage(Packet &p)
{
	/* Display ClearMessage to the default window */
	if (p.type() == 0x01) {
    	ClearMessage m(p);
    	ui->textEdit->append(QString::fromStdString("[CLEARMESSAGE] " + m.bodyString()));
	}

	/* Just print who sent it and that it is trusted */
	if (p.type() == 0x02) {
		EncryptedMessage m(p);
		if (cryptoEngine.tryDecrypt(m)) {
			if(m.trusted()){
				string buddy = cryptoEngine.buddy(m.sender()).name();
				ui->textEdit->append(QString::fromStdString("[" + buddy +
							"] " + string((char *)m.decryptedBody().data())));

			} else {
				ui->textEdit->append(QString::fromStdString("[UNTRUSTED] " +
							string((char *)m.decryptedBody().data())));
			}
		}
	}
}

void MainWindow::saveBuddies(vector<Buddy> buddies)
{
    settings.beginWriteArray("buddies");
    for(int i = 0; i < buddies.size(); i++) {
        settings.setArrayIndex(i);
        settings.setValue("name", buddies[i].name().c_str());
        settings.setValue("key", CryptoEngine::pubkeyToBase64(buddies[i].pubkey()).c_str());
    }
    settings.endArray();
}

void MainWindow::loadBuddies(CryptoEngine& ce)
{
    int size = settings.beginReadArray("buddies");
    cout << "Loading buddies..." << endl;
    for(int i = 0; i < size; i++) {
        settings.setArrayIndex(i);
        string base64 = settings.value("key", "").toString().toStdString();
        string buddy_name = settings.value("name", "").toString().toStdString();
        if (buddy_name.length() > 0) {
            CryptoPP::RSA::PublicKey pubkey = CryptoEngine::pubkeyFromBase64(base64);
            ce.addBuddy(Buddy(pubkey, buddy_name));
        }
    }
    settings.endArray();
}

void MainWindow::populateBuddyList(vector<Buddy> buddies)
{
    QWidget *central = new QWidget(this);
    ui->buddyScrollArea->setWidget(central);
    vlayout = new QVBoxLayout(central);
    central->show();
    for(int i = 0; i < buddies.size(); i++)
    {
        addBuddyToList(buddies[i].name());
    }
    QSpacerItem *spacer = new QSpacerItem(vlayout);
    vlayout->addWidget(spacer);
    buddy_button->show();
}

void MainWindow::addBuddyToList(string buddy_name)
{
    QPushButton *buddy_button = new QPushButton(QString::fromStdString(buddy_name));
    vlayout->addWidget(buddy_button);
    buddy_button->show();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	settings.setValue("crypto/privkey", cryptoEngine.privkeyToBase64(cryptoEngine.getPrivkey()).c_str());
	saveBuddies(cryptoEngine.buddies());
	settings.sync();
	net.shutdown();
}
void MainWindow::on_actionAdd_Peers_triggered()
{
    AddPeer * a = new AddPeer(this, net, engine, cryptoEngine, tracker, settings);
    a->exec();
}

void MainWindow::on_actionSet_Keys_triggered()
{
    PGP * p = new PGP(this, settings);
    p->exec();
}

void MainWindow::on_actionAdd_Buddies_triggered()
{
    AddBuddy * ab = new AddBuddy(this);
    ab->exec();
}
