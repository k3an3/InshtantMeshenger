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
