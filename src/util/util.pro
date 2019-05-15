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
    DESTDIR = $$PWD/../../bin/x86_64/Derfla.app/Contents/Libs
    RESDIR = $$PWD/../../bin/x86_64/Derfla.app/Contents/Resources/
} else {
    contains(QMAKE_HOST.arch, x86_64): {
        DESTDIR = $$PWD/../../bin/x86_64
        RESDIR = $$PWD/../../bin/x86_64
    } else: {
        DESTDIR = $$PWD/../../bin/x86
        RESDIR = $$PWD/../../bin/x86
    }
}

win32-*msvc: QMAKE_CXXFLAGS += "/std:c++17"
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
    $$PWD/directory.cpp \
    $$PWD/util.cpp \
    $$PWD/derflaaction.cpp \
    $$PWD/scopedguard.cpp

HEADERS += \
    $$PWD/util_global.h \
    $$PWD/stdafx.h \
    $$PWD/directory.h \
    $$PWD/util.h \
    $$PWD/derflaaction.h \
    $$PWD/scopedguard.h 

INCLUDEPATH += $$PWD

CODECFORTR      = UTF-8
CODECFORSRC     = UTF-8
TRANSLATIONS    = $$PWD/translations/util_en_US.ts \
                  $$PWD/translations/util_zh_CN.ts

isEmpty(QMAKE_LUPDATE) {
    win32:QMAKE_LUPDATE = $$[QT_INSTALL_BINS]\lupdate.exe
    else:QMAKE_LUPDATE = $$[QT_INSTALL_BINS]/lupdate
}

isEmpty(QMAKE_LRELEASE) {
    win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\lrelease.exe
    else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
}

lupdate.commands = $$QMAKE_LUPDATE -no-obsolete $$PWD/util.pro
lupdates.depends = $$SOURCES $$HEADERS $$FORMS $$TRANSLATIONS
lrelease.commands = $$QMAKE_LRELEASE $$PWD/util.pro
lrelease.depends = lupdate
translate.depends = lrelease
translate.commands = '$(COPY_DIR) $$shell_path($$PWD/translations) $$shell_path($$RESDIR)'
QMAKE_EXTRA_TARGETS += lupdate lrelease translate
POST_TARGETDEPS += translate

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
    OBJECTIVE_SOURCES += $$PWD/mac_util.mm
    LIBS += -framework AppKit
}
