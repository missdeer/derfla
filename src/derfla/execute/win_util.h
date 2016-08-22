#ifndef WIN_UTIL_H
#define WIN_UTIL_H

struct Directory;

namespace win_util
{
extern qint64 timestamp;
void readDescriptionFromResource(const QString& f, QString& desc);
void processFile(const Directory& d, const QFileInfo& fileInfo);
HRESULT resolveShellLink(HWND hwnd, LPCWSTR lpszLinkFile, LPWSTR lpszPath, LPWSTR lpszWorkingDirectory, LPWSTR lpszDescription, LPWSTR lpszArguments);
};

#endif // WIN_UTIL_H
