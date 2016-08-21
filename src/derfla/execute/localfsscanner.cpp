#include "stdafx.h"
#include "localfsscanner.h"

LocalFSScanner::LocalFSScanner(QObject *parent) : QObject(parent)
{
    connect(this, &LocalFSScanner::finished, [this] {QTimer::singleShot(10 *60* 1000, this, &LocalFSScanner::scan);});

    workerThread.start(QThread::IdlePriority);
}

LocalFSScanner::~LocalFSScanner()
{
    if (workerThread.isRunning())
    {
        workerThread.quit();
        workerThread.wait();
    }
}

void LocalFSScanner::start()
{
    this->moveToThread(&workerThread);
    QTimer::singleShot(1000, this, &LocalFSScanner::scan);
}

void LocalFSScanner::scan()
{
    qDebug() << "LocalFSScanner::scan" << QThread::currentThreadId();
    scanDirectories.clear();
    getBuiltinDirectories();
    getDirectoriesFromEnvironmentVariable();

    std::for_each(scanDirectories.begin(), scanDirectories.end(),
                  std::bind(&LocalFSScanner::scanDirectory, this, std::placeholders::_1));
    emit finished();
}

void LocalFSScanner::getDirectoriesFromEnvironmentVariable()
{
    QString path = qgetenv("PATH");
    QStringList environment = QProcess::systemEnvironment();
    auto it = std::find_if(environment.begin(), environment.end(),
                           [&](const QString& env) { return env.startsWith("PATH="); });
    if (environment.end() != it)
         path = it->mid(5);

#if defined(Q_OS_WIN)
    QStringList&& paths = path.split(';');
#else
    QStringList&& paths = path.split(':');
#endif
    std::for_each(paths.begin(), paths.end(), [&](const QString& p) { scanDirectories.append(p);});
}

#if defined(Q_OS_WIN)
void LocalFSScanner::getBuiltinDirectories()
{
}
void LocalFSScanner::scanDirectory(const QString &d)
{
    QDir dir(d);
    qDebug() << "scanning" << dir.absolutePath();
}
#elif defined(Q_OS_MAC)
void LocalFSScanner::getBuiltinDirectories()
{
    auto homePath = qgetenv("HOME");
    scanDirectories << "/Applications"
                    << homePath + "/Applications"
                    << "/System/Library/CoreServices"
                    << "/System/Library/CoreServices/Applications";
}

void LocalFSScanner::scanDirectory(const QString &d)
{
    QDir dir(d);
    qDebug() << "scanning" << dir.absolutePath();
}
#else
void LocalFSScanner::getBuiltinDirectories()
{
}
void LocalFSScanner::scanDirectory(const QString &d)
{
    QDir dir(d);
    qDebug() << "scanning" << dir.absolutePath();
}
#endif
