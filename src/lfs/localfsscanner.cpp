#include "stdafx.h"
#include <functional>
#include "dbrw.h"
#include "util.h"
#if defined(Q_OS_WIN)
#include "win_util.h"
#elif defined(Q_OS_MAC)
#else
#include "unix_util.h"
#endif
#include "localfsscanner.h"

using namespace std;

#define check_stop do { if (stop_) return;   QThread::yieldCurrentThread(); } while(0)

LocalFSScanner::LocalFSScanner(DBRW &dbrw, QObject *parent)
    : dbrw_(dbrw)
    , QObject(parent)
    , stop_(false)
{
    connect(this, &LocalFSScanner::scanRequired, &LocalFSScanner::scan);
}

LocalFSScanner::~LocalFSScanner()
{
}

void LocalFSScanner::start()
{
    stop_ = false;
    //qDebug() << "scan local file system now" << QThread::currentThreadId();
    QTimer::singleShot(3 * 1000, this, &LocalFSScanner::scanRequired);
}

void LocalFSScanner::stop()
{
    stop_ = true;
}

void LocalFSScanner::scan()
{
    timestamp_ = QDateTime::currentDateTime().toMSecsSinceEpoch();
    util::timestamp = timestamp_;
#if defined(Q_OS_WIN)
    if (qApp->thread() != QThread::currentThread())
        CoInitialize(NULL);
#endif

    scanDirectories_.clear();

    getBuiltinDirectories();
    getDirectoriesFromEnvironmentVariable();
    std::sort(scanDirectories_.begin(), scanDirectories_.end(),
        [&](const Directory& d1, const Directory& d2) { return d1.directory < d2.directory; });
    auto it = std::unique(scanDirectories_.begin(), scanDirectories_.end(),
        [&](const Directory& d1, const Directory& d2) { return QDir(d1.directory) == QDir(d2.directory); });
    scanDirectories_.erase(it, scanDirectories_.end());

    for(const auto & d :  scanDirectories_)
        scanDirectory(d);
    dbrw_.removeOldRecords(timestamp_);
    emit finished();

#if defined(Q_OS_WIN)
    if (qApp->thread() != QThread::currentThread())
        CoUninitialize();
#endif
}

void LocalFSScanner::getDirectoriesFromEnvironmentVariable()
{
    QStringList& paths = util::getEnvPaths();
    std::for_each(paths.begin(), paths.end(), [&](const QString& p) { scanDirectories_.append(Directory( p, false)); });
}

#if defined(Q_OS_WIN)
void LocalFSScanner::getBuiltinDirectories()
{
    std::vector<std::pair<REFKNOWNFOLDERID, bool>> dirs{
        { FOLDERID_ApplicationShortcuts , false },
        { FOLDERID_AppsFolder , false },
        { FOLDERID_StartMenu , true },
        { FOLDERID_CommonStartMenu , true },
        { FOLDERID_ControlPanelFolder , true },
        { FOLDERID_Desktop , false },
        { FOLDERID_PublicDesktop , false },
        { FOLDERID_QuickLaunch , false },
        { FOLDERID_UserPinned , true },
        { FOLDERID_History, false},
        { FOLDERID_Recent, false},
    };
    
    for (auto p : dirs)
    {
        LPWSTR wszPath = NULL;
        HRESULT hr = SHGetKnownFolderPath(p.first, 0, NULL, &wszPath);
        if (SUCCEEDED(hr))
        {
            scanDirectories_ << Directory(QString::fromUtf16((const ushort *)wszPath), p.second);
            CoTaskMemFree(wszPath);
        }
    }
}

void LocalFSScanner::scanDirectory(const Directory &d)
{
    check_stop;
    QDir dir(d.directory);
    
    QFileInfoList list = dir.entryInfoList(QStringList() << "*.exe" << "*.msc" << "*.bat" << "*.lnk", QDir::Files);
    check_stop;

    auto inserter = std::bind(&DBRW::insertLFS, &dbrw_,
                              std::placeholders::_1,
                              std::placeholders::_2,
                              std::placeholders::_3,
                              std::placeholders::_4,
                              std::placeholders::_5,
                              std::placeholders::_6,
                              std::placeholders::_7,
                              std::placeholders::_8,
                              std::placeholders::_9);
    for(const auto & fi : list)
        util::processFile(d, fi, inserter);

    check_stop;
    if (d.recursive)
    {
        QFileInfoList list = dir.entryInfoList(QStringList() << "*", QDir::NoDotAndDotDot | QDir::AllDirs);
        for (const auto & fileInfo : list)
        {
            check_stop;
            scanDirectory(Directory(d.directory + QDir::separator() + fileInfo.fileName(), true));
        }
    }
}

