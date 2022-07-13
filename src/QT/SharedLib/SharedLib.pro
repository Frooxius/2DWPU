#-------------------------------------------------
#
# Project created by QtCreator 2011-12-22T12:48:29
#
#-------------------------------------------------

QT       -= gui

TARGET = SharedLib
TEMPLATE = lib
CONFIG += staticlib

SOURCES += dummy.cpp \
    ../../2DWPU/SharedLib/dummy.cpp \
    ../../2DWPU/SharedLib/Exception.cpp

HEADERS += dummy.h \
    ../../2DWPU/SharedLib/Global.h \
    ../../2DWPU/SharedLib/Exception.h \
    ../../2DWPU/SharedLib/Memory.h
unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
