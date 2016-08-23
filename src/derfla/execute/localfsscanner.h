#ifndef LOCALFSSCANNER_H
#define LOCALFSSCANNER_H

#include <QObject>

struct Directory
{
    Directory( const QString& d, bool r)
        : directory(d), recursive(r) 
    {}
    QString directory;
    bool recursive = false;
};

class LocalFSScanner : public QObject
{
    Q_OBJECT
public:
    explicit LocalFSScanner(QObject *parent = 0);
    ~LocalFSScanner();
    void start();
signals:
    void finished();
    void scanRequired();
private slots:
    void scan();
    void scheduleScan();
    void scanFinished();
private:
    qint64 timestamp_;
    QThread workerThread_;
    QList<Directory> scanDirectories_;
    void getDirectoriesFromEnvironmentVariable();
    void getBuiltinDirectories();
    void scanDirectory(const Directory& d);
};

#endif // LOCALFSSCANNER_H
