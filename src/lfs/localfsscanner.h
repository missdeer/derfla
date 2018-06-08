#ifndef LOCALFSSCANNER_H
#define LOCALFSSCANNER_H

#include <QObject>
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
private:
    DBRW& dbrw_;
    bool stop_ = false;
    qint64 timestamp_ = 0;
    QList<Directory> scanDirectories_;
    void getDirectoriesFromEnvironmentVariable();
    void getBuiltinDirectories();
    void scanDirectory(const Directory& d);
};

#endif // LOCALFSSCANNER_H
