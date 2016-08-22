#include "stdafx.h"
#include "dbrw.h"
#include "localfsscanner.h"
#include "WinIconProvider.h"
#include "win_util.h"

namespace win_util {

    qint64 timestamp = 0;

    void readDescriptionFromResource(const QString& f, QString& desc)
    {
        DWORD  verHandle = NULL;
        DWORD  verSize = GetFileVersionInfoSize(f.toStdWString().c_str(), &verHandle);

        if (!verSize)
        {
            return;
        }
        LPSTR verData = new char[verSize];
        BOOST_SCOPE_EXIT(verData) {
            delete verData;
        } BOOST_SCOPE_EXIT_END

            if (!GetFileVersionInfo(f.toStdWString().c_str(), verHandle, verSize, verData))
            {
                return;
            }
        HRESULT hr;

        struct LANGANDCODEPAGE {
            WORD wLanguage;
            WORD wCodePage;
        } *lpTranslate;

        // Read the list of languages and code pages.
        UINT cbTranslate = 0;
        VerQueryValue(verData,
            L"\\VarFileInfo\\Translation",
            (LPVOID*)&lpTranslate,
            &cbTranslate);

        // Read the file description for each language and code page.
        const size_t subBlockSize = 50;
        WCHAR SubBlock[subBlockSize] = { 0 };
        UINT dwBytes = 0;
        for (int i = 0; i < (cbTranslate / sizeof(struct LANGANDCODEPAGE)); i++)
        {
            hr = StringCchPrintf(SubBlock, subBlockSize,
                L"\\StringFileInfo\\%04x%04x\\FileDescription",
                lpTranslate[i].wLanguage,
                lpTranslate[i].wCodePage);
            if (FAILED(hr))
            {
                // TODO: write error handler.
            }

            LPBYTE lpBuffer = NULL;
            // Retrieve file description for language and code page "i".
            VerQueryValue(verData,
                SubBlock,
                (LPVOID *)&lpBuffer,
                &dwBytes);

            desc = QString::fromUtf16((const ushort*)lpBuffer);
            break;
        }
    }

    void processFile(const Directory& d, const QFileInfo& fileInfo)
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
            QRegExp r("%([^%]+)%");
            f = QString::fromUtf16((const ushort *)wszPath);
            int pos = 0;
            while ((pos = r.indexIn(f, pos)) != -1)
            {
                QString e = r.cap(1);
                auto v = qgetenv(e.toStdString().c_str());
                f.replace("%" % e % "%", v);
            }
            QFileInfo fi(f);
            if (fi.suffix() != "exe" && fi.suffix() != "msc")
                return;
            QString a = QString::fromUtf16((const ushort*)pwszArguments);
            pos = 0;
            while ((pos = r.indexIn(a, pos)) != -1)
            {
                QString e = r.cap(1);
                auto v = qgetenv(e.toStdString().c_str());
                a.replace("%" % e % "%", v);
            }
            QString w = QString::fromUtf16((const ushort*)wszWorkingDirectory);
            pos = 0;
            while ((pos = r.indexIn(w, pos)) != -1)
            {
                QString e = r.cap(1);
                auto v = qgetenv(e.toStdString().c_str());
                w.replace("%" % e % "%", v);
            }

            QString desc = QString::fromUtf16((const ushort*)wszDescription);
            if (desc.isEmpty())
                readDescriptionFromResource(f, desc);
            if (desc.isEmpty())
                desc = f;

            WinIconProvider iconProvider;
            QIcon i = iconProvider.icon(fi);
            QPixmap pixmap = i.pixmap(32, 32);
            QByteArray bytes;
            QBuffer buf(&bytes);
            buf.open(QIODevice::WriteOnly);
            pixmap.save(&buf, "XPM");

            DBRW::instance()->insertLFS(bytes,
                fileInfo.baseName(),
                desc,
                f,
                a,
                w,
                timestamp,
                fileInfo.lastModified().toMSecsSinceEpoch(),
                fileInfo.isDir() ? "g" : "c"
            );
            return;
        }

        WinIconProvider iconProvider;
        QIcon i = iconProvider.icon(fileInfo);
        QPixmap pixmap = i.pixmap(32, 32);
        QByteArray bytes;
        QBuffer buf(&bytes);
        buf.open(QIODevice::WriteOnly);
        pixmap.save(&buf, "XPM");

        QString desc;
        readDescriptionFromResource(f, desc);
        if (desc.isEmpty())
            desc = f;
        DBRW::instance()->insertLFS(bytes,
            fileInfo.fileName(),
            desc,
            f,
            "",
            QDir::toNativeSeparators(QFileInfo(f).absolutePath()),
            timestamp,
            fileInfo.lastModified().toMSecsSinceEpoch(),
            fileInfo.isDir() ? "g" : "c"
        );
    }

    HRESULT resolveShellLink(HWND hwnd, LPCWSTR lpszLinkFile, LPWSTR lpszPath, LPWSTR lpszWorkingDirectory, LPWSTR lpszDescription, LPWSTR lpszArguments)
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
}
