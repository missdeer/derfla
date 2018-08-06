
INCLUDEPATH += $$PWD

unix:!macx: {
	QT = x11extras
    LIBS += -lX11 -lxcb
    SOURCES  += $$PWD/qglobalshortcut_x11.cc
}

HEADERS += \
    $$PWD/qglobalshortcut.h

SOURCES += \
    $$PWD/qglobalshortcut.cc

win32: {
    SOURCES += $$PWD/qglobalshortcut_win.cc
    *-g++* {
        LIBS += -luser32
    }
    *-msvc* {
        LIBS += user32.lib
    }
}
