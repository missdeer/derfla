QT += core gui

CONFIG += c++17 console precompile_header
CONFIG -= app_bundle

TARGET = calculator
PRECOMPILED_HEADER = stdafx.h

macx: {
    DESTDIR = $$PWD/../../bin/x86_64/Derfla.app/Contents/Extensions/calculator
} else {
    contains(QMAKE_HOST.arch, x86_64): {
        DESTDIR = $$PWD/../../bin/x86_64/extensions/calculator
    } else: {
        DESTDIR = $$PWD/../../bin/x86/extensions/calculator
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

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH += $$PWD $$PWD/core $$PWD/math

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/core/book.cpp \
    $$PWD/core/constants.cpp \
    $$PWD/core/evaluator.cpp \
    $$PWD/core/functions.cpp \
    $$PWD/core/manualserver.cpp \
    $$PWD/core/numberformatter.cpp \
    $$PWD/core/opcode.cpp \
    $$PWD/core/pageserver.cpp \
    $$PWD/core/session.cpp \
    $$PWD/core/sessionhistory.cpp \
    $$PWD/core/settings.cpp \
    $$PWD/core/userfunction.cpp \
    $$PWD/core/variable.cpp \
    $$PWD/math/cmath.cpp \
    $$PWD/math/cnumberparser.cpp \
    $$PWD/math/floatcommon.c \
    $$PWD/math/floatconst.c \
    $$PWD/math/floatconvert.c \
    $$PWD/math/floaterf.c \
    $$PWD/math/floatexp.c \
    $$PWD/math/floatgamma.c \
    $$PWD/math/floathmath.c \
    $$PWD/math/floatincgamma.c \
    $$PWD/math/floatio.c \
    $$PWD/math/floatipower.c \
    $$PWD/math/floatlog.c \
    $$PWD/math/floatlogic.c \
    $$PWD/math/floatlong.c \
    $$PWD/math/floatnum.c \
    $$PWD/math/floatpower.c \
    $$PWD/math/floatseries.c \
    $$PWD/math/floattrig.c \
    $$PWD/math/hmath.cpp \
    $$PWD/math/number.c \
    $$PWD/math/quantity.cpp \
    $$PWD/math/rational.cpp \
    $$PWD/math/units.cpp

HEADERS += \
    $$PWD/stdafx.h \
    $$PWD/core/book.h \
    $$PWD/core/constants.h \
    $$PWD/core/errors.h \
    $$PWD/core/evaluator.h \
    $$PWD/core/functions.h \
    $$PWD/core/manualserver.h \
    $$PWD/core/numberformatter.h \
    $$PWD/core/opcode.h \
    $$PWD/core/pageserver.h \
    $$PWD/core/session.h \
    $$PWD/core/sessionhistory.h \
    $$PWD/core/settings.h \
    $$PWD/core/userfunction.h \
    $$PWD/core/variable.h \
    $$PWD/math/cmath.h \
    $$PWD/math/cnumberparser.h \
    $$PWD/math/floatcommon.h \
    $$PWD/math/floatconfig.h \
    $$PWD/math/floatconst.h \
    $$PWD/math/floatconvert.h \
    $$PWD/math/floaterf.h \
    $$PWD/math/floatexp.h \
    $$PWD/math/floatgamma.h \
    $$PWD/math/floathmath.h \
    $$PWD/math/floatincgamma.h \
    $$PWD/math/floatio.h \
    $$PWD/math/floatipower.h \
    $$PWD/math/floatlog.h \
    $$PWD/math/floatlogic.h \
    $$PWD/math/floatlong.h \
    $$PWD/math/floatnum.h \
    $$PWD/math/floatpower.h \
    $$PWD/math/floatseries.h \
    $$PWD/math/floattrig.h \
    $$PWD/math/hmath.h \
    $$PWD/math/number.h \
    $$PWD/math/quantity.h \
    $$PWD/math/rational.h \
    $$PWD/math/units.h

DEFINES += SPEEDCRUNCH_VERSION=\\\"master\\\"
DEFINES += QT_USE_QSTRINGBUILDER

INCLUDEPATH += $$PWD

CODECFORTR      = UTF-8
CODECFORSRC     = UTF-8
TRANSLATIONS    = $$PWD/translations/calculator_en_US.ts \
                  $$PWD/translations/calculator_zh_CN.ts

isEmpty(QMAKE_LUPDATE) {
    win32:QMAKE_LUPDATE = $$[QT_INSTALL_BINS]\lupdate.exe
    else:QMAKE_LUPDATE = $$[QT_INSTALL_BINS]/lupdate
}

isEmpty(QMAKE_LRELEASE) {
    win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\lrelease.exe
    else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
}

lupdate.commands = $$QMAKE_LUPDATE -no-obsolete $$PWD/calculator.pro
lupdates.depends = $$SOURCES $$HEADERS $$FORMS $$TRANSLATIONS
lrelease.commands = $$QMAKE_LRELEASE $$PWD/calculator.pro
lrelease.depends = lupdate
translate.depends = lrelease
translate.commands = '$(COPY_DIR) $$shell_path($$PWD/translations) $$shell_path($$DESTDIR/)'
QMAKE_EXTRA_TARGETS += lupdate lrelease translate
POST_TARGETDEPS += translate

win32: {
    win32-*g++* {
        DEFINES += WINVER=0x0600 _WIN32_WINNT=0x0600 STRSAFE_NO_DEPRECATE
    }
    win32-*msvc* {
        QMAKE_CXXFLAGS_RELEASE += /Zi
        QMAKE_LFLAGS_RELEASE += /DEBUG
    }
    RC_FILE = calculator.rc
    DEFINES += _USE_MATH_DEFINES _CRT_SECURE_NO_WARNINGS _CRT_NONSTDC_NO_WARNINGS _SCL_SECURE_NO_WARNINGS 
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
    $$PWD/calculator.qrc
