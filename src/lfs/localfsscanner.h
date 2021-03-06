#ifndef LOCALFSSCANNER_H
#define LOCALFSSCANNER_H

#include <QObject>
#include <QProcess>
#include "dbrw.h"

struct Directory;

class LocalFSScanner : public QObject
{
    Q_OBJECT
public:
    explicit LocalFSScanner(DBRW& dbrw, QObject *parent = 0);
    ~LocalFSScanner();
signals:
    void finished();
    void scanRequired();
public slots:
    void start();
    void stop();
private slots:
    void scan();
#if defined(Q_OS_MAC)
    void finished(int exitCode, QProcess::ExitStatus);
#endif
private:
    DBRW& dbrw_;
    bool stop_ = false;
    qint64 timestamp_ = 0;
    QList<Directory> scanDirectories_;
    void getDirectoriesFromEnvironmentVariable();
    void getBuiltinDirectories();
    void scanDirectory(const Directory& d);
#if defined(Q_OS_MAC)
    void scanDockIcons();
#endif
};

#endif // LOCALFSSCANNER_H
