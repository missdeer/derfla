QT += core gui network
equals(QT_MAJOR_VERSION, 6): QT+= core5compat

CONFIG += c++latest console precompile_header
CONFIG -= app_bundle

TARGET = webdict
PRECOMPILED_HEADER = stdafx.h

macx: {
    DESTDIR = $$PWD/../../bin/x86_64/Derfla.app/Contents/Extensions/webdict
} else {
    contains(QMAKE_HOST.arch, x86_64): {
        DESTDIR = $$PWD/../../bin/x86_64/extensions/webdict
    } else: {
        DESTDIR = $$PWD/../../bin/x86/extensions/webdict
    }
}

win32-clang-msvc: CONFIG -= precompile_header
include($$PWD/../../3rdparty/qtsingleapplication/qtsingleapplication.pri)

include($$PWD/../util/util.pri)
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
    youdao.cpp

HEADERS += \
    stdafx.h \
    youdao.h

INCLUDEPATH += $$PWD

CODECFORTR      = UTF-8
CODECFORSRC     = UTF-8
TRANSLATIONS    = $$PWD/translations/webdict_en_US.ts \
                  $$PWD/translations/webdict_zh_CN.ts

isEmpty(QMAKE_LUPDATE) {
    win32:QMAKE_LUPDATE = $$[QT_INSTALL_BINS]\lupdate.exe
    else:QMAKE_LUPDATE = $$[QT_INSTALL_BINS]/lupdate
}

isEmpty(QMAKE_LRELEASE) {
    win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\lrelease.exe
    else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
}

lupdate.commands = $$QMAKE_LUPDATE -no-obsolete $$PWD/webdict.pro
lupdates.depends = $$SOURCES $$HEADERS $$FORMS $$TRANSLATIONS
lrelease.commands = $$QMAKE_LRELEASE $$PWD/webdict.pro
lrelease.depends = lupdate
translate.depends = lrelease
translate.commands = '$(COPY_DIR) $$shell_path($$PWD/translations) $$shell_path($$DESTDIR/)'
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
    RC_FILE = webdict.rc
    LIBS += -lVersion -lComctl32 -luser32 -lOle32 -lGdi32 -lShell32 -luuid -ladvapi32 -lwinmm

    copy_cfg.commands = '$(COPY_FILE) $$shell_path($$PWD/extension.derflaext) $$shell_path($$DESTDIR)'
    QMAKE_EXTRA_TARGETS += copy_cfg
    POST_TARGETDEPS += copy_cfg
}

macx: {
    copy_cfg.commands = '$(COPY_FILE) $$shell_path($$PWD/extension.derflaext) $$shell_path($$DESTDIR)'
    QMAKE_EXTRA_TARGETS += copy_cfg
    POST_TARGETDEPS += copy_cfg
}

DISTFILES += \
    $$PWD/extension.derflaext

RESOURCES += \
    $$PWD/webdict.qrc
