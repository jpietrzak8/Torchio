#-------------------------------------------------
#
# Project created by QtCreator 2014-04-09T17:36:39
#
#-------------------------------------------------

QT       += core dbus

QT       -= gui

TARGET = torchio
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    torcontroller.cpp \
    tordbus.cpp \
    torflashled.cpp \
    tormorse.cpp

maemo5 {
    target.path = /opt/torchio/bin
    INSTALLS += target
}

OTHER_FILES += \
    qtc_packaging/debian_fremantle/rules \
    qtc_packaging/debian_fremantle/README \
    qtc_packaging/debian_fremantle/copyright \
    qtc_packaging/debian_fremantle/control \
    qtc_packaging/debian_fremantle/compat \
    qtc_packaging/debian_fremantle/changelog \
    COPYING \
    LICENSE.md \
    README.md \
    torchio64.png

HEADERS += \
    torcontroller.h \
    tordbus.h \
    torexception.h \
    torflashled.h \
    tormorse.h
