#include "stdafx.h"
#include "dbrw.h"
#include "util.h"
#if defined(Q_OS_WIN)
#include "win_util.h"
#endif
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
    timestamp = QDateTime::currentDateTime().toMSecsSinceEpoch();
#if defined(Q_OS_WIN)
    win_util::timestamp = timestamp;
    CoInitialize(NULL);
    BOOST_SCOPE_EXIT(void) {
        CoUninitialize();
    } BOOST_SCOPE_EXIT_END
#endif

    scanDirectories.clear();

    getBuiltinDirectories();
    getDirectoriesFromEnvironmentVariable();
    std::sort(scanDirectories.begin(), scanDirectories.end(),
        [&](const Directory& d1, const Directory& d2) { return d1.directory < d2.directory; });
    auto it = std::unique(scanDirectories.begin(), scanDirectories.end(), 
        [&](const Directory& d1, const Directory& d2) { return QDir(d1.directory) == QDir(d2.directory); });
    scanDirectories.erase(it, scanDirectories.end());

    std::for_each(scanDirectories.begin(), scanDirectories.end(),
                  std::bind(&LocalFSScanner::scanDirectory, this, std::placeholders::_1));
    DBRW::instance()->removeOldRecords(timestamp);
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
    std::for_each(paths.begin(), paths.end(), [&](const QString& p) { scanDirectories.append(Directory( p, false)); });
}

#if defined(Q_OS_WIN)
void LocalFSScanner::getBuiltinDirectories()
{
    std::vector<std::pair<REFKNOWNFOLDERID, bool>> dirs{
        { FOLDERID_AdminTools , false},
        { FOLDERID_ApplicationShortcuts , false },
        { FOLDERID_AppsFolder , false },
        { FOLDERID_CommonAdminTools , false },
        { FOLDERID_CommonPrograms , false },
        { FOLDERID_CommonStartMenu , true },
        { FOLDERID_ControlPanelFolder , true },
        { FOLDERID_Desktop , false },
        { FOLDERID_ImplicitAppShortcuts , false },
        { FOLDERID_Programs , false },
        { FOLDERID_PublicDesktop , false },
        { FOLDERID_QuickLaunch , false },
    };
    
    for (auto p : dirs)
    {
        LPWSTR wszPath = NULL;
        HRESULT hr = SHGetKnownFolderPath(p.first, 0, NULL, &wszPath);
        if (SUCCEEDED(hr))
        {
            scanDirectories << Directory(QString::fromUtf16((const ushort *)wszPath), p.second);
            CoTaskMemFree(wszPath);
        }
    }
}

void LocalFSScanner::scanDirectory(const Directory &d)
{    
    using namespace win_util;
    QDir dir(d.directory);
    
    QFileInfoList list = dir.entryInfoList(QStringList() << "*.exe" << "*.msc" << "*.lnk", QDir::Files);
    std::for_each(list.begin(), list.end(),
        std::bind(processFile, d, std::placeholders::_1));

    if (d.recursive)
    {
        QFileInfoList list = dir.entryInfoList(QStringList() << "*", QDir::NoDotAndDotDot | QDir::AllDirs);
        std::for_each(list.begin(), list.end(),
            [&](const QFileInfo& fileInfo) { scanDirectory(Directory(d.directory + QDir::separator() + fileInfo.fileName(), true)); });
    }
}

#elif defined(Q_OS_MAC)
void LocalFSScanner::getBuiltinDirectories()
{
    auto homePath = qgetenv("HOME");
    scanDirectories << Directory("/Applications", false)
                    << Directory(homePath + "/Applications", false)
                    << Directory(homePath, false)
                    << Directory("/System/Library/CoreServices", false)
                    << Directory("/System/Library/CoreServices/Applications", false);
}

void LocalFSScanner::scanDirectory(const Directory &d)
{
    QDir dir(d.directory);

    QFileInfoList list = dir.entryInfoList(QStringList() << "*.app", QDir::AllDirs | QDir::NoDotAndDotDot);
    list << dir.entryInfoList(QStringList() << "*", QDir::Files | QDir::Readable);

    DBRW* dbrw = DBRW::instance();
    std::for_each(list.begin(), list.end(),
                  [&](const QFileInfo& fileInfo) {
        if ((fileInfo.isFile() && fileInfo.permission(QFile::ExeGroup))
                || (fileInfo.isDir() && fileInfo.suffix() == "app"))
        {
            QString f(d.directory + QDir::separator() + fileInfo.fileName());
            dbrw->insertLFS(util::extractXPMFromFile(fileInfo),
                            fileInfo.fileName(),
                            f,
                            f,
                            "",
                            QFileInfo(f).filePath(),
                            timestamp,
                            fileInfo.lastModified().toMSecsSinceEpoch(),
                            fileInfo.isDir() ? "g" : "c"
                            );
        }
    });
}
#else
void LocalFSScanner::getBuiltinDirectories()
{
    auto homePath = qgetenv("HOME");
    scanDirectories << Directory("/usr/share/applications/",false)
                    << Directory("/usr/local/share/applications/", false)
                    << Directory("/usr/share/gdm/applications/", false)
                    << Directory("/usr/share/applications/kde/", false)
                    << Directory(homePath + "/.local/share/applications/", false);
}
void LocalFSScanner::scanDirectory(const Directory &d)
{
    QDir dir(d.directory);
    qDebug() << "scanning" << dir.absolutePath();

    QFileInfoList list = dir.entryInfoList(QStringList() << "*", QDir::Files | QDir::Readable);

    std::for_each(list.begin(), list.end(),
                  [&](const QFileInfo& fileInfo) {
        if (fileInfo.permission(QFile::ExeGroup) && fileInfo.isFile())
        {
            QString f(d.directory + QDir::separator() + fileInfo.fileName());
            qDebug() << "find" <<  f;
        }
    });

    list = dir.entryInfoList(QStringList() << "*.desktop", QDir::Files | QDir::Readable);
    std::for_each(list.begin(), list.end(),
                  [&](const QFileInfo& fileInfo) {
            QString f(d.directory + QDir::separator() + fileInfo.fileName());
            qDebug() << "find" <<  f;
    });
}
#endif
