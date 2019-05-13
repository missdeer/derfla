QT       += core gui widgets xml gui-private

TARGET = Derfla
TEMPLATE = app
CONFIG += c++17 precompile_header
PRECOMPILED_HEADER = stdafx.h
DESTDIR = $$PWD/../../bin

contains(QMAKE_HOST.arch, x86_64): {
    DESTDIR = $$DESTDIR/x86_64
} else: {
    DESTDIR = $$DESTDIR/x86
}

win32-clang-msvc: CONFIG -= precompile_header

!win32: include($$PWD/../../3rdparty/UGlobalHotkey/uglobalhotkey.pri)
include($$PWD/../../3rdparty/qtsingleapplication/qtsingleapplication.pri)
include($$PWD/../../3rdparty/Boost.pri)

include($$PWD/../util/util.pri)

SOURCES += main.cpp\
    alfredui/config.cpp \
    alfredui/config_parse.cpp \
    alfredui/defaulsearch.cpp \
    alfredui/doublelistitem.cpp \
    alfredui/frequenceparse.cpp \
    alfredui/listitem.cpp \
    alfredui/alfredlistwidget.cpp \
    alfredui/plaintext.cpp \
    alfredui/returnbyscript.cpp \
    alfredui/theme.cpp \
    alfredui/alfredwidget.cpp \
    alfredui/widgetthread.cpp \
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
    alfredui/config.h \
    alfredui/config_parse.h \
    alfredui/defaulsearch.h \
    alfredui/doublelistitem.h \
    alfredui/frequenceparse.h \
    alfredui/geticon.h \
    alfredui/listitem.h \
    alfredui/alfredlistwidget.h \
    alfredui/plaintext.h \
    alfredui/returnbyscript.h \
    alfredui/theme.h \
    alfredui/alfredwidget.h \
    alfredui/widgetthread.h \
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
    alfredui/doublelistitem.ui \
    alfredui/listitem.ui \
    alfredui/alfredwidget.ui \
    candidatelist.ui \
    preferencedialog.ui

INCLUDEPATH += $$PWD $$PWD/alfredui

CODECFORTR      = UTF-8
CODECFORSRC     = UTF-8
TRANSLATIONS    = $$PWD/translations/derfla_en_US.ts \
                  $$PWD/translations/derfla_zh_CN.ts

isEmpty(QMAKE_LUPDATE) {
    win32:QMAKE_LUPDATE = $$[QT_INSTALL_BINS]\lupdate.exe
    else:QMAKE_LUPDATE = $$[QT_INSTALL_BINS]/lupdate
}

isEmpty(QMAKE_LRELEASE) {
    win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\lrelease.exe
    else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
}

lupdate.commands = $$QMAKE_LUPDATE -no-obsolete $$PWD/derfla.pro
lupdates.depends = $$SOURCES $$HEADERS $$FORMS $$TRANSLATIONS
lrelease.commands = $$QMAKE_LRELEASE $$PWD/derfla.pro
lrelease.depends = lupdate
translate.depends = lrelease
QMAKE_EXTRA_TARGETS += lupdate lrelease translate qti18n
POST_TARGETDEPS += translate qti18n

macx: {
#    QMAKE_MAC_SDK = macosx10.11
    ICON = derfla.icns
    icon.path = $$PWD
    icon.files += derfla.png
    INSTALLS += icon

    OBJECTIVE_SOURCES += \
        CocoaInitializer.mm \
        SparkleAutoUpdater.mm \
        darkmode.mm
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


    MACDEPLOYQT = $$[QT_INSTALL_BINS]/macdeployqt

    translate.depends = lrelease
    translate.files = $$system("find $${PWD}/translations -name '*.qm' ")
    translate.path = Contents/Resources/translations/
    translate.commands = '$(COPY_DIR) $$shell_path($${PWD}/translations) $$shell_path($${DESTDIR}/$${TARGET}.app/Contents/Resources/)'

    qti18n.depends = translate
    qti18n.commands = '$(COPY_FILE) $$shell_path($$[QT_INSTALL_BINS]/../translations/qt_zh_CN.qm) $$shell_path($${DESTDIR}/$${TARGET}.app/Contents/Resources/translations/qt_zh_CN.qm)'

    QMAKE_BUNDLE_DATA += translate qti18n

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

    translate.commands = '$(COPY_DIR) $$shell_path($$PWD/translations) $$shell_path($$DESTDIR/translations)'

    qti18n.depends = translate
    qti18n.commands = '$(COPY_FILE) $$shell_path($$[QT_INSTALL_BINS]/../translations/qt_zh_CN.qm) $$shell_path($${DESTDIR}/translations/qt_zh_CN.qm)'
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

unix: !macx: {
    HEADERS += alfredui/gpixbuftoqicon.h
    SOURCES += alfredui/gpixbuftoqicon.cpp
}

RESOURCES += \
    derfla.qrc

DISTFILES += \
    rc/updater-dsa-pub.pem
