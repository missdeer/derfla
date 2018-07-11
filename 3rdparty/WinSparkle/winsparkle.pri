INCLUDEPATH += $$PWD/include

contains(QMAKE_HOST.arch, x86_64): LIBS += -L$$PWD/x64/Release
else: LIBS += -L$$PWD/Release
LIBS += -lWinSparkle
