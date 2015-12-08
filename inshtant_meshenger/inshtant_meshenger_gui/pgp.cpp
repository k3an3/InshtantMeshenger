#include <QFrame>
#include <QPalette>
#include <QtCore>
#include <QtGui>
#include "pgp.h"
#include "ui_pgp.h"
#include "mainwindow.h"

#include <parser.h>
#include <state.h>
#include <net.h>
#include <cryptopp/rsa.h>
#include <cryptopp/osrng.h>

using namespace libmeshenger;
using namespace std;
using namespace CryptoPP;

PGP::PGP(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PGP)
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

PGP::~PGP()
{
    delete ui;
}

void PGP::on_pushButton_clicked()
{
    this->destroy();
}

void PGP::on_Export_Key_clicked()
{
    ui->textEdit->clear();
    RSA::PrivateKey privkey = ((MainWindow *) this->parent())->cryptoEngine.getPrivkey();
    string base64 = CryptoEngine::privkeyToBase64(privkey);
    ui->textEdit->append(QString::fromStdString(base64));
}

void PGP::on_Load_Key_clicked()
{
    string base64 = ui->textEdit->toPlainText().toStdString();
    RSA::PrivateKey privkey = CryptoEngine::privkeyFromBase64(base64);
    ((MainWindow *) this->parent())->cryptoEngine.setPrivateKey(privkey);
}

void PGP::on_pushButton_4_clicked()
{
    ui->textEdit->clear();
    RSA::PublicKey pubkey = ((MainWindow *) this->parent())->cryptoEngine.getPubkey();
    string base64 = CryptoEngine::pubkeyToBase64(pubkey);
    ui->textEdit->append(QString::fromStdString(base64));
}

void PGP::on_Generate_key_clicked()
{
    AutoSeededRandomPool rng;
    InvertibleRSAFunction params;
    params.GenerateRandomWithKeySize(rng, 3072);
    RSA::PublicKey pubkey(params);
    RSA::PrivateKey privkey(params);

    ((MainWindow *) parent())->cryptoEngine.setPrivateKey(privkey);

    ui->textEdit->clear();
    string base64 = CryptoEngine::pubkeyToBase64(pubkey);
    ui->textEdit->append(QString::fromStdString(base64));
}
