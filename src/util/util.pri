INCLUDEPATH += $$PWD $$PWD/sqlite $$PWD/../../3rdparty/lua/ $$PWD/../../3rdparty/lua/src/
DEPENDPATH += $$PWD

macx: {
    LIBS+=-L$$PWD/../../bin/x86_64/Derfla.app/Contents/Libs
} else {
    contains(QMAKE_HOST.arch, x86_64): {
        LIBS+=-L$$PWD/../../bin/x86_64
    } else: {
        LIBS+=-L$$PWD/../../bin/x86
    }
}

LIBS+=-lDerflaUtil

HEADERS += \
    $$PWD/util_global.h \
    $$PWD/stdafx.h \
    $$PWD/directory.h \
    $$PWD/util.h \
    $$PWD/derflaaction.h \
    $$PWD/scopedguard.h

win32: {
    HEADERS += \
        $$PWD/win_util.h \
        $$PWD/WinIconProvider.h
}

unix: !macx: {
    HEADERS += \
        $$PWD/unix_util.h
}
