#include "stdafx.h"

#include <functional>

#include "dbrw.h"
#include "util.h"
#if defined(Q_OS_WIN)
#    include "win_util.h"
#elif defined(Q_OS_MAC)
#    include "plistparser.h"
#else
#    include "unix_util.h"
#endif
#include "directory.h"
#include "localfsscanner.h"

#define check_stop                         \
    do                                     \
    {                                      \
        if (stop_)                         \
        {                                  \
            return;                        \
        }                                  \
        QThread::yieldCurrentThread();     \
        QCoreApplication::processEvents(); \
    } while (0)

const int scanInterval = 3600;

LocalFSScanner::LocalFSScanner(DBRW &dbrw, QObject *parent) : QObject(parent), dbrw_(dbrw), stop_(false)
{
    connect(this, &LocalFSScanner::scanRequired, &LocalFSScanner::scan);
}

LocalFSScanner::~LocalFSScanner() {}

void LocalFSScanner::start()
{
    stop_ = false;
    // qDebug() << "scan local file system now" << QThread::currentThreadId();
    QTimer::singleShot(3 * 1000, this, &LocalFSScanner::scanRequired);
}

void LocalFSScanner::stop()
{
    stop_ = true;
}

void LocalFSScanner::scan()
{
#if defined(Q_OS_MAC)
    scanDockIcons();
#endif
    scanDirectories_.clear();

    getBuiltinDirectories();
    getDirectoriesFromEnvironmentVariable();
    std::sort(
        scanDirectories_.begin(), scanDirectories_.end(), [&](const Directory &d1, const Directory &d2) { return d1.directory < d2.directory; });
    auto iter = std::unique(scanDirectories_.begin(), scanDirectories_.end(), [&](const Directory &d1, const Directory &d2) {
        return QDir(d1.directory) == QDir(d2.directory);
    });
    scanDirectories_.erase(iter, scanDirectories_.end());

    for (const auto &directory : scanDirectories_)
    {
        scanDirectory(directory);
    }
    // dbrw_.removeOldRecords(timestamp_);
    dbrw_.removeInvalidRecords();
    emit finished();
    QTimer::singleShot(scanInterval, this, &LocalFSScanner::scanRequired);
}

void LocalFSScanner::getDirectoriesFromEnvironmentVariable()
{
    QStringList &paths = util::getEnvPaths();
    std::for_each(paths.begin(), paths.end(), [this](const QString &dirPath) { scanDirectories_.append(Directory(dirPath, false)); });
}

#if defined(Q_OS_WIN)
void LocalFSScanner::getBuiltinDirectories()
{
    std::vector<std::pair<REFKNOWNFOLDERID, bool>> dirs {
        {FOLDERID_ApplicationShortcuts, false},
        {FOLDERID_AppsFolder, false},
        {FOLDERID_StartMenu, true},
        {FOLDERID_CommonStartMenu, true},
        {FOLDERID_ControlPanelFolder, true},
        {FOLDERID_Desktop, false},
        {FOLDERID_PublicDesktop, false},
        {FOLDERID_QuickLaunch, false},
        {FOLDERID_UserPinned, true},
        {FOLDERID_History, false},
        {FOLDERID_Recent, false},
    };

    for (auto dirPath : dirs)
    {
        LPWSTR  wszPath = nullptr;
        HRESULT hRes    = SHGetKnownFolderPath(dirPath.first, 0, nullptr, &wszPath);
        if (SUCCEEDED(hRes))
        {
            scanDirectories_ << Directory(QString::fromUtf16(reinterpret_cast<const ushort *>(wszPath)), dirPath.second);
            CoTaskMemFree(wszPath);
        }
    }
}

