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

void AddBuddy::on_pushButton_2_clicked()
{
    string base64 = ui->plainTextEdit->toPlainText().toStdString();
    string name = ui->lineEdit->text().toStdString();
    Buddy buddy(base64, name);
    ((MainWindow *) parent())->cryptoEngine.addBuddy(buddy);
}

void AddBuddy::on_pushButton_clicked()
{
    destroy();
}
