QT       += core gui widgets xml concurrent sql

TARGET = Derfla
TEMPLATE = app
CONFIG += c++11 precompile_header
PRECOMPILED_HEADER = stdafx.h

include($$PWD/../../3rdparty/UGlobalHotkey/uglobalhotkey.pri)
include($$PWD/../../3rdparty/quazip-0.7.2/quazip.pri)
include($$PWD/../../3rdparty/qtsingleapplication/qtsingleapplication.pri)
include($$PWD/../../3rdparty/Boost.pri)

SOURCES += main.cpp\
        derflawidget.cpp \
    CharLineEdit.cpp \
    candidatelist.cpp \
    candidatelistdelegate.cpp \
    candidatelistwidget.cpp

HEADERS  += derflawidget.h \
    CharLineEdit.h \
    candidatelist.h \
    candidatelistdelegate.h \
    candidatelistwidget.h \
    stdafx.h

FORMS    += \
    candidatelist.ui


macx: {
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
    # Windows icons
    RC_FILE = derfla.rc
    DISTFILES += derfla.rc
    LIBS+=-L$$PWD/../../3rdparty/zlib-1.2.8 \
        -L$$PWD/../../3rdparty/Everything-SDK/lib \
        -lzlib
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