#elif defined(Q_OS_MAC)
void LocalFSScanner::getBuiltinDirectories()
{
    auto homePath = qgetenv("HOME");
    scanDirectories_ << Directory("/Applications", true)
                    << Directory(homePath + "/Applications", true)
                    << Directory("/Applications/Xcode.app/Contents/Applications", false)
                    << Directory("/Developer/Applications", false)
                    << Directory(homePath, false)
                    << Directory(homePath + "/Library/PreferencePanes", false)
                    << Directory("/usr/local/Cellar", true)
                    << Directory("/opt/homebrew-cask/Caskroom", false)
                    << Directory("/System/Library/PerferencePanes", false)
                    << Directory("/System/Library/CoreServices/Applications", true);
}

void LocalFSScanner::scanDirectory(const Directory &d)
{
    check_stop;
    QThread::yieldCurrentThread();

    if (QFileInfo(d.directory).suffix() == "framework")
        return;

    QDir dir(d.directory);
    QFileInfoList list = dir.entryInfoList(QStringList() << "*", QDir::Files | QDir::Readable);
    check_stop;
    if (d.recursive)
        list << dir.entryInfoList(QStringList() , QDir::AllDirs | QDir::NoDotAndDotDot);
    else
        list << dir.entryInfoList(QStringList() << "*.app" << "*.prefPane", QDir::AllDirs | QDir::NoDotAndDotDot);

    check_stop;
    for (const auto & fileInfo : list)
    {
        check_stop;
        QString f(d.directory + QDir::separator() + fileInfo.fileName());
        if ((fileInfo.isFile() && fileInfo.permission(QFile::ExeGroup))
                || (fileInfo.isDir() && (fileInfo.suffix() == "app" || fileInfo.suffix() == "prefPane")))
        {
            dbrw_.insertLFS(util::extractPNGIconFromFile(fileInfo),
                            fileInfo.fileName(),
                            f,
                            f,
                            "",
                            QFileInfo(f).filePath(),
                            timestamp_,
                            fileInfo.lastModified().toMSecsSinceEpoch(),
                            fileInfo.isDir() ? "g" : "c"
                            );
        }
        else if (fileInfo.isDir() && d.recursive)
        {
            check_stop;
            scanDirectory(Directory {f, true });
        }
    });
}
#else
void LocalFSScanner::getBuiltinDirectories()
{
    auto homePath = qgetenv("HOME");
    scanDirectories_ << Directory("/usr/share/applications/",false)
                    << Directory("/usr/local/share/applications/", false)
                    << Directory("/usr/share/gdm/applications/", false)
                    << Directory("/usr/share/applications/kde/", false)
                    << Directory(homePath + "/.local/share/applications/", false);
}
void LocalFSScanner::scanDirectory(const Directory &d)
{
    check_stop;
    QDir dir(d.directory);

    QFileInfoList list = dir.entryInfoList(QStringList(), QDir::Files | QDir::Readable);

    check_stop;
    for (const auto & fileInfo : list)
    {
        check_stop;
        if (fileInfo.permission(QFile::ExeGroup) && fileInfo.isFile() && fileInfo.suffix() != "desktop")
        {
            QString f(d.directory + QDir::separator() + fileInfo.fileName());
            qWarning() << "find" <<  f;
            dbrw_.insertLFS(util::extractPNGIconFromFile(fileInfo),
                            fileInfo.fileName(),
                            f,
                            f,
                            "",
                            QFileInfo(f).absolutePath(),
                            timestamp_,
                            fileInfo.lastModified().toMSecsSinceEpoch(),
                            "c"
                            );
        }
    });

    check_stop;
    list = dir.entryInfoList(QStringList() << "*.desktop", QDir::Files | QDir::Readable);
    auto inserter = std::bind(&DBRW::insertLFS, &dbrw_,
                              std::placeholders::_1,
                              std::placeholders::_2,
                              std::placeholders::_3,
                              std::placeholders::_4,
                              std::placeholders::_5,
                              std::placeholders::_6,
                              std::placeholders::_7,
                              std::placeholders::_8,
                              std::placeholders::_9);
    for(const auto & fi : list)
        util::processFile(d, fi, inserter);

    if (d.recursive)
    {
        check_stop;
        list = dir.entryInfoList(QStringList() , QDir::AllDirs | QDir::NoDotAndDotDot);
        for (const auto & fileInfo : list)
        {
            check_stop;
            QString f(d.directory + QDir::separator() + fileInfo.fileName());
            f.replace("//", "/");
            scanDirectory(Directory {f, true });
        });
    }
}
#endif