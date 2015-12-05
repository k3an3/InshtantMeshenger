#-------------------------------------------------
#
# Project created by QtCreator 2015-11-04T03:40:27
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = inshtant_meshenger_gui
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    add_a_peer.cpp \
    PGP.cpp

HEADERS  += mainwindow.h \
    add_a_peer.h

FORMS    += mainwindow.ui \
    add_a_peer.ui \
    PGP.ui
