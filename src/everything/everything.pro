QT += core gui xml concurrent sql gui-private

CONFIG += c++14 console precompile_header
CONFIG -= app_bundle

TARGET = et
PRECOMPILED_HEADER = stdafx.h
DESTDIR = $$PWD/../../bin/everything

win32-clang-msvc: CONFIG -= precompile_header
include($$PWD/../../3rdparty/qtsingleapplication/qtsingleapplication.pri)
include($$PWD/../../3rdparty/Boost.pri)
include($$PWD/../../3rdparty/everything/everything.pri)
include($$PWD/../util/util.pri)
# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp

DISTFILES += \
    extension.cfg

HEADERS += \
    stdafx.h

win32: {
    win32-g++* {
        DEFINES += WINVER=0x0600 _WIN32_WINNT=0x0600
    }
    LIBS += -lVersion -lComctl32 -luser32 -lOle32 -lGdi32 -lShell32 -luuid -ladvapi32 -lwinmm

    contains(QMAKE_HOST.arch, x86_64): {
        copy_everything_dll.commands = '$(COPY_FILE) $$shell_path($$PWD/../../3rdparty/everything/sdk/dll/Everything64.dll) $$shell_path($$DESTDIR/Everything64.dll)'
        copy_everything_exe.commands = '$(COPY_FILE) $$shell_path($$PWD/../../3rdparty/everything/sdk/exe/Everything64.exe) $$shell_path($$DESTDIR/Everything64.exe)'
    }
    else: {
        copy_everything_dll.commands = '$(COPY_FILE) $$shell_path($$PWD/../../3rdparty/everything/sdk/dll/Everything32.dll) $$shell_path($$DESTDIR/Everything32.dll)'
        copy_everything_exe.commands = '$(COPY_FILE) $$shell_path($$PWD/../../3rdparty/everything/sdk/exe/Everything32.exe) $$shell_path($$DESTDIR/Everything32.exe)'
    }
    copy_png.commands = '$(COPY_FILE) $$shell_path($$PWD/folder.png) $$shell_path($$DESTDIR)'
    copy_cfg.commands = '$(COPY_FILE) $$shell_path($$PWD/extension.cfg) $$shell_path($$DESTDIR)'
    QMAKE_EXTRA_TARGETS += copy_png copy_cfg copy_everything_exe copy_everything_dll
    POST_TARGETDEPS += copy_png copy_cfg copy_everything_exe copy_everything_dll
}

RESOURCES += \
    everything.qrc
