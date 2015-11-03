#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtCore>
#include <QtGui>


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
    net = net_p;
}

MainWindow::~MainWindow()
{
    delete ui;
}

 void MainWindow::on_pushButton_clicked()
{
    ClearMessage m(ui->lineEdit->text().toStdString());
    Packet p(m);
    ui->textBrowser->append(ui->lineEdit->text());
    net->sendToAllPeers(p);
    ui->lineEdit->clear();
}

 void MainWindow::displayMessage(Packet &p)
 {
     ClearMessage m(p);
     ui->textBrowser->append(QString::fromStdString(m.bodyString()));
 }
