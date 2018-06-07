INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

LIBS += -L$$PWD/../../bin -lderflautil

HEADERS += \
    $$PWD/util_global.h \
    $$PWD/stdafx.h \
    $$PWD/directory.h \
    $$PWD/util.h

win32: {
    HEADERS += \
        $$PWD/win_util.h \
        $$PWD/WinIconProvider.h
}

unix: !macx: {
    HEADERS += \
        $$PWD/unix_util.h
}
