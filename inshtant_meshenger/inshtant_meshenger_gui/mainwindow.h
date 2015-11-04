#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <net.h>
#include <parser.h>
#include <string>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0, libmeshenger::Net *net = 0);
    ~MainWindow();
    void displayMessage(libmeshenger::Packet &p);

private slots:

    void on_messageToSendLineEdit_returnPressed();

    void on_sendPushButton_clicked();

    void on_getMessagesPushButton_clicked();

private:
    Ui::MainWindow *ui;
    libmeshenger::Net *net;
};

#endif // MAINWINDOW_H
