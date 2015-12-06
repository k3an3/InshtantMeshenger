#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <net.h>
#include <parser.h>
#include <string>
#include <state.h>
#include <crypto.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent, libmeshenger::Net &net, libmeshenger::PacketEngine &engine_p, libmeshenger::CryptoEngine &crypto_p, libmeshenger::Tracker &tracker);
    ~MainWindow();
    void displayMessage(libmeshenger::Packet &p);

    libmeshenger::Net &net;
    libmeshenger::PacketEngine &engine;
    libmeshenger::CryptoEngine &cryptoEngine;
	libmeshenger::Tracker &tracker;
private slots:

    void on_messageToSendLineEdit_returnPressed();

    void on_sendPushButton_clicked();

    void checkForPackets();


private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
