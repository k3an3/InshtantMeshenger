#include "addbuddy.h"
#include "ui_addbuddy.h"

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
