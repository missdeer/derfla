QT       += core gui widgets xml concurrent sql gui-private

TARGET = Derfla
TEMPLATE = app
CONFIG += c++14 precompile_header
PRECOMPILED_HEADER = stdafx.h
DESTDIR = ../../bin

include($$PWD/../../3rdparty/UGlobalHotkey/uglobalhotkey.pri)
include($$PWD/../../3rdparty/qtsingleapplication/qtsingleapplication.pri)
include($$PWD/../../3rdparty/Boost.pri)

SOURCES += main.cpp\
        gui/derflawidget.cpp \
    gui/CharLineEdit.cpp \
    gui/candidatelist.cpp \
    gui/candidatelistdelegate.cpp \
    gui/candidatelistwidget.cpp \
    gui/derflaaction.cpp \
    executor/executor.cpp \
    executor/bashexecutor.cpp \
    executor/zshexecutor.cpp \
    executor/phpexecutor.cpp \
    executor/pythonexecutor.cpp \
    executor/perlexecutor.cpp \
    executor/rubyexecutor.cpp \
    executil/scriptescape.cpp \
    executil/executorrunner.cpp \
    executil/processwrapper.cpp

HEADERS  += stdafx.h \
    gui/derflawidget.h \
    gui/CharLineEdit.h \
    gui/candidatelist.h \
    gui/candidatelistdelegate.h \
    gui/candidatelistwidget.h \
    gui/derflaaction.h \
    executor/executor.h \
    executor/bashexecutor.h \
    executor/zshexecutor.h \
    executor/phpexecutor.h \
    executor/pythonexecutor.h \
    executor/perlexecutor.h \
    executor/rubyexecutor.h \
    executil/scriptescape.h \
    executil/executorrunner.h \
    executil/processwrapper.h

FORMS    += \
    gui/candidatelist.ui

INCLUDEPATH += $$PWD $$PWD/gui $$PWD/executor $$PWD/executil

CONFIG(release, debug|release) : {
    DEFINES += QT_NO_DEBUG_OUTPUT=1 QT_NO_INFO_OUTPUT=1
}

unix: !macx: {
    LIBS += -lz
}

macx: {
    SOURCES +=  \
        executor/osascriptexecutor.cpp

    HEADERS +=  \
        executor/osascriptexecutor.h

#    QMAKE_MAC_SDK = macosx10.11
    ICON = derfla.icns
    icon.path = $$PWD
    icon.files += derfla.png
    INSTALLS += icon
    LIBS+=-L$$PWD/../../3rdparty/zlib-1.2.8 -lz

    CONFIG(release, debug|release) : {
    }
    QMAKE_INFO_PLIST = osxInfo.plist
#    copy_skins.commands = 'cp -R \"$$PWD/skins\" \"$${TARGET}.app/Contents/Resources\"'
#    QMAKE_EXTRA_TARGETS +=  copy_skins
#    POST_TARGETDEPS += copy_skins
#    QMAKE_POST_LINK += $$quote(cp -R \"$$PWD/skins\" \"$${TARGET}.app/Contents/Resources\")
}

win32: {
    win32-msvc* {
        QMAKE_LFLAGS += "/NODEFAULTLIB:MSVCRT"
        LIBS += -L$$PWD/../../3rdparty/zlib-1.2.8 -lzlib
    } else {
        DEFINES += WINVER=0x0600 _WIN32_WINNT=0x0600
        LIBS += -lz
    }
    QT += winextras

    # Windows icons
    RC_FILE = derfla.rc
    DISTFILES += derfla.rc
    LIBS += -L$$PWD/../../3rdparty/Everything-SDK/lib -lVersion -lComctl32 -lOle32 -lGdi32
    INCLUDEPATH += $$PWD/../../3rdparty/Everything-SDK/include \
        $$PWD/../../3rdparty/Everything-SDK/ipc

    contains(QMAKE_HOST.arch, x86_64): LIBS += -lEverything64
    else: LIBS += -lEverything32

#    CONFIG(release, debug|release): {
#        copy_skins.commands = '$(COPY_DIR) $$shell_path($$PWD/skins) $$shell_path($$OUT_PWD/Release/skins/)'
#    } else {
#        copy_skins.commands = '$(COPY_DIR) $$shell_path($$PWD/skins) $$shell_path($$OUT_PWD/Debug/skins/)'
#    }
#    QMAKE_EXTRA_TARGETS +=  copy_skins
#    POST_TARGETDEPS += copy_skins
}

RESOURCES += \
    derfla.qrc
