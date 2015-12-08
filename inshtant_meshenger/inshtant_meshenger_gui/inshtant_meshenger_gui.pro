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
    pgp.cpp \
    addpeer.cpp \
    addbuddy.cpp

HEADERS  += mainwindow.h \
    pgp.h \
    addpeer.h \
    addbuddy.h

FORMS    += mainwindow.ui \
    pgp.ui \
    addpeer.ui \
    addbuddy.ui
