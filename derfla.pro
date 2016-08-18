QT       += core gui widgets

TARGET = Derfla
TEMPLATE = app


SOURCES += main.cpp\
        derflawidget.cpp \
    CharLineEdit.cpp

HEADERS  += derflawidget.h \
    CharLineEdit.h

FORMS    +=


macx: {
    OBJECTIVE_SOURCES += \
        mdfindwrapper.mm

    QMAKE_MAC_SDK = macosx10.11
    ICON = cjlv.icns
    icon.path = $$PWD
    #icon.files += cjlv.png
    INSTALLS += icon
    INCLUDEPATH += /usr/local/include
    LIBS+=-L$$PWD/3rdparty/zlib-1.2.8 -lz -framework CoreServices -lobjc

    CONFIG(release, debug|release) : {
        QMAKE_INFO_PLIST = osxInfo.plist
        copy_skins.commands = 'cp -R \"$$PWD/extensions\" \"$${TARGET}.app/Contents/PlugIns\"'
        QMAKE_EXTRA_TARGETS +=  copy_skins
        POST_TARGETDEPS += copy_skins
        QMAKE_POST_LINK += $$quote(cp -R \"$$PWD/extensions\" \"$${TARGET}.app/Contents/PlugIns\")
    }
}

win32: {
    QMAKE_LFLAGS += "/LTCG"
    CONFIG(release, debug|release): {
        copy_skins.commands = '$(COPY_DIR) $$shell_path($$PWD/skins) $$shell_path($$OUT_PWD/Release/skins/)'

        QMAKE_EXTRA_TARGETS +=  copy_skins
        POST_TARGETDEPS += copy_skins
    }
}
