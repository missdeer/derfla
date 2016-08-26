QT       += core gui widgets xml concurrent sql

TARGET = Derfla
TEMPLATE = app
CONFIG += c++11 precompile_header
PRECOMPILED_HEADER = stdafx.h
DEFINES += QT_USE_QSTRINGBUILDER=1

include($$PWD/../../3rdparty/UGlobalHotkey/uglobalhotkey.pri)
include($$PWD/../../3rdparty/quazip-0.7.2/quazip.pri)
include($$PWD/../../3rdparty/qtsingleapplication/qtsingleapplication.pri)
include($$PWD/../../3rdparty/Boost.pri)
include($$PWD/../../3rdparty/sqlite3/sqlite3.pri)
include($$PWD/../../3rdparty/qtplist/qtplist.pri)

SOURCES += main.cpp\
        gui/derflawidget.cpp \
    gui/CharLineEdit.cpp \
    gui/candidatelist.cpp \
    gui/candidatelistdelegate.cpp \
    gui/candidatelistwidget.cpp \
    gui/derflaaction.cpp \
    lfs/localfsscanner.cpp \
    lfs/dbrw.cpp \
    util/util.cpp \
    alfred/alfredworkflow.cpp \
    alfred/alfredworkflowinput.cpp \
    alfred/alfredworkflowoutput.cpp \
    alfred/alfredworkflowaction.cpp \
    alfred/alfredworkflowtrigger.cpp \
    alfred/alfredworkflowscriptescape.cpp \
    executor/executor.cpp \
    executor/bashexecutor.cpp \
    executor/zshexecutor.cpp \
    executor/phpexecutor.cpp \
    executor/pythonexecutor.cpp \
    executor/perlexecutor.cpp \
    executor/rubyexecutor.cpp

HEADERS  += stdafx.h \
    gui/derflawidget.h \
    gui/CharLineEdit.h \
    gui/candidatelist.h \
    gui/candidatelistdelegate.h \
    gui/candidatelistwidget.h \
    gui/derflaaction.h \
    lfs/localfsscanner.h \
    lfs/dbrw.h \
    util/util.h \
    alfred/alfredworkflow.h \
    alfred/alfredworkflowinput.h \
    alfred/alfredworkflowoutput.h \
    alfred/alfredworkflowaction.h \
    alfred/alfredworkflowtrigger.h \
    alfred/alfredworkflowscriptescape.h \
    executor/executor.h \
    executor/bashexecutor.h \
    executor/zshexecutor.h \
    executor/phpexecutor.h \
    executor/pythonexecutor.h \
    executor/perlexecutor.h \
    executor/rubyexecutor.h

FORMS    += \
    gui/candidatelist.ui

INCLUDEPATH += $$PWD $$PWD/gui $$PWD/lfs $$PWD/alfred $$PWD/util $$PWD/executor

macx: {

    SOURCES +=  \
        executor/osascriptexecutor.cpp

    HEADERS +=  \
        executor/osascriptexecutor.h

    QMAKE_MAC_SDK = macosx10.11
    ICON = derfla.icns
    icon.path = $$PWD
    icon.files += derfla.png
    INSTALLS += icon
    LIBS+=-L$$PWD/../../3rdparty/zlib-1.2.8 -lz

    CONFIG(release, debug|release) : {
    }
    QMAKE_INFO_PLIST = osxInfo.plist
    copy_skins.commands = 'cp -R \"$$PWD/skins\" \"$${TARGET}.app/Contents/Resources\"'
    QMAKE_EXTRA_TARGETS +=  copy_skins
    POST_TARGETDEPS += copy_skins
    QMAKE_POST_LINK += $$quote(cp -R \"$$PWD/skins\" \"$${TARGET}.app/Contents/Resources\")
}

win32: {

    QT += winextras

    SOURCES += util/win_util.cpp \
        gui/WinIconProvider.cpp

    HEADERS += util/win_util.h \
        gui/WinIconProvider.h

    # Windows icons
    RC_FILE = derfla.rc
    DISTFILES += derfla.rc
    LIBS+=-L$$PWD/../../3rdparty/zlib-1.2.8 \
        -L$$PWD/../../3rdparty/Everything-SDK/lib \
        -lzlib -lOle32 -lVersion -lComctl32 -lGdi32
    INCLUDEPATH += $$PWD/../../3rdparty/Everything-SDK/include \
        $$PWD/../../3rdparty/Everything-SDK/ipc

    contains(QMAKE_HOST.arch, x86_64): LIBS += -lEverything64
    else: LIBS += -lEverything32

    QMAKE_LFLAGS += "/LTCG"
    CONFIG(release, debug|release): {
        copy_skins.commands = '$(COPY_DIR) $$shell_path($$PWD/skins) $$shell_path($$OUT_PWD/Release/skins/)'
    } else {
        copy_skins.commands = '$(COPY_DIR) $$shell_path($$PWD/skins) $$shell_path($$OUT_PWD/Debug/skins/)'
    }
    QMAKE_EXTRA_TARGETS +=  copy_skins
    POST_TARGETDEPS += copy_skins
}

RESOURCES += \
    derfla.qrc
