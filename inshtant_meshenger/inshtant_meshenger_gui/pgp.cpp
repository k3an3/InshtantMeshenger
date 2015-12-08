#include <QFrame>
#include <QPalette>
#include <QtCore>
#include <QSettings>
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

PGP::PGP(QWidget *parent, QSettings& settings_p) :
    QDialog(parent),
    settings(settings_p),
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

    on_textEdit_textChanged();
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
    ui->label->setText("Your private key:");
    ui->textEdit->clear();
    RSA::PrivateKey privkey = ((MainWindow *) this->parent())->cryptoEngine.getPrivkey();
    string base64 = CryptoEngine::privkeyToBase64(privkey);
    ui->textEdit->append(QString::fromStdString(base64));
}

void PGP::on_Load_Key_clicked()
{
    ui->label->setText("Key was imported.");
    string base64 = ui->textEdit->toPlainText().toStdString();
    RSA::PrivateKey privkey = CryptoEngine::privkeyFromBase64(base64);
    ((MainWindow *) this->parent())->cryptoEngine.setPrivateKey(privkey);
    settings.setValue("crypto/privkey", base64.c_str());
}

void PGP::on_pushButton_4_clicked()
{
    ui->label->setText("Your public key:");
    ui->textEdit->clear();
    RSA::PublicKey pubkey = ((MainWindow *) this->parent())->cryptoEngine.getPubkey();
    string base64 = CryptoEngine::pubkeyToBase64(pubkey);
    ui->textEdit->append(QString::fromStdString(base64));
}

void PGP::on_Generate_key_clicked()
{
    ui->label->setText("New key generated below:");
    AutoSeededRandomPool rng;
    InvertibleRSAFunction params;
    params.GenerateRandomWithKeySize(rng, 3072);
    RSA::PublicKey pubkey(params);
    RSA::PrivateKey privkey(params);

    ((MainWindow *) parent())->cryptoEngine.setPrivateKey(privkey);
    settings.setValue("crypto/privkey", CryptoEngine::privkeyToBase64(privkey).c_str());

    ui->textEdit->clear();
    string base64 = CryptoEngine::pubkeyToBase64(pubkey);
    ui->textEdit->append(QString::fromStdString(base64));
}

void PGP::on_textEdit_textChanged()
{
    string fp = CryptoEngine::fingerprint(((MainWindow *) parent())->cryptoEngine.getPubkey());
    ui->fpLine->setText(QString::fromStdString(fp));
}
