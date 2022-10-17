#ifndef WIN_UTIL_H
#define WIN_UTIL_H

#include "util_global.h"
struct Directory;

using LFSInserter = std::function<bool(
    const QByteArray &, const QString &, const QString &, const QString &, const QString &, const QString &, qint64, qint64, const QString &)>;

namespace util
{
    UTILSHARED_EXPORT bool    isConsoleApplication(const QString &path);
    UTILSHARED_EXPORT QString readDescriptionFromResource(const QString &filePath);
    UTILSHARED_EXPORT void    processFile(const Directory &directory, const QFileInfo &fileInfo, LFSInserter inserter);
    UTILSHARED_EXPORT HRESULT
    resolveShellLink(HWND hwnd, LPCWSTR lpszLinkFile, LPWSTR lpszPath, LPWSTR lpszWorkingDirectory, LPWSTR lpszDescription, LPWSTR lpszArguments);
}; // namespace util

#endif // WIN_UTIL_H
