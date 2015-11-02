#include "mainwindow.h"
#include <QApplication>
#include <net.h>

using namespace libmeshenger;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
