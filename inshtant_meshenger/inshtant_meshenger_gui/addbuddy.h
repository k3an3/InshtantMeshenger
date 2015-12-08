#ifndef ADDBUDDY_H
#define ADDBUDDY_H

#include <QDialog>

namespace Ui {
class AddBuddy;
}

class AddBuddy : public QDialog
{
    Q_OBJECT

public:
    explicit AddBuddy(QWidget *parent = 0);
    ~AddBuddy();

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::AddBuddy *ui;
};

#endif // ADDBUDDY_H
