#-------------------------------------------------
#
# Project created by QtCreator 2016-08-22T21:45:48
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QCalendar
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    mycalendar.cpp \
    language.cpp \
    mylabel.cpp \
    myeventframe.cpp \
    utils.cpp \
    myevent.cpp \
    eventmanager.cpp \
    addeventdialog.cpp \
    settingsdialog.cpp \
    vieweventdialog.cpp

HEADERS  += widget.h \
    mycalendar.h \
    language.h \
    mylabel.h \
    myeventframe.h \
    utils.h \
    myevent.h \
    eventmanager.h \
    addeventdialog.h \
    settingsdialog.h \
    vieweventdialog.h

FORMS    += widget.ui \
    addeventdialog.ui \
    settingsdialog.ui \
    vieweventdialog.ui
