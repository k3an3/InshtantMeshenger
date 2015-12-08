#include <QFrame>
#include <QPalette>
#include <QtCore>
#include <QtGui>
#include <QTimer>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_add_a_peer.h"
#include "ui_PGP.h"

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
    ClearMessage m(ui->messageToSendLineEdit->text().toStdString());
    Packet p(m);
    ui->textEdit->append(ui->messageToSendLineEdit->text());
    net.sendToAllPeers(p);
    ui->messageToSendLineEdit->clear();
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
void MainWindow::on_actionAdd_your_Key_clicked(){
    PGP s(0, net, engine, cryptoEngine);
    win = &s;
    s.show();
}
void MainWindow::on_actionNetwork_settings_clicked(){
    add_a_peer m(0, net, engine, cryptoEngine);
    win = &m;
    m.show();
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
