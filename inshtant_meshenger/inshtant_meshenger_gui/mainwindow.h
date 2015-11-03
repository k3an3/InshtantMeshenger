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

private:
    Ui::MainWindow *ui;
    libmeshenger::Net *net;

private slots:

    void on_pushButton_clicked();
};

#endif // MAINWINDOW_H
