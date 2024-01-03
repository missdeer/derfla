
#include <Windows.h>

#include <array>
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
    IShellLink *psl = nullptr;

    std::array<wchar_t, MAX_PATH> szGotPath {};
    std::array<wchar_t, MAX_PATH> szDescription {};
    WIN32_FIND_DATA               wfd;

    *lpszPath = 0; // Assume failure

    CoInitialize(nullptr);
    // Get a pointer to the IShellLink interface. It is assumed that CoInitialize
    // has already been called.
    HRESULT hres = CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID *)&psl);
    if (SUCCEEDED(hres))
    {
        IPersistFile *ppf = nullptr;

        // Get a pointer to the IPersistFile interface.
        hres = psl->QueryInterface(IID_IPersistFile, (void **)&ppf);

        if (SUCCEEDED(hres))
        {
            std::array<wchar_t, MAX_PATH> wsz {};

            // Ensure that the string is Unicode.
            MultiByteToWideChar(CP_ACP, 0, lpszLinkFile, -1, wsz.data(), MAX_PATH);
            // Add code here to check return value from MultiByteWideChar
            // for success.

            // Load the shortcut.
            hres = ppf->Load(wsz.data(), STGM_READ);

            if (SUCCEEDED(hres))
            {
                // Resolve the link.
                hres = psl->Resolve(hwnd, 0);

                if (SUCCEEDED(hres))
                {
                    // Get the path to the link target.
                    hres = psl->GetPath(szGotPath.data(), MAX_PATH, &wfd, SLGP_RAWPATH);

                    if (SUCCEEDED(hres))
                    {
                        // Get the description of the target.
                        hres = psl->GetDescription(szDescription.data(), MAX_PATH);

                        if (SUCCEEDED(hres))
                        {
                            wcscpy_s(lpszPath, iPathBufferSize, static_cast<LPCWSTR>(szGotPath.data()));
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
        auto ret = SendMessage(hWnd, EVERYTHING_WM_IPC, EVERYTHING_IPC_IS_DESKTOP_SHORTCUT, 0);
        if (!ret)
        {
            // create one
            SendMessage(hWnd, EVERYTHING_WM_IPC, EVERYTHING_IPC_CREATE_DESKTOP_SHORTCUT, 0);
        }

        QDir    deskDir(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
        QString path = QDir::toNativeSeparators(deskDir.absoluteFilePath("Search Everything.lnk"));

        std::array<wchar_t, MAX_PATH> szTarget {};
        HRESULT                       hResult = ResolveIt(hWnd, path.toStdString().c_str(), szTarget.data(), sizeof(szTarget) / sizeof(szTarget[0]));
        if (!ret)
        {
            // remove the one created right now
            SendMessage(hWnd, EVERYTHING_WM_IPC, EVERYTHING_IPC_DELETE_DESKTOP_SHORTCUT, 0);
        }
        if (hResult == S_OK)
        {
            QFileInfo fileInfo(QString::fromWCharArray(static_cast<LPCWSTR>(szTarget.data())));
            return fileInfo.absoluteFilePath();
        }
    }

    return {};
}

bool QuickGetFilesByFileName(bool regexpEnabled, const QString &pattern, QStringList &results, const std::function<bool(bool)> &pfChecker, int count)
{
    if (!isEverythingRunning())
    {
        if (QMessageBox::question(nullptr,
                                  QObject::tr("Notice"),
                                  QObject::tr("Everything is not running, do you want to launch the Everything application?")) == QMessageBox::Yes)
        {
            QString everythingFilePath = QCoreApplication::applicationDirPath() % "/Everything.exe";
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
        std::array<wchar_t, MAX_PATH> path {};
        Everything_GetResultFullPathName(i, path.data(), MAX_PATH);
        QString filePath(QString::fromStdWString(path.data()));
        bool    isDir = !!Everything_IsFolderResult(i);
        if (pfChecker(isDir))
        {
            results.append(filePath);
        }
    }
    Everything_Reset();

    return true;
}
