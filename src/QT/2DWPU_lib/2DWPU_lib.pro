#-------------------------------------------------
#
# Project created by QtCreator 2011-12-22T12:45:22
#
#-------------------------------------------------

QT       -= gui

TARGET = 2DWPU_lib
TEMPLATE = lib
CONFIG += staticlib

SOURCES += dummy.cpp \
    ../../2DWPU/2DWPU_lib/Analys2DWPUcore.cpp \
    ../../2DWPU/2DWPU_lib/2DWPU.cpp \
    ../../2DWPU/2DWPU_lib/Analys2DWPU.cpp \
    ../../2DWPU/2DWPU_lib/IOinterface.cpp \
    ../../2DWPU/2DWPU_lib/Simple2DWPU.cpp \
    ../../2DWPU/2DWPU_lib/Simple2DWPUcore.cpp

HEADERS += dummy.h \
    ../../2DWPU/2DWPU_lib/Analys2DWPUcore.h \
    ../../2DWPU/2DWPU_lib/Analys2DWPU.h \
    ../../2DWPU/2DWPU_lib/WPU2Dcore.h \
    ../../2DWPU/2DWPU_lib/Simple2DWPUparallelismManager.h \
    ../../2DWPU/2DWPU_lib/IOinterface.h \
    ../../2DWPU/2DWPU_lib/Instructions.h \
    ../../2DWPU/2DWPU_lib/2DWPU.h \
    ../../2DWPU/2DWPU_lib/Registers.h \
    ../../2DWPU/2DWPU_lib/2DWPUconfig.h \
    ../../2DWPU/2DWPU_lib/CoreInfoHolders.h \
    ../../2DWPU/2DWPU_lib/Simple2DWPU.h \
    ../../2DWPU/2DWPU_lib/Simple2DWPUcore.h
unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