void LocalFSScanner::scanDirectory(const Directory &directory)
{
    check_stop;
    QDir dir(directory.directory);

    QFileInfoList list = dir.entryInfoList(
        QStringList() << QStringLiteral("*.exe") << QStringLiteral("*.msc") << QStringLiteral("*.bat") << QStringLiteral("*.lnk"), QDir::Files);
    check_stop;

    auto inserter = std::bind(&DBRW::insertLFS,
                              &dbrw_,
                              std::placeholders::_1,
                              std::placeholders::_2,
                              std::placeholders::_3,
                              std::placeholders::_4,
                              std::placeholders::_5,
                              std::placeholders::_6,
                              std::placeholders::_7);
    for (const auto &fileInfo : list)
    {
        check_stop;
        util::processFile(directory, fileInfo, inserter);
    }

    check_stop;
    if (directory.recursive)
    {
        QFileInfoList list = dir.entryInfoList(QStringList() << QStringLiteral("*"), QDir::NoDotAndDotDot | QDir::AllDirs);
        for (const auto &fileInfo : list)
        {
            check_stop;
            scanDirectory(Directory(directory.directory + QDir::separator() + fileInfo.fileName(), true));
        }
    }
}

#elif defined(Q_OS_MAC)
void LocalFSScanner::getBuiltinDirectories()
{
    auto homePath = qgetenv("HOME");
    scanDirectories_ << Directory(QStringLiteral("/Applications"), true) << Directory(homePath + "/Applications", true)
                     << Directory(QStringLiteral("/Applications/Xcode.app/Contents/Applications"), false)
                     << Directory(QStringLiteral("/Developer/Applications"), false) << Directory(homePath, false)
                     << Directory(homePath + "/Library/PreferencePanes", false) << Directory(QStringLiteral("/usr/local/Cellar"), true)
                     << Directory(QStringLiteral("/opt/homebrew-cask/Caskroom"), false)
                     << Directory(QStringLiteral("/System/Library/PerferencePanes"), false)
                     << Directory(QStringLiteral("/System/Library/CoreServices/Applications"), true);
}

void LocalFSScanner::scanDirectory(const Directory &d)
{
    check_stop;
    QThread::yieldCurrentThread();

    if (QFileInfo(d.directory).suffix() == "framework")
    {
        return;
    }

    QDir          dir(d.directory);
    QFileInfoList list = dir.entryInfoList(QStringList() << QStringLiteral("*"), QDir::Files | QDir::Readable);
    check_stop;
    if (d.recursive)
    {
        list << dir.entryInfoList(QStringList(), QDir::AllDirs | QDir::NoDotAndDotDot);
    }
    else
    {
        list << dir.entryInfoList(QStringList() << QStringLiteral("*.app") << QStringLiteral("*.prefPane"), QDir::AllDirs | QDir::NoDotAndDotDot);
    }

    check_stop;
    for (const auto &fileInfo : list)
    {
        check_stop;
        QString f(d.directory + QDir::separator() + fileInfo.fileName());
        if ((fileInfo.isFile() && fileInfo.permission(QFile::ExeGroup)) ||
            (fileInfo.isDir() && (fileInfo.suffix() == "app" || fileInfo.suffix() == "prefPane")))
        {
            dbrw_.insertLFS(util::extractPNGIconFromFile(fileInfo),
                            fileInfo.fileName(),
                            f,
                            f,
                            {},
                            QFileInfo(f).filePath(),
                            fileInfo.isDir() ? QStringLiteral("shellExecute") : QStringLiteral("terminalCommand"));
        }
        else if (fileInfo.isDir() && d.recursive)
        {
            check_stop;
            scanDirectory(Directory {f, true});
        }
    }
}

void LocalFSScanner::scanDockIcons()
{
    auto  homePath = qgetenv("HOME");
    auto *p        = new QProcess;
    connect(p, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finished(int, QProcess::ExitStatus)));
    p->setProgram("/usr/bin/plutil");
    p->setArguments(QStringList() << "-convert"
                                  << "xml1" << homePath + "/Library/Preferences/com.apple.dock.plist"
                                  << "-o"
                                  << "-");
    p->start();
}

