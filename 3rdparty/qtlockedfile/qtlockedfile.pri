INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
HEADERS += $$PWD/qtlockedfile.h

contains(TEMPLATE, lib): {
    SOURCES += $$PWD/qtlockedfile.cpp

    unix:SOURCES += $$PWD/qtlockedfile_unix.cpp
    win32:SOURCES += $$PWD/qtlockedfile_win.cpp
    win32:contains(CONFIG, shared) {
        DEFINES += QT_QTLOCKEDFILE_EXPORT=__declspec(dllexport)
    }
}


