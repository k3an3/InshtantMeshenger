#include <QFrame>
#include <QPalette>
#include <QtCore>
#include <QtGui>
#include <QTimer>
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <parser.h>
#include <state.h>
#include <net.h>

using namespace libmeshenger;
using namespace std;

MainWindow::MainWindow(QWidget *parent, libmeshenger::Net &net_p, libmeshenger::PacketEngine &engine_p, libmeshenger::CryptoEngine &crypto_p) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
	net(net_p),
	engine(engine_p),
	cryptoEngine(crypto_p)
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

void MainWindow::on_messageToSendLineEdit_returnPressed()
{
	/* Choose which buddy to send it to (default unencrypted) */
	/* Format: '-e buddyname message contents go here' */
	string msg = ui->messageToSendLineEdit->text().toStdString();
	if ((msg[0] == '-') && (msg[1] == 'e')) {
		int i = 0;
		for (i = 3; i < msg.length(); i++) {
			if (msg[i] == ' ')
				break;
		}
		EncryptedMessage em(string(msg.c_str()+i));
		string buddyname(msg.c_str()+3, msg.c_str()+i);

		/* Encrypt to a buddy */
		cryptoEngine.encryptMessage(em, buddyname);
		Packet p(em);
		ui->textEdit->append(ui->messageToSendLineEdit->text());
		net.sendToAllPeers(p);
		ui->messageToSendLineEdit->clear();
	} else {
		ClearMessage m(msg);
		Packet p(m);
		ui->textEdit->append(ui->messageToSendLineEdit->text());
		net.sendToAllPeers(p);
		ui->messageToSendLineEdit->clear();
	}
}

void MainWindow::on_sendPushButton_clicked()
{
    ClearMessage m(ui->messageToSendLineEdit->text().toStdString());
    Packet p(m);
    ui->textEdit->append(ui->messageToSendLineEdit->text());
    net.sendToAllPeers(p);
    ui->messageToSendLineEdit->clear();
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
