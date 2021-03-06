QT += core gui xml concurrent sql gui-private

CONFIG += c++17 console precompile_header
CONFIG -= app_bundle

TARGET = et
PRECOMPILED_HEADER = stdafx.h
macx: {
    DESTDIR = $$PWD/../../bin/x86_64/Derfla.app/Contents/Extensions/everything
} else {
    contains(QMAKE_HOST.arch, x86_64): {
        DESTDIR = $$PWD/../../bin/x86_64/extensions/everything
    } else: {
        DESTDIR = $$PWD/../../bin/x86/extensions/everything
    }
}

win32-*msvc: QMAKE_CXXFLAGS += "/std:c++17"
win32-clang-msvc: CONFIG -= precompile_header
include($$PWD/../../3rdparty/qtsingleapplication/qtsingleapplication.pri)
include($$PWD/../../3rdparty/Boost.pri)

include($$PWD/../util/util.pri)
# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp

DISTFILES += \
    extension.derflaext

HEADERS += \
    stdafx.h

INCLUDEPATH += $$PWD

CODECFORTR      = UTF-8
CODECFORSRC     = UTF-8
TRANSLATIONS    = $$PWD/translations/everything_en_US.ts \
                  $$PWD/translations/everything_zh_CN.ts

isEmpty(QMAKE_LUPDATE) {
    win32:QMAKE_LUPDATE = $$[QT_INSTALL_BINS]\lupdate.exe
    else:QMAKE_LUPDATE = $$[QT_INSTALL_BINS]/lupdate
}

isEmpty(QMAKE_LRELEASE) {
    win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\lrelease.exe
    else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
}

lupdate.commands = $$QMAKE_LUPDATE -no-obsolete $$PWD/everything.pro
lupdates.depends = $$SOURCES $$HEADERS $$FORMS $$TRANSLATIONS
lrelease.commands = $$QMAKE_LRELEASE $$PWD/everything.pro
lrelease.depends = lupdate
translate.depends = lrelease
translate.commands = '$(COPY_DIR) $$shell_path($$PWD/translations) $$shell_path($$DESTDIR/)'
QMAKE_EXTRA_TARGETS += lupdate lrelease translate
POST_TARGETDEPS += translate

win32: {
    include($$PWD/../../3rdparty/everything/everything.pri)
    win32-*g++* {
        DEFINES += WINVER=0x0600 _WIN32_WINNT=0x0600
    }
    win32-*msvc* {
        QMAKE_CXXFLAGS_RELEASE += /Zi
        QMAKE_LFLAGS_RELEASE += /DEBUG
    }
    RC_FILE = everything.rc
    LIBS += -lVersion -lComctl32 -luser32 -lOle32 -lGdi32 -lShell32 -luuid -ladvapi32 -lwinmm

    contains(QMAKE_HOST.arch, x86_64): {
        copy_everything_dll.commands = '$(COPY_FILE) $$shell_path($$PWD/../../3rdparty/everything/sdk/dll/Everything64.dll) $$shell_path($$DESTDIR/Everything64.dll)'
        copy_everything_exe.commands = '$(COPY_FILE) $$shell_path($$PWD/../../3rdparty/everything/sdk/exe/Everything64.exe) $$shell_path($$DESTDIR/Everything64.exe)'
    }
    else: {
        copy_everything_dll.commands = '$(COPY_FILE) $$shell_path($$PWD/../../3rdparty/everything/sdk/dll/Everything32.dll) $$shell_path($$DESTDIR/Everything32.dll)'
        copy_everything_exe.commands = '$(COPY_FILE) $$shell_path($$PWD/../../3rdparty/everything/sdk/exe/Everything32.exe) $$shell_path($$DESTDIR/Everything32.exe)'
    }
    QMAKE_EXTRA_TARGETS += copy_everything_exe copy_everything_dll
    POST_TARGETDEPS += copy_everything_exe copy_everything_dll
}

macx: {
    OBJECTIVE_HEADERS += \
        mdfindwrapper.h
    OBJECTIVE_SOURCES += \
        mdfindwrapper.mm
    LIBS+=-lobjc -framework CoreServices
}

copy_png.commands = '$(COPY_FILE) $$shell_path($$PWD/folder.png) $$shell_path($$DESTDIR)'
copy_cfg.commands = '$(COPY_FILE) $$shell_path($$PWD/extension.derflaext) $$shell_path($$DESTDIR)'

QMAKE_EXTRA_TARGETS += copy_png copy_cfg
POST_TARGETDEPS += copy_png copy_cfg

RESOURCES += \
    $$PWD/everything.qrc

DISTFILES += \
    $$PWD/extension.derflaext
