#include "addbuddy.h"
#include "ui_addbuddy.h"
#include <crypto.h>
#include <string>
#include "mainwindow.h"
using namespace std;
using namespace libmeshenger;

AddBuddy::AddBuddy(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddBuddy)
{
    ui->setupUi(this);
}

AddBuddy::~AddBuddy()
{
    delete ui;
}

void AddBuddy::on_pushButton_clicked()
{
    try {
            string base64 = ui->plainTextEdit->toPlainText().toStdString();
            string name = ui->lineEdit->text().toStdString();
            if (name.length() > 0) {
            Buddy buddy(base64, name);
            ((MainWindow *) parent())->cryptoEngine.addBuddy(buddy);
            ((MainWindow *) parent())->addBuddyToList(buddy.name());

            destroy();
        }
    } catch (CryptoPP::BERDecodeErr e) {
    }
}

void AddBuddy::on_pushButton_2_clicked()
{
    destroy();
}

void AddBuddy::on_plainTextEdit_textChanged()
{
    try {
        string fp = CryptoEngine::fingerprint(CryptoEngine::pubkeyFromBase64(ui->plainTextEdit->toPlainText().toStdString()));
        ui->fpLine->setText(QString::fromStdString(fp));
    } catch (CryptoPP::BERDecodeErr e) {
    }
}
