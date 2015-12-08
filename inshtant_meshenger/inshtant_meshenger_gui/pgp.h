#ifndef PGP_H
#define PGP_H

#include <QDialog>
#include <net.h>
#include <parser.h>
#include <string>
#include <state.h>
#include <crypto.h>

namespace Ui {
class PGP;
}

class PGP : public QDialog
{
    Q_OBJECT

public:
    explicit PGP(QWidget *parent = 0);
    ~PGP();
private slots:
    void on_pushButton_clicked();

    void on_Export_Key_clicked();

    void on_Load_Key_clicked();

    void on_pushButton_4_clicked();

    void on_Generate_key_clicked();

private:
    Ui::PGP *ui;
};

#endif // PGP_H
