#-------------------------------------------------
#
# Project created by QtCreator 2015-10-31T15:21:15
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = 309_test
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    chat.cpp \
    dialog.cpp

HEADERS  += mainwindow.h \
    chat.h \
    dialog.h

FORMS    += mainwindow.ui \
    chat.ui \
    dialog.ui

DISTFILES += \
    ChatForm.ui.qml \
    Chat.qml
