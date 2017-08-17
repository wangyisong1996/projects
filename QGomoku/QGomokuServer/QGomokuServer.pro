#-------------------------------------------------
#
# Project created by QtCreator 2016-08-31T15:05:20
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QGomokuServer
TEMPLATE = app


SOURCES += main.cpp \
        servermainwindow.cpp \
        gomokuserver.cpp

HEADERS  += servermainwindow.h \
		gomokuserver.h

FORMS    += servermainwindow.ui
