QT += core gui xml concurrent sql gui-private

CONFIG += c++14 console precompile_header
CONFIG -= app_bundle

PRECOMPILED_HEADER = stdafx.h
DESTDIR = $$PWD/../../bin/lfs

include($$PWD/../../3rdparty/qtsingleapplication/qtsingleapplication.pri)
include($$PWD/../../3rdparty/Boost.pri)
include($$PWD/../../3rdparty/sqlite3/sqlite3.pri)
include($$PWD/../../3rdparty/rapidjson/rapidjson.pri)
include($$PWD/util/util.pri)
# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH += $$PWD/util

SOURCES += \
    main.cpp \
    dbrw.cpp \
    localfsscanner.cpp \
    localfsitem.cpp

SUBDIRS += \
    lfs.pro

HEADERS += \
    stdafx.h \
    dbrw.h \
    localfsscanner.h \
    localfsitem.h

win32: {
    win32-msvc* {
        QMAKE_LFLAGS += "/NODEFAULTLIB:MSVCRT"
    } else {
        DEFINES += WINVER=0x0600 _WIN32_WINNT=0x0600
        LIBS += -lz
    }

    LIBS += -lVersion -lComctl32 -lOle32 -lGdi32

    copy_cfg.commands = '$(COPY_FILE) $$shell_path($$PWD/extension.cfg) $$shell_path($$DESTDIR)'
    QMAKE_EXTRA_TARGETS += copy_cfg
    POST_TARGETDEPS += copy_cfg
}

DISTFILES += \
    extension.cfg
