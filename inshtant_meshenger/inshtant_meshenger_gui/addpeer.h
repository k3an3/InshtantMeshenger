#ifndef ADDPEER_H
#define ADDPEER_H

#include <QDialog>
#include <QSettings>
#include <net.h>
#include <parser.h>
#include <string>
#include <state.h>
#include <crypto.h>
#include <vector>

namespace Ui {
class AddPeer;
}

class AddPeer : public QDialog
{
    Q_OBJECT

public:
    explicit AddPeer(QWidget *parent, libmeshenger::Net &net_p, libmeshenger::PacketEngine &engine_p, libmeshenger::CryptoEngine &crypto_p, libmeshenger::Tracker &tracker, QSettings &settings_p);
    ~AddPeer();
    libmeshenger::Net &net;
    libmeshenger::PacketEngine &engine;
    libmeshenger::CryptoEngine &cryptoEngine;
	libmeshenger::Tracker &tracker;
    QSettings &settings;
private slots:
    void on_close_button_clicked();
    void on_Disconnect_button_clicked();
    void on_add_peer_button_clicked();

private:
    Ui::AddPeer *ui;
};

#endif // ADDPEER_H
