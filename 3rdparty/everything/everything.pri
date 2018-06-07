INCLUDEPATH += $$PWD/sdk/include $$PWD/sdk/ipc $$PWD

SOURCES += $$PWD/everythingwrapper.cpp
HEADERS += $$PWD/everythingwrapper.h

LIBS+=-L$$PWD/sdk/lib

contains(QMAKE_HOST.arch, x86_64): LIBS += -lEverything64
else: LIBS += -lEverything32
