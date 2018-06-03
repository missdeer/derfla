#ifndef WIN_UTIL_H
#define WIN_UTIL_H

struct Directory;

typedef std::function<bool(const QByteArray&, const QString&, const QString&, const QString&, const QString&, const QString&, qint64, qint64, const QString&)> LFSInserter;

namespace util
{
    bool isConsoleApplication(const QString& path);
    void readDescriptionFromResource(const QString& f, QString& desc);
    void processFile(const Directory& d, const QFileInfo& fileInfo, LFSInserter inserter);
    HRESULT resolveShellLink(HWND hwnd, LPCWSTR lpszLinkFile, LPWSTR lpszPath, LPWSTR lpszWorkingDirectory, LPWSTR lpszDescription, LPWSTR lpszArguments);
};

#endif // WIN_UTIL_H
