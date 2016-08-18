QT       += core gui widgets xml

TARGET = Derfla
TEMPLATE = app
CONFIG += c++11

SOURCES += main.cpp\
        derflawidget.cpp \
    CharLineEdit.cpp \
    candidatelist.cpp \
    candidatelistdelegate.cpp

HEADERS  += derflawidget.h \
    CharLineEdit.h \
    candidatelist.h \
    candidatelistdelegate.h

FORMS    += \
    candidatelist.ui


macx: {
    QMAKE_MAC_SDK = macosx10.11
    ICON = derfla.icns
    icon.path = $$PWD
    #icon.files += cjlv.png
    INSTALLS += icon

    CONFIG(release, debug|release) : {
    }
    QMAKE_INFO_PLIST = osxInfo.plist
    copy_skins.commands = 'cp -R \"$$PWD/skins\" \"$${TARGET}.app/Contents/Resources\"'
    QMAKE_EXTRA_TARGETS +=  copy_skins
    POST_TARGETDEPS += copy_skins
    QMAKE_POST_LINK += $$quote(cp -R \"$$PWD/skins\" \"$${TARGET}.app/Contents/Resources\")
}

win32: {
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
