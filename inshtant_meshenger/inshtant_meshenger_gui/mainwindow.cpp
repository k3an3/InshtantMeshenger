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
#include <algorithm>

#include<vector>
#include <crypto.h>
#include <parser.h>
#include <state.h>
#include <net.h>
#include <tracker.h>
#include <string>
#include <QDesktopServices>
#include <QSound>
#include <fstream>
#include <QFileDialog>
#include <cstdio>
#include <QMessageBox>


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
    //message_sound("sounds/r2d2.wav");

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
    doSendMessage(msg);
}

void MainWindow::doSendMessage(string msg)
{
    if (msg.length() > 0) {
        /* Choose which buddy to send it to (default unencrypted) */
        /* Format: '-e buddyname message contents go here' */
		int index = ui->tabWidget->currentIndex();
        if (index > 0) {
			string buddyname = cryptoEngine.buddies()[index - 1].name();
            EncryptedMessage em(msg);

            /* Encrypt to a buddy */
            try {
            cryptoEngine.encryptMessage(em, buddyname);
            Packet p(em);
            tracker.reportPacket(p.idString());
            tracker.reportHop(p.idString(), "0",
                    net.get_ifaddr("meshtrack.pqz.us").to_string());
            tabEditVector[index - 1]->append("<font color=\"green\">[self]$</font> " + ui->messageToSendLineEdit->text());
            net.sendToAllPeers(p);
            } catch (std::runtime_error &e) {
                ui->textEdit->append("<font color=\"red\"[error]-># Buddy doesn't exist!</font>");
            }
            ui->messageToSendLineEdit->clear();
        } else {
            /* Message is just cleartext */
            /* Such duplicate code */
            ClearMessage m(msg);
            Packet p(m);
            tracker.reportPacket(p.idString());
            tracker.reportHop(p.idString(), "0",
                    net.get_ifaddr("meshtrack.pqz.us").to_string());
			if (index <= 0) index = 1;
            ui->textEdit->append("<font color=\"red\">[self]$</font> " + ui->messageToSendLineEdit->text());
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
        ui->textEdit->append(QString::fromStdString("<font color=\"red\">[CLEARMESSAGE]-></font> " + m.bodyString()));
    }

    /* Just print who sent it and that it is trusted */
    if (p.type() == 0x02) {
        EncryptedMessage m(p);
        if (cryptoEngine.tryDecrypt(m)) {
            if(m.trusted()){
                /* Check to see if it is a file transfer */
                string buddy = cryptoEngine.buddy(m.sender()).name();
                int i;
                for (i = 0; i < cryptoEngine.buddies().size(); i++) {
                    if (!cryptoEngine.buddies()[i].name().compare(buddy))
                        break;
                }
                string token((char*) m.decryptedBody().data(), 5);
                if (token.compare("%%%%f") == 0) {
                    /* Receive file */
                    string filename = "";
                    vector<uint8_t> data = m.decryptedBody();
                    cout << "RX Message Size: " << data.size() << endl;
                    cout << "Receiving file" << endl;
                    for(int end = 5; end < data.size(); end++) {
                        if (data[end] == '%') {
                            data = vector<uint8_t>(data.begin() + end + 1, data.end());
                            break;
                        } else if (data[end] == '/') {
                            /* Disallow paths */
                            filename = "";
                        }
                        filename.push_back(data[end]);
                    }
                    cout << "Posting message to tab " << i << endl;
                    tabEditVector[i]->append(QString::fromStdString("<font color=\"green\">-># Received a file. Saved as " + filename + "</font>"));
                    cout << "Received file name: " << filename << endl;
                    cout << "Size: " << data.size() << endl;
                    ofstream of(filename.c_str(), ios::out | ios::binary);
                    of.write((char *) data.data(), data.size());
                    of.close();

                } else {
                    tabEditVector[i]->append(QString::fromStdString("<font color=\"green\">[" + buddy +
                                "]-></font> " + string((char *)m.decryptedBody().data())));
                }

            } else {
                ui->textEdit->append(QString::fromStdString("<font color=\"red\">[UNTRUSTED]-> " +
                            string((char *)m.decryptedBody().data()) + "</font>"));
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
    spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    vlayout->addItem(spacer);

    for(int i = 0; i < buddies.size(); i++)
    {
        addBuddyToList(buddies[i].name());
    }
}

void MainWindow::addBuddyToList(string buddy_name)
{
    QPushButton *buddy_button = new QPushButton(QString::fromStdString(buddy_name));
    buddyButtonVector.push_back(buddy_button);
    vlayout->removeItem(spacer);
    vlayout->addWidget(buddy_button);
    vlayout->addItem(spacer);
    buddy_button->show();

    QTextEdit *tabEdit = new QTextEdit(this);
    tabEditVector.push_back(tabEdit);
    ui->tabWidget->addTab(tabEdit,QString::fromStdString(buddy_name));
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

void MainWindow::on_actionView_Tracking_triggered()
{
    QDesktopServices::openUrl(QUrl("http://meshtrack.pqz.us/packets", QUrl::TolerantMode));
}

void MainWindow::on_pushButton_clicked()
{
    string path = QFileDialog::getOpenFileName(this).toStdString();
    cout << "Sending file " << path << endl;
    string filename;
    for (int i = 0; i < path.length(); i++) {
        if (path[i] == '/') {
            filename = "";
        } else {
            filename.push_back(path[i]);
        }
    }
    cout << endl;
    cout << "Filename: " << filename << endl;
    ifstream in(path.c_str(), ios::in | ios::binary );
    vector<uint8_t> filedata;
    in.unsetf(std::ios::skipws);

    while(true) {
        int c = in.get();
        if (c == EOF)
            break;
        filedata.push_back((uint8_t) c);
    }
    cout << "File size: " << filedata.size() << endl;

    string msg("%%%%f" + filename + "%" + string((char *) filedata.data(), filedata.size()));
    cout << "Message size:" << msg.length() << endl;
    doSendMessage(msg);
}
