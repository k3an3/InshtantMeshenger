#ifndef ADDPEER_H
#define ADDPEER_H

#include <QDialog>

namespace Ui {
class AddPeer;
}

class AddPeer : public QDialog
{
    Q_OBJECT

public:
    explicit AddPeer(QWidget *parent = 0);
    ~AddPeer();
private slots:
private:
    Ui::AddPeer *ui;
};

#endif // ADDPEER_H
