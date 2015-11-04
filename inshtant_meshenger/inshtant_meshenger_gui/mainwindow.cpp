#include <QFrame>
#include <QPalette>
#include <QtCore>
#include <QtGui>
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <parser.h>
#include <state.h>
#include <net.h>

using namespace libmeshenger;
using namespace std;

MainWindow::MainWindow(QWidget *parent, libmeshenger::Net *net_p) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
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

    net = net_p;
}

MainWindow::~MainWindow()
{
    delete ui;
    delete net;
}

void MainWindow::on_messageToSendLineEdit_returnPressed()
{
    ClearMessage m(ui->lineEdit->text().toStdString());
    Packet p(m);
    ui->textBrowser->append(ui->lineEdit->text());
    net->sendToAllPeers(p);
    ui->lineEdit->clear();
}

void MainWindow::on_sendPushButton_clicked()
{
    ClearMessage m(ui->lineEdit->text().toStdString());
    Packet p(m);
    ui->textBrowser->append(ui->lineEdit->text());
    net->sendToAllPeers(p);
    ui->lineEdit->clear();
}

void MainWindow::on_getMessagesPushButton_clicked()
{
    // get messages
}

void MainWindow::displayMessage(Packet &p)
{
    ClearMessage m(p);
    ui->textBrowser->append(QString::fromStdString(m.bodyString()));
}
