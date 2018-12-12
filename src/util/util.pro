#-------------------------------------------------
#
# Project created by QtCreator 2018-06-07T10:58:59
#
#-------------------------------------------------

QT       += widgets

TARGET = derflautil
TEMPLATE = lib
CONFIG += c++17 precompile_header
PRECOMPILED_HEADER = stdafx.h

macx: {
    DESTDIR = $$PWD/../../bin/Derfla.app/Contents/Libs
} else {
    DESTDIR = $$PWD/../../bin/
}

win32-clang-msvc: CONFIG -= precompile_header
DEFINES += UTIL_LIBRARY

include($$PWD/../../3rdparty/Boost.pri)
include($$PWD/../../3rdparty/qtsingleapplication/qtsingleapplication.pri)
# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    directory.cpp \
    util.cpp

HEADERS += \
    util_global.h \
    stdafx.h \
    directory.h \
    util.h

# workaround for non-MSVC mkspec on 5.12
equals(QT_MINOR_VERSION, 12): !win32-*msvc: LIBS += /usr/local/lib
win32: {
    win32-*g++* {
        DEFINES += WINVER=0x0600 _WIN32_WINNT=0x0600
    }
    win32-*msvc* {
        QMAKE_CXXFLAGS_RELEASE += /Zi
        QMAKE_LFLAGS_RELEASE += /DEBUG
    }
    QT += winextras
    SOURCES += \
        $$PWD/win_util.cpp \
        $$PWD/WinIconProvider.cpp

    HEADERS += \
        $$PWD/win_util.h \
        $$PWD/WinIconProvider.h
    RC_FILE = util.rc
    LIBS += -lVersion -lComctl32 -luser32 -lOle32 -lGdi32 -lShell32 -luuid -ladvapi32 -lwinmm
}

unix: !macx: {
    SOURCES += \
        $$PWD/unix_util.cpp

    HEADERS += \
        $$PWD/unix_util.h
}

macx: {
    OBJECTIVE_SOURCES += mac_util.mm
    LIBS += -framework AppKit
}