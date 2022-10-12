QT       += core gui widgets gui-private
equals(QT_MAJOR_VERSION, 6): QT+= core5compat

TARGET = Derfla
TEMPLATE = app
CONFIG += c++latest precompile_header
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
include($$PWD/../util/util.pri)

SOURCES += $$PWD/main.cpp\
    $$PWD/alfredui/alfredtheme.cpp \
    $$PWD/alfredui/doublelistitem.cpp \
    $$PWD/alfredui/listitem.cpp \
    $$PWD/alfredui/alfredlistwidget.cpp \
    $$PWD/alfredui/plaintext.cpp \
    $$PWD/alfredui/alfredwidget.cpp \
    $$PWD/derflaui/derflawidget.cpp \
    $$PWD/derflaui/charlineedit.cpp \
    $$PWD/derflaui/candidatelist.cpp \
    $$PWD/derflaui/candidatelistdelegate.cpp \
    $$PWD/derflaui/candidatelistwidget.cpp\
    $$PWD/derflaui/skinmanager.cpp \
    $$PWD/extension/extension.cpp \
    $$PWD/extension/extensionmanager.cpp \
    $$PWD/extension/extensionmodel.cpp \
    $$PWD/updater/autoupdater.cpp \
    $$PWD/commonui/preferencedialog.cpp \
    $$PWD/commonui/booleaneditor.cpp \
    $$PWD/derflaapp.cpp \
    $$PWD/actionexecutor.cpp  \
    $$PWD/alfredui/thememanager.cpp \
    $$PWD/commonui/commonwidget.cpp

HEADERS  += $$PWD/stdafx.h \
    $$PWD/alfredui/alfredtheme.h \
    $$PWD/alfredui/doublelistitem.h \
    $$PWD/alfredui/geticon.h \
    $$PWD/alfredui/listitem.h \
    $$PWD/alfredui/alfredlistwidget.h \
    $$PWD/alfredui/plaintext.h \
    $$PWD/alfredui/alfredwidget.h \
    $$PWD/derflaui/derflawidget.h \
    $$PWD/derflaui/charlineedit.h \
    $$PWD/derflaui/candidatelist.h \
    $$PWD/derflaui/candidatelistdelegate.h \
    $$PWD/derflaui/candidatelistwidget.h\
    $$PWD/derflaui/skinmanager.h \
    $$PWD/extension/extension.h \
    $$PWD/extension/extensionmanager.h \
    $$PWD/extension/extensionmodel.h \
    $$PWD/updater/autoupdater.h \
    $$PWD/commonui/preferencedialog.h \
    $$PWD/commonui/booleaneditor.h \
    $$PWD/derflaapp.h \
    $$PWD/actionexecutor.h  \
    $$PWD/alfredui/thememanager.h \
    $$PWD/commonui/commonwidget.h

FORMS    += \
    $$PWD/alfredui/doublelistitem.ui \
    $$PWD/alfredui/listitem.ui \
    $$PWD/alfredui/alfredwidget.ui \
    $$PWD/derflaui/candidatelist.ui \
    $$PWD/commonui/preferencedialog.ui

INCLUDEPATH += $$PWD \
    $$PWD/alfredui \
    $$PWD/derflaui \
    $$PWD/commonui \
    $$PWD/updater \
    $$PWD/extension

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
    INCLUDEPATH += $$PWD/macui

    OBJECTIVE_SOURCES += \
        $$PWD/updater/SparkleAutoUpdater.mm \
        $$PWD/macui/CocoaInitializer.mm \
        $$PWD/macui/darkmode.mm
    HEADERS += $$PWD/macui/CocoaInitializer.h \
        $$PWD/updater/SparkleAutoUpdater.h

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
        install_name_tool -change libDerflaUtil.1.dylib @executable_path/../Libs/libDerflaUtil.1.dylib $$DESTDIR/Derfla.app/Contents/MacOS/Derfla && \
        cp -avf $$PWD/skins  $$DESTDIR/Derfla.app/Contents/Resources && \
        cp -avf $$PWD/themes $$DESTDIR/Derfla.app/Contents/Resources && \
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
        qti18n.commands = '$(COPY_FILE) $$shell_path($$[QT_INSTALL_BINS]/../share/qt5/translations/qt_zh_CN.qm) $$shell_path($${DESTDIR}/translations/qt_zh_CN.qm)'
    }
    win32-*msvc* {
        QMAKE_CXXFLAGS_RELEASE += /Zi
        QMAKE_LFLAGS_RELEASE += /DEBUG
        qti18n.commands = '$(COPY_FILE) $$shell_path($$[QT_INSTALL_BINS]/../translations/qt_zh_CN.qm) $$shell_path($${DESTDIR}/translations/qt_zh_CN.qm)'
    }
    include($$PWD/../../3rdparty/qglobalshortcut/qglobalshortcut.pri)
    include($$PWD/../../3rdparty/WinSparkle/winsparkle.pri)
    SOURCES += \
        $$PWD/updater/winsparkleautoupdater.cpp
    HEADERS += \
        $$PWD/updater/winsparkleautoupdater.h

    translate.commands = '$(COPY_DIR) $$shell_path($$PWD/translations) $$shell_path($$DESTDIR/translations)'

    qti18n.depends = translate
	
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
    copy_theme.commands = '$(COPY_DIR) $$shell_path($$PWD/themes) $$shell_path($$DESTDIR/themes)'

    QMAKE_EXTRA_TARGETS += copy_winsparkle copy_skin copy_theme
    POST_TARGETDEPS += copy_winsparkle copy_skin copy_theme
}

RESOURCES += \
    $$PWD/derfla.qrc

DISTFILES += \
    $$PWD/rc/updater-dsa-pub.pem

