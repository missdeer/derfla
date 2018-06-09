QT       += core gui widgets xml concurrent sql gui-private

TARGET = Derfla
TEMPLATE = app
CONFIG += c++14 precompile_header
PRECOMPILED_HEADER = stdafx.h
DESTDIR = ../../bin

win32-clang-msvc: CONFIG -= precompile_header

include($$PWD/../../3rdparty/UGlobalHotkey/uglobalhotkey.pri)
include($$PWD/../../3rdparty/qtsingleapplication/qtsingleapplication.pri)
include($$PWD/../../3rdparty/Boost.pri)

SOURCES += main.cpp\
    derflawidget.cpp \
    charlineedit.cpp \
    candidatelist.cpp \
    candidatelistdelegate.cpp \
    candidatelistwidget.cpp \
    derflaaction.cpp \
    actionexecutor.cpp \
    extension.cpp \
    extensionmanager.cpp

HEADERS  += stdafx.h \
    derflawidget.h \
    charlineedit.h \
    candidatelist.h \
    candidatelistdelegate.h \
    candidatelistwidget.h \
    derflaaction.h \
    actionexecutor.h \
    extension.h \
    extensionmanager.h

FORMS    += \
    candidatelist.ui

INCLUDEPATH += $$PWD

macx: {
#    QMAKE_MAC_SDK = macosx10.11
    ICON = derfla.icns
    icon.path = $$PWD
    icon.files += derfla.png
    INSTALLS += icon

    QMAKE_INFO_PLIST = osxInfo.plist
#    copy_skins.commands = 'cp -R \"$$PWD/skins\" \"$${TARGET}.app/Contents/Resources\"'
#    QMAKE_EXTRA_TARGETS +=  copy_skins
#    POST_TARGETDEPS += copy_skins
#    QMAKE_POST_LINK += $$quote(cp -R \"$$PWD/skins\" \"$${TARGET}.app/Contents/Resources\")
}

win32: {
    win32-g++* {
        DEFINES += WINVER=0x0600 _WIN32_WINNT=0x0600
    }

    # Windows icons
    RC_FILE = derfla.rc
    LIBS += -lVersion -lComctl32 -lOle32 -lGdi32
}

RESOURCES += \
    derfla.qrc
