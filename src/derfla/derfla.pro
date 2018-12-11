QT       += core gui widgets xml gui-private

TARGET = Derfla
TEMPLATE = app
CONFIG += c++17 precompile_header
PRECOMPILED_HEADER = stdafx.h
DESTDIR = ../../bin

win32-clang-msvc: CONFIG -= precompile_header

!win32: include($$PWD/../../3rdparty/UGlobalHotkey/uglobalhotkey.pri)
include($$PWD/../../3rdparty/qtsingleapplication/qtsingleapplication.pri)
include($$PWD/../../3rdparty/Boost.pri)

# workaround for non-MSVC mkspec on 5.12
equals(QT_MINOR_VERSION, 12): !win32-*msvc: LIBS += /usr/local/lib
include($$PWD/../util/util.pri)

SOURCES += main.cpp\
    derflawidget.cpp \
    charlineedit.cpp \
    candidatelist.cpp \
    candidatelistdelegate.cpp \
    candidatelistwidget.cpp \
    derflaaction.cpp \
    actionexecutor.cpp \
    extension.cpp \
    extensionmanager.cpp \
    autoupdater.cpp \
    preferencedialog.cpp \
    skinmanager.cpp \
    extensionmodel.cpp \
    booleaneditor.cpp

HEADERS  += stdafx.h \
    derflawidget.h \
    charlineedit.h \
    candidatelist.h \
    candidatelistdelegate.h \
    candidatelistwidget.h \
    derflaaction.h \
    actionexecutor.h \
    extension.h \
    extensionmanager.h \
    autoupdater.h \
    preferencedialog.h \
    skinmanager.h \
    extensionmodel.h \
    booleaneditor.h

FORMS    += \
    candidatelist.ui \
    preferencedialog.ui

INCLUDEPATH += $$PWD

macx: {
#    QMAKE_MAC_SDK = macosx10.11
    ICON = derfla.icns
    icon.path = $$PWD
    icon.files += derfla.png
    INSTALLS += icon

    OBJECTIVE_SOURCES += \
        CocoaInitializer.mm \
        SparkleAutoUpdater.mm
    HEADERS += CocoaInitializer.h SparkleAutoUpdater.h

    LIBS += -F$$PWD/../../3rdparty/Sparkle \
            -framework AppKit \
            -framework Carbon \
            -framework Foundation \
            -framework ApplicationServices \
            -framework Sparkle \
            -framework CoreServices -lobjc

    QMAKE_LFLAGS += -Wl,-rpath,@loader_path/../Frameworks
    QMAKE_LFLAGS += -F$$PWD/../../3rdparty/Sparkle
    QMAKE_CXXFLAGS += -F$$PWD/../../3rdparty/Sparkle
    QMAKE_CFLAGS += -F$$PWD/../../3rdparty/Sparkle
    QMAKE_OBJECTIVE_CFLAGS += -F$$PWD/../../3rdparty/Sparkle

    QMAKE_POST_LINK = mkdir -p $$DESTDIR/Derfla.app/Contents/Frameworks && \
        rm -rf $$DESTDIR/Derfla.app/Contents/Frameworks/Sparkle.framework && \
        cp -avf $$PWD/../../3rdparty/Sparkle/Sparkle.framework $$DESTDIR/Derfla.app/Contents/Frameworks && \
        install_name_tool -change libderflautil.1.dylib @executable_path/../Libs/libderflautil.1.dylib $$DESTDIR/Derfla.app/Contents/MacOS/Derfla && \
        cp -avf $$PWD/skins  $$DESTDIR/Derfla.app/Contents/Resources && \
        cp $$PWD/rc/updater-dsa-pub.pem $$DESTDIR/Derfla.app/Contents/Resources/dsa_pub.pem

    QMAKE_INFO_PLIST = osxInfo.plist
}

win32: {
    win32-*g++* {
        DEFINES += WINVER=0x0600 _WIN32_WINNT=0x0600
    }
    win32-*msvc* {
        QMAKE_CXXFLAGS_RELEASE += /Zi
        QMAKE_LFLAGS_RELEASE += /DEBUG
    }
    include($$PWD/../../3rdparty/qglobalshortcut/qglobalshortcut.pri)
    include($$PWD/../../3rdparty/WinSparkle/winsparkle.pri)
    SOURCES += \
        winsparkleautoupdater.cpp
    HEADERS += \
        winsparkleautoupdater.h

    # Windows icons
    RC_FILE = derfla.rc
    LIBS += -lVersion -lComctl32 -lOle32 -lGdi32

    contains(QMAKE_HOST.arch, x86_64): {
        copy_winsparkle.commands = '$(COPY_FILE) $$shell_path($$PWD/../../3rdparty/WinSparkle/x64/Release/WinSparkle.dll) $$shell_path($$DESTDIR)'
    }
    else: {
        copy_winsparkle.commands = '$(COPY_FILE) $$shell_path($$PWD/../../3rdparty/WinSparkle/Release/WinSparkle.dll) $$shell_path($$DESTDIR)'
    }
    copy_skin.commands = '$(COPY_DIR) $$shell_path($$PWD/skins) $$shell_path($$DESTDIR/skins)'

    QMAKE_EXTRA_TARGETS += copy_winsparkle copy_skin
    POST_TARGETDEPS += copy_winsparkle copy_skin
}

RESOURCES += \
    derfla.qrc

DISTFILES += \
    rc/updater-dsa-pub.pem
