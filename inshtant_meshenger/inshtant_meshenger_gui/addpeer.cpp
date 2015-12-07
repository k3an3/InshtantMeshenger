#include <QFrame>
#include <QPalette>
#include <QtCore>
#include <QtGui>
#include "addpeer.h"
#include "ui_addpeer.h"

#include <parser.h>
#include <state.h>
#include <net.h>

using namespace libmeshenger;
using namespace std;

AddPeer::AddPeer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddPeer)
{
    ui->setupUi(this);
}

AddPeer::~AddPeer()
{
    delete ui;
}
