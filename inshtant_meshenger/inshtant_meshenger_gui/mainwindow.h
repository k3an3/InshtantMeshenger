#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <net.h>
#include <parser.h>
#include <string>
#include <state.h>
#include <crypto.h>
#include <vector>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent, libmeshenger::Net &net, libmeshenger::PacketEngine &engine_p, libmeshenger::CryptoEngine &crypto_p, libmeshenger::Tracker &tracker, QSettings &settings_p);
    ~MainWindow();
    void displayMessage(libmeshenger::Packet &p);

    libmeshenger::Net &net;
    libmeshenger::PacketEngine &engine;
    libmeshenger::CryptoEngine &cryptoEngine;
	libmeshenger::Tracker &tracker;
    QSettings &settings;
    void loadBuddies(libmeshenger::CryptoEngine& ce);
    void saveBuddies(std::vector<libmeshenger::Buddy> buddies);
private slots:
    void on_messageToSendLineEdit_returnPressed();
    void on_sendPushButton_clicked();
    void checkForPackets();
    /*
    void MainWindow::on_actionAdd_your_Key_clicked();
    void MainWindow::on_actionNetwork_settings_clicked();*/

    void on_actionAdd_Peers_triggered();

    void sendMessage();

    void closeEvent(QCloseEvent *event);
    void on_actionSet_Keys_triggered();

    void on_actionAdd_Buddies_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
