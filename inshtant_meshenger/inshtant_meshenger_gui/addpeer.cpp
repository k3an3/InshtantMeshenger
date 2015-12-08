#include <QFrame>
#include <QPalette>
#include <QtCore>
#include <QtGui>
#include "addpeer.h"
#include "ui_addpeer.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

#include <parser.h>
#include <state.h>
#include <net.h>

using namespace libmeshenger;
using namespace std;

AddPeer::AddPeer(QWidget *parent, libmeshenger::Net &net_p, libmeshenger::PacketEngine &engine_p, libmeshenger::CryptoEngine &crypto_p, Tracker &tracker_p, QSettings &settings_p):
    QDialog(parent),
    ui(new Ui::AddPeer),
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
    palette.setColor(QPalette::Background, Qt::gray);
}

AddPeer::~AddPeer()
{
    delete ui;
}

void AddPeer::on_close_button_clicked()
{
    this->destroy();
}
void AddPeer::on_add_peer_button_clicked()
{
    net.addPeer(ui->host_to_add->text().toStdString());
}
