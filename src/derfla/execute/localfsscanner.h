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
public slots:
    void scan();
private:
    qint64 timestamp;
    QThread workerThread;
    QList<Directory> scanDirectories;
    void getDirectoriesFromEnvironmentVariable();
    void getBuiltinDirectories();
    void scanDirectory(const Directory& d);
#if defined(Q_OS_WIN)
    void readDescriptionFromResource(const QString& f, QString& desc);
    void processFilesOnWindows(const Directory& d, const QFileInfo& fileInfo);
    HRESULT resolveShellLink(HWND hwnd, LPCWSTR lpszLinkFile, LPWSTR lpszPath, LPWSTR lpszWorkingDirectory, LPWSTR lpszDescription, LPWSTR lpszArguments);
#endif
};

#endif // LOCALFSSCANNER_H
