

INCLUDEPATH += $$PWD

win32 | macx : {
    SOURCES += $$PWD/sqlite3.c
} else: {
    LIBS += -lsqlite3
}
