#ifndef LOCALFSSCANNER_H
#define LOCALFSSCANNER_H

#include <QObject>

class LocalFSScanner : public QObject
{
    Q_OBJECT
public:
    explicit LocalFSScanner(QObject *parent = 0);
    ~LocalFSScanner();
    void start();
signals:
    void finished();
public slots:
    void scan();
private:
    QThread workerThread;
    QStringList scanDirectories;
    void getDirectoriesFromEnvironmentVariable();
    void getBuiltinDirectories();
    void scanDirectory(const QString& d);
};

#endif // LOCALFSSCANNER_H
