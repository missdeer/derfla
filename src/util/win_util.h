#ifndef WIN_UTIL_H
#define WIN_UTIL_H

#include "util_global.h"
struct Directory;

typedef std::function<bool(const QByteArray&, const QString&, const QString&, const QString&, const QString&, const QString&, qint64, qint64, const QString&)> LFSInserter;

namespace util
{
    UTILSHARED_EXPORT bool isConsoleApplication(const QString& path);
    UTILSHARED_EXPORT void readDescriptionFromResource(const QString& f, QString& desc);
    UTILSHARED_EXPORT void processFile(const Directory& d, const QFileInfo& fileInfo, LFSInserter inserter);
    UTILSHARED_EXPORT HRESULT resolveShellLink(HWND hwnd, LPCWSTR lpszLinkFile, LPWSTR lpszPath, LPWSTR lpszWorkingDirectory, LPWSTR lpszDescription, LPWSTR lpszArguments);
};

#endif // WIN_UTIL_H
