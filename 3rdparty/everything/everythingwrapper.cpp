
#include <Windows.h>

#include <cstring>
#include <Shellapi.h>
#include <Shlobj.h>
#include <Winnt.h>
#include <shlguid.h>
#include <shobjidl.h>

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QStandardPaths>
#include <QStringBuilder>

#include "everythingwrapper.h"
#include "Everything.h"
#include "everything_ipc.h"

HRESULT ResolveIt(HWND hwnd, LPCSTR lpszLinkFile, LPWSTR lpszPath, int iPathBufferSize)
{
    HRESULT         hres;
    IShellLink     *psl;
    WCHAR           szGotPath[MAX_PATH];
    WCHAR           szDescription[MAX_PATH];
    WIN32_FIND_DATA wfd;

    *lpszPath = 0; // Assume failure

    CoInitialize(NULL);
    // Get a pointer to the IShellLink interface. It is assumed that CoInitialize
    // has already been called.
    hres = CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID *)&psl);
    if (SUCCEEDED(hres))
    {
        IPersistFile *ppf;

        // Get a pointer to the IPersistFile interface.
        hres = psl->QueryInterface(IID_IPersistFile, (void **)&ppf);

        if (SUCCEEDED(hres))
        {
            WCHAR wsz[MAX_PATH] = {0};

            // Ensure that the string is Unicode.
            MultiByteToWideChar(CP_ACP, 0, lpszLinkFile, -1, wsz, MAX_PATH);
            // Add code here to check return value from MultiByteWideChar
            // for success.

            // Load the shortcut.
            hres = ppf->Load(wsz, STGM_READ);

            if (SUCCEEDED(hres))
            {
                // Resolve the link.
                hres = psl->Resolve(hwnd, 0);

                if (SUCCEEDED(hres))
                {
                    // Get the path to the link target.
                    hres = psl->GetPath(szGotPath, MAX_PATH, (WIN32_FIND_DATA *)&wfd, SLGP_RAWPATH);

                    if (SUCCEEDED(hres))
                    {
                        // Get the description of the target.
                        hres = psl->GetDescription(szDescription, MAX_PATH);

                        if (SUCCEEDED(hres))
                        {
                            wcscpy_s(lpszPath, iPathBufferSize, szGotPath);
                        }
                    }
                }
            }

            // Release the pointer to the IPersistFile interface.
            ppf->Release();
        }

        // Release the pointer to the IShellLink interface.
        psl->Release();
    }
    return hres;
}

bool isEverythingRunning()
{
    HWND everything_hwnd = FindWindow(EVERYTHING_IPC_WNDCLASS, nullptr);
    return everything_hwnd != nullptr;
}

void launchEverything(const QString &everythingFilePath)
{
    if (QFile::exists(everythingFilePath))
    {
        QString path = QDir::toNativeSeparators(everythingFilePath);
        ::ShellExecuteW(nullptr, L"open", path.toStdWString().c_str(), nullptr, nullptr, SW_SHOWMINIMIZED);
    }
}

QString GetEverythingPath()
{
    HWND hWnd = FindWindow(EVERYTHING_IPC_WNDCLASS, nullptr);
    if (hWnd)
    {
        int ret = (int)SendMessage(hWnd, EVERYTHING_WM_IPC, EVERYTHING_IPC_IS_DESKTOP_SHORTCUT, 0);
        if (!ret)
        {
            // create one
            SendMessage(hWnd, EVERYTHING_WM_IPC, EVERYTHING_IPC_CREATE_DESKTOP_SHORTCUT, 0);
        }

        QString path = QDir::toNativeSeparators(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) % "/Search Everything.lnk");

        WCHAR   szTarget[MAX_PATH] = {0};
        HRESULT hr                 = ResolveIt(hWnd, path.toStdString().c_str(), szTarget, sizeof(szTarget) / sizeof(szTarget[0]));
        if (!ret)
        {
            // remove the one created right now
            SendMessage(hWnd, EVERYTHING_WM_IPC, EVERYTHING_IPC_DELETE_DESKTOP_SHORTCUT, 0);
        }
        if (hr == S_OK)
        {
            QFileInfo f(QString::fromWCharArray(szTarget));
            return f.absoluteFilePath();
        }
    }

    return QString();
}

bool QuickGetFilesByFileName(bool regexpEnabled, const QString &pattern, QStringList &results, std::function<bool(bool)> checker, const int count)
{
    HWND everything_hwnd = FindWindow(EVERYTHING_IPC_WNDCLASS, nullptr);
    if (!everything_hwnd)
    {
        if (QMessageBox::question(nullptr,
                                  QObject::tr("Notice"),
                                  QObject::tr("Everything is not running, do you want to launch the Everything application?")) == QMessageBox::Yes)
        {
            QString everythingFilePath = QApplication::applicationDirPath() % "/Everything.exe";
            if (!QFile::exists(everythingFilePath))
            {
                QMessageBox::warning(nullptr,
                                     QObject::tr("Warning"),
                                     QObject::tr("Can't find the Everything executable, please re-install everything extension."),
                                     QMessageBox::Ok);
                return false;
            }
            ::ShellExecuteW(nullptr, L"open", everythingFilePath.toStdWString().c_str(), nullptr, nullptr, SW_SHOWMINIMIZED);
        }
        return false;
    }
    // find
    Everything_SetRegex(regexpEnabled);
    Everything_SetSearch(pattern.toStdWString().c_str());
    Everything_Query(TRUE);

    for (DWORD i = 0; i < Everything_GetNumResults() && results.size() < count; i++)
    {
        WCHAR path[MAX_PATH] = {0};
        Everything_GetResultFullPathName(i, path, MAX_PATH);
        QString f(QString::fromStdWString(path));
        BOOL    isDir = Everything_IsFolderResult(i);
        if (checker(isDir))
            results.append(f);
    }
    Everything_Reset();

    return true;
}
