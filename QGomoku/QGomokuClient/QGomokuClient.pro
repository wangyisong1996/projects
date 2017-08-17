#-------------------------------------------------
#
# Project created by QtCreator 2016-09-01T18:39:21
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QGomokuClient
TEMPLATE = app


SOURCES += main.cpp\
        clientmainwindow.cpp \
    logindialog.cpp

HEADERS  += clientmainwindow.h \
    logindialog.h

FORMS    += clientmainwindow.ui \
    logindialog.ui

RESOURCES += \
    resources.qrc
