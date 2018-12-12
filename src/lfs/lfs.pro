QT += core gui sql

CONFIG += c++17 console precompile_header
CONFIG -= app_bundle

TARGET = lfs
PRECOMPILED_HEADER = stdafx.h

macx: {
    DESTDIR = $$PWD/../../bin/Derfla.app/Contents/Extensions/lfs
} else {
    DESTDIR = $$PWD/../../bin/extensions/lfs
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

HEADERS += \
    stdafx.h \
    dbrw.h \
    localfsscanner.h \
    localfsitem.h

INCLUDEPATH += $$PWD

CODECFORTR      = UTF-8
CODECFORSRC     = UTF-8
TRANSLATIONS    = $$PWD/translations/lfs_en_US.ts \
                  $$PWD/translations/lfs_zh_CN.ts

isEmpty(QMAKE_LUPDATE) {
    win32:QMAKE_LUPDATE = $$[QT_INSTALL_BINS]\lupdate.exe
    else:QMAKE_LUPDATE = $$[QT_INSTALL_BINS]/lupdate
}

isEmpty(QMAKE_LRELEASE) {
    win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\lrelease.exe
    else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
}

lupdate.commands = $$QMAKE_LUPDATE $$PWD/lfs.pro
lupdates.depends = $$SOURCES $$HEADERS $$FORMS $$TRANSLATIONS
lrelease.commands = $$QMAKE_LRELEASE $$PWD/lfs.pro
lrelease.depends = lupdate
translate.depends = lrelease
QMAKE_EXTRA_TARGETS += lupdate lrelease translate qti18n
POST_TARGETDEPS += translate qti18n

win32: {
    win32-*g++* {
        DEFINES += WINVER=0x0600 _WIN32_WINNT=0x0600
    }
    win32-*msvc* {
        QMAKE_CXXFLAGS_RELEASE += /Zi
        QMAKE_LFLAGS_RELEASE += /DEBUG
    }
    RC_FILE = lfs.rc
    LIBS += -lVersion -lComctl32 -luser32 -lOle32 -lGdi32 -lShell32 -luuid -ladvapi32 -lwinmm

    translate.commands = '$(COPY_DIR) $$shell_path($$PWD/translations) $$shell_path($$DESTDIR/translations)'

    qti18n.depends = translate
    qti18n.commands = '$(COPY_FILE) $$shell_path($$[QT_INSTALL_BINS]/../translations/qt_zh_CN.qm) $$shell_path($${DESTDIR}/translations/qt_zh_CN.qm)'

    copy_cfg.commands = '$(COPY_FILE) $$shell_path($$PWD/extension.cfg) $$shell_path($$DESTDIR)'
    QMAKE_EXTRA_TARGETS += copy_cfg
    POST_TARGETDEPS += copy_cfg
}

macx: {
    include($$PWD/../../3rdparty/qtplist/qtplist.pri)
    copy_cfg.commands = '$(COPY_FILE) $$shell_path($$PWD/extension.cfg) $$shell_path($$DESTDIR)'
    QMAKE_EXTRA_TARGETS += copy_cfg
    POST_TARGETDEPS += copy_cfg
}

DISTFILES += \
    extension.cfg
