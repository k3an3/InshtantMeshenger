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
    explicit MainWindow(QWidget *parent, libmeshenger::Net &net, libmeshenger::PacketEngine &engine_p, libmeshenger::CryptoEngine &crypto_p);
    ~MainWindow();
    void displayMessage(libmeshenger::Packet &p);

    libmeshenger::Net &net;
    libmeshenger::PacketEngine &engine;
    libmeshenger::CryptoEngine &cryptoEngine;
private slots:
    void on_messageToSendLineEdit_returnPressed();
    void on_sendPushButton_clicked();
    void checkForPackets();
    /*
    void MainWindow::on_actionAdd_your_Key_clicked();
    void MainWindow::on_actionNetwork_settings_clicked();*/

    void on_actionAdd_Peers_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
