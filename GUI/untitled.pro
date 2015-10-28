#-------------------------------------------------
#
# Project created by QtCreator 2015-10-24T14:32:55
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = untitled
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    chat.cpp

HEADERS  += mainwindow.h \
    chat.h

FORMS    += mainwindow.ui \
    chat.ui

DISTFILES += \
    ChatForm.ui.qml \
    Chat.qml
