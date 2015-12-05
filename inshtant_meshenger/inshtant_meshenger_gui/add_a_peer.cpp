
#include <QFrame>
#include <QPalette>
#include <QtCore>
#include <QtGui>
#include "add_a_peer.h"
#include "ui_add_a_peer.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <parser.h>
#include <state.h>
#include <net.h>

using namespace libmeshenger;
using namespace std;

ADD_A_PEER::~ADD_A_PEER()
{
    delete ui;
}

void ADD_A_PEER::on_button_discover_returnPressed()//the button the enable and disable discovery
{

}
void ADD_A_PEER::on_Add_a_Peer_same_net_returnPressed()//Buttons to add to same network
{
    ((MainWindow *)this->parent)->net.addPeer(ui->IP_To_Add_Same->text().toStdString());
     ui->IP_To_Add_Same->clear();
}
void ADD_A_PEER::on_Add_a_Peer_different_net_returnPressed()//  Buttons to add to a differnt network
//should be exactly the same as the code for same network
{

    ((MainWindow *)this->parent)->net.addPeer(ui->IP_To_Add_Differnt->text().toStdString());
     ui->IP_To_Add_Differnt->clear();
}
void ADD_A_PEER::on_Disconnect_button_toggled()
{

}
