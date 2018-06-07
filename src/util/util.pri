INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/util.h

SOURCES += \
    $$PWD/util.cpp

unix: !macx: {
    LIBS += -lz

    SOURCES += \
        $$PWD/unix_util.cpp

    HEADERS += \
        $$PWD/unix_util.h
}

win32: {
    QT += winextras
    SOURCES += \
        $$PWD/win_util.cpp \
        $$PWD/WinIconProvider.cpp


    HEADERS += \
        $$PWD/win_util.h \
        $$PWD/WinIconProvider.h

}
