

INCLUDEPATH += $$PWD

win32-msvc* : {
    SOURCES += $$PWD/sqlite3.c
} else: {
    LIBS += -lsqlite3
}
