#include "stdafx.h"
#include <boost/scope_exit.hpp>
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
#if defined(Q_OS_WIN)
    CoInitialize(NULL);
    BOOST_SCOPE_EXIT(void) {
        CoUninitialize();
    } BOOST_SCOPE_EXIT_END
#endif
    qDebug() << "LocalFSScanner::scan" << QThread::currentThreadId();
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
    QDir dir(d.directory);
    qDebug() << "scanning" << dir.absolutePath();
    
    QFileInfoList list = dir.entryInfoList(QStringList() << "*.exe" << "*.msc" << "*.lnk", QDir::Files);
    std::for_each(list.begin(), list.end(),
        std::bind(&LocalFSScanner::processFilesOnWindows, this, d, std::placeholders::_1));

    if (d.recursive)
    {
        QFileInfoList list = dir.entryInfoList(QStringList() << "*", QDir::NoDotAndDotDot | QDir::AllDirs);
        std::for_each(list.begin(), list.end(),
            [&](const QFileInfo& fileInfo) { scanDirectory(Directory(d.directory + QDir::separator() + fileInfo.fileName(), true)); });
    }
}

void LocalFSScanner::processFilesOnWindows(const Directory& d, const QFileInfo& fileInfo)
{
    QString f(d.directory + QDir::separator() + fileInfo.fileName());
    f.replace("\\\\", "\\");
    if (fileInfo.suffix() == "lnk")
    {
        WCHAR wszPath[MAX_PATH] = { 0 };
        WCHAR wszWorkingDirectory[MAX_PATH] = { 0 };
        WCHAR wszDescription[MAX_PATH] = { 0 };
        const size_t argumentsLength = 65535;
        WCHAR *pwszArguments = new WCHAR[argumentsLength];
        BOOST_SCOPE_EXIT(pwszArguments) {
            delete pwszArguments;
        } BOOST_SCOPE_EXIT_END
        HRESULT hr = resolveShellLink(NULL, f.toStdWString().c_str(), wszPath, wszWorkingDirectory, wszDescription, pwszArguments);
        if (FAILED(hr))
            return;
        f = QString::fromUtf16((const ushort *)wszPath);
        QRegExp r("%([^%]+)%");

        int pos = 0;
        if ((pos = r.indexIn(f, pos)) != -1)
        {
            QString e = r.cap(1);
            auto v = qgetenv(e.toStdString().c_str());
            f.replace("%" % e % "%", v);
        }
    }
    qDebug() << "find file:" << f;
}

HRESULT LocalFSScanner::resolveShellLink(HWND hwnd, LPCWSTR lpszLinkFile, LPWSTR lpszPath, LPWSTR lpszWorkingDirectory, LPWSTR lpszDescription, LPWSTR lpszArguments)
{
    IShellLink* psl = nullptr;

    *lpszPath = 0; // Assume failure 

    // Get a pointer to the IShellLink interface. It is assumed that CoInitialize
    // has already been called. 
    HRESULT hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl);
    if (FAILED(hres))
    {
        qDebug() << "CoCreateInstance failed";
        return hres;
    }

    BOOST_SCOPE_EXIT(psl) {
        // Release the pointer to the IShellLink interface. 
        psl->Release();
    } BOOST_SCOPE_EXIT_END

    IPersistFile* ppf;

    // Get a pointer to the IPersistFile interface. 
    hres = psl->QueryInterface(IID_IPersistFile, (void**)&ppf);

    if (FAILED(hres))
    {
        qDebug() << "psl->QueryInterface(IID_IPersistFile, (void**)&ppf) failed";
        return hres;
    }

    BOOST_SCOPE_EXIT(ppf) {
        // Release the pointer to the IPersistFile interface. 
        ppf->Release();
    } BOOST_SCOPE_EXIT_END

    // Load the shortcut. 
    hres = ppf->Load(lpszLinkFile, STGM_READ);

    if (FAILED(hres))
    {
        qDebug() << "ppf->Load(lpszLinkFile, STGM_READ) failed";
        return hres;
    }
    // Resolve the link. 
    hres = psl->Resolve(hwnd, 0);

    if (FAILED(hres))
    {
        qDebug() << "psl->Resolve(hwnd, 0) failed";
        return hres;
    }

    // Get the path to the link target. 
    WIN32_FIND_DATA wfd;
    WCHAR szGotPath[MAX_PATH] = { 0 };
    hres = psl->GetPath(szGotPath, MAX_PATH, &wfd, SLGP_RAWPATH);

    if (FAILED(hres))
    {
        qDebug() << "psl->GetPath(szGotPath, MAX_PATH, (WIN32_FIND_DATA*)&wfd, SLGP_RAWPATH) failed";
        return hres;
    }

    hres = StringCbCopy(lpszPath, MAX_PATH, szGotPath);
    if (FAILED(hres))
    {
        // Handle the error
        qDebug() << "failed StringCbCopy(lpszPath, iPathBufferSize, szGotPath)";
        return hres;
    }
    
    // Get the description of the target. 
    WCHAR szDescription[MAX_PATH] = { 0 };
    hres = psl->GetDescription(szDescription, MAX_PATH);

    if (FAILED(hres))
    {
        qDebug() << "psl->GetDescription(szDescription, MAX_PATH) failed";
        return hres;
    }

    hres = StringCbCopy(lpszDescription, MAX_PATH, szDescription);
    if (FAILED(hres))
    {
        // Handle the error
        qDebug() << "failed StringCbCopy(lpszDescription, MAX_PATH, szDescription)";
        return hres;
    }
    // Get the working directory 
    WCHAR szWorkingDirectory[MAX_PATH] = { 0 };
    hres = psl->GetWorkingDirectory(szWorkingDirectory, MAX_PATH);
    if (FAILED(hres))
    {
        qDebug() << "psl->GetWorkingDirectory(szWorkingDirectory, MAX_PATH) failed";
        return hres;
    }

    hres = StringCbCopy(lpszWorkingDirectory, MAX_PATH, szWorkingDirectory);
    if (FAILED(hres))
    {
        // Handle the error
        qDebug() << "failed StringCbCopy(lpszWorkingDirectory, MAX_PATH, szWorkingDirectory)";
        return hres;
    }

    // Get arguments
    const size_t argumentsLength = 65535;
    WCHAR *pszArguments = new WCHAR[argumentsLength];
    BOOST_SCOPE_EXIT(pszArguments) {
        delete pszArguments;
    } BOOST_SCOPE_EXIT_END
    hres = psl->GetArguments(pszArguments, argumentsLength);
    if (FAILED(hres))
    {
        qDebug() << "psl->GetArguments(pszArguments, argumentsLength) failed";
        return hres;
    }

    hres = StringCbCopy(lpszArguments, argumentsLength, pszArguments);
    if (FAILED(hres))
    {
        // Handle the error
        qDebug() << "failed StringCbCopy(lpszArguments, argumentsLength, pszArguments)";
        return hres;
    }

    return hres;
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
    qDebug() << "scanning" << dir.absolutePath();
}
#else
void LocalFSScanner::getBuiltinDirectories()
{
}
void LocalFSScanner::scanDirectory(const Directory &d)
{
    QDir dir(d.directory);
    qDebug() << "scanning" << dir.absolutePath();
}
#endif
