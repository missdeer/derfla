INCLUDEPATH	+= $$PWD
DEPENDPATH      += $$PWD
HEADERS		+= $$PWD/qtsingleapplication.h $$PWD/qtlocalpeer.h

QT *= network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

gotqtlockedfile = $$find(HEADERS, .*qtlockedfile.h)
isEmpty(gotqtlockedfile):include($$PWD/../qtlockedfile/qtlockedfile.pri)

contains(TEMPLATE, lib): {
    SOURCES += $$PWD/qtsingleapplication.cpp $$PWD/qtlocalpeer.cpp
    win32:contains(CONFIG, shared) {
        DEFINES += QT_QTSINGLEAPPLICATION_EXPORT=1
    }
}