void LocalFSScanner::finished(int exitCode, QProcess::ExitStatus)
{
    auto *p = qobject_cast<QProcess *>(sender());

    p->deleteLater();

    if (exitCode != 0)
        return;

    QByteArray output = p->readAllStandardOutput();

    QBuffer     buf(&output);
    QVariantMap rootMap = PListParser::parsePList(&buf).toMap();

    auto it = rootMap.find("persistent-apps");
    if (rootMap.end() != it)
    {
        auto papps = it.value();
        if (papps.canConvert<QVariantList>())
        {
            auto it = papps.value<QSequentialIterable>();
            for (const QVariant &v : it)
            {
                auto item       = v.toMap();
                auto tileDataIt = item.find("tile-data");
                if (item.end() == tileDataIt)
                    continue;

                auto tileData = tileDataIt.value().toMap();

                auto fileLabelIt = tileData.find("file-label");
                if (tileData.end() == fileLabelIt)
                    continue;
                QString fileLabel = fileLabelIt.value().toString();

                auto fileDataIt = tileData.find("file-data");
                if (tileData.end() == fileDataIt)
                    continue;

                auto fileData = fileDataIt.value().toMap();
                auto cfurlIt  = fileData.find("_CFURLString");
                if (fileData.end() == cfurlIt)
                    continue;
                QString cfurl = QByteArray::fromPercentEncoding(cfurlIt.value().toByteArray());

                if (cfurl.startsWith("file://") && cfurl.endsWith(".app/"))
                {
                    cfurl = cfurl.right(cfurl.length() - 7);
                    cfurl = cfurl.left(cfurl.length() - 1);

                    QFileInfo fileInfo(cfurl);
                    dbrw_.insertLFS(
                        util::extractPNGIconFromFile(fileInfo), fileLabel, fileLabel, cfurl, {}, fileInfo.filePath(), QStringLiteral("shellExecute"));
                }
            }
        }
    }
}
#else
void LocalFSScanner::getBuiltinDirectories()
{
    auto homePath = qgetenv("HOME");
    scanDirectories_ << Directory("/usr/share/applications/", false) << Directory("/usr/local/share/applications/", false)
                     << Directory("/usr/share/gdm/applications/", false) << Directory("/usr/share/applications/kde/", false)
                     << Directory(homePath + "/.local/share/applications/", false);
}
void LocalFSScanner::scanDirectory(const Directory &d)
{
    check_stop;
    QDir dir(d.directory);

    QFileInfoList list = dir.entryInfoList(QStringList(), QDir::Files | QDir::Readable);

    check_stop;
    for (const auto &fileInfo : list)
    {
        check_stop;
        if (fileInfo.permission(QFile::ExeGroup) && fileInfo.isFile() && fileInfo.suffix() != "desktop")
        {
            QString f(d.directory + QDir::separator() + fileInfo.fileName());
            qWarning() << "find" << f;
            dbrw_.insertLFS(util::extractPNGIconFromFile(fileInfo),
                            fileInfo.fileName(),
                            f,
                            f,
                            "",
                            QFileInfo(f).absolutePath(),
                            timestamp_,
                            fileInfo.lastModified().toMSecsSinceEpoch(),
                            "terminalCommand");
        }
    }

    check_stop;
    list          = dir.entryInfoList(QStringList() << "*.desktop", QDir::Files | QDir::Readable);
    auto inserter = std::bind(&DBRW::insertLFS,
                              &dbrw_,
                              std::placeholders::_1,
                              std::placeholders::_2,
                              std::placeholders::_3,
                              std::placeholders::_4,
                              std::placeholders::_5,
                              std::placeholders::_6,
                              std::placeholders::_7);
    for (const auto &fi : list)
        util::processFile(d, fi, inserter);

    if (d.recursive)
    {
        check_stop;
        list = dir.entryInfoList(QStringList(), QDir::AllDirs | QDir::NoDotAndDotDot);
        for (const auto &fileInfo : list)
        {
            check_stop;
            QString f(d.directory + QDir::separator() + fileInfo.fileName());
            f.replace("//", "/");
            scanDirectory(Directory {f, true});
        }
    }
}
#endif
