QT += core gui xml concurrent sql gui-private

CONFIG += c++17 console precompile_header
CONFIG -= app_bundle

TARGET = et
PRECOMPILED_HEADER = stdafx.h
macx: {
    DESTDIR = $$PWD/../../bin/Derfla.app/Contents/Extensions/everything
} else {
    DESTDIR = $$PWD/../../bin/extensions/everything
}

win32-clang-msvc: CONFIG -= precompile_header
include($$PWD/../../3rdparty/qtsingleapplication/qtsingleapplication.pri)
include($$PWD/../../3rdparty/Boost.pri)

# workaround for non-MSVC mkspec on 5.12
equals(QT_MINOR_VERSION, 12): !win32-*msvc: LIBS += /usr/local/lib
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

lupdate.commands = $$QMAKE_LUPDATE $$PWD/everything.pro
lupdates.depends = $$SOURCES $$HEADERS $$FORMS $$TRANSLATIONS
lrelease.commands = $$QMAKE_LRELEASE $$PWD/everything.pro
lrelease.depends = lupdate
translate.depends = lrelease
QMAKE_EXTRA_TARGETS += lupdate lrelease translate qti18n
POST_TARGETDEPS += translate qti18n

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

    translate.commands = '$(COPY_DIR) $$shell_path($$PWD/translations) $$shell_path($$DESTDIR/translations)'

    qti18n.depends = translate
    qti18n.commands = '$(COPY_FILE) $$shell_path($$[QT_INSTALL_BINS]/../translations/qt_zh_CN.qm) $$shell_path($${DESTDIR}/translations/qt_zh_CN.qm)'

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
    LIBS+= -framework CoreServices -lobjc
}

copy_png.commands = '$(COPY_FILE) $$shell_path($$PWD/folder.png) $$shell_path($$DESTDIR)'
copy_cfg.commands = '$(COPY_FILE) $$shell_path($$PWD/extension.cfg) $$shell_path($$DESTDIR)'

QMAKE_EXTRA_TARGETS += copy_png copy_cfg
POST_TARGETDEPS += copy_png copy_cfg

RESOURCES += \
    everything.qrc
