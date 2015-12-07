#include <QFrame>
#include <QPalette>
#include <QtCore>
#include <QtGui>
#include "pgp.h"
#include "ui_pgp.h"

#include <parser.h>
#include <state.h>
#include <net.h>

using namespace libmeshenger;
using namespace std;

PGP::PGP(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PGP)
{
    ui->setupUi(this);
}

PGP::~PGP()
{
    delete ui;
}
