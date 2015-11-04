#include <QFrame>
#include <QPalette>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /* set up a color palette */
    QPalette palette;
    palette.setColor(QPalette::Window, Qt::darkGray);
    palette.setColor(QPalette::Background, Qt::darkGray);
    palette.setColor(QPalette::WindowText, Qt::black);
    palette.setColor(QPalette::Foreground, Qt::black);
    palette.setColor(QPalette::Base, Qt::white);
    palette.setColor(QPalette::AlternateBase, Qt::lightGray);
    palette.setColor(QPalette::ToolTipBase, Qt::darkGray);
    palette.setColor(QPalette::Text, Qt::black);
    palette.setColor(QPalette::Button, Qt::gray);
    palette.setColor(QPalette::ButtonText, Qt::black);
    palette.setColor(QPalette::BrightText, Qt::white);

    ui->centralWidget->setAutoFillBackground(true);
    ui->centralWidget->setPalette(palette);

    palette.setColor(QPalette::Background, Qt::gray);
    /* set up color for chatFrame */
    ui->chatFrame->setFrameStyle(0x0000);
    ui->chatFrame->setAutoFillBackground(true);
    ui->chatFrame->setPalette(palette);
    /* set up color for buddyFrame */
    ui->buddyListFrame->setFrameStyle(0x0000);
    ui->buddyListFrame->setAutoFillBackground(true);
    ui->buddyListFrame->setPalette(palette);

    ui->tabWidget->removeTab(1);
}

MainWindow::~MainWindow()
{
    delete ui;
}
