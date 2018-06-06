#include "stdafx.h"
#include "dbrw.h"
#include "localfsscanner.h"
#include "util.h"
#include "win_util.h"

namespace util {

    static DWORD AbsoluteSeek(HANDLE hFile, DWORD  offset)
    {
        DWORD newOffset;

        if ((newOffset = SetFilePointer(hFile,
            offset,
            NULL,
            FILE_BEGIN)) == 0xFFFFFFFF)
        {
            qCritical() << "SetFilePointer failed, error " << GetLastError();
        }

        return newOffset;
    }

    static void  ReadBytes(HANDLE hFile, LPVOID buffer, DWORD  size)
    {
        DWORD bytes;

        if (!ReadFile(hFile,
            buffer,
            size,
            &bytes,
            NULL))
        {
            qCritical() << "ReadFile failed, error" << GetLastError();
        }
        else if (size != bytes)
        {
            qCritical() << "Read the wrong number of bytes, expected" << size << ", got" << bytes;
        }
    }

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

    void processFile(const Directory& d, const QFileInfo& fileInfo, LFSInserter inserter)
    {
        QString f(d.directory + QDir::separator() + fileInfo.fileName());
        f.replace("\\\\", "\\");
        if (fileInfo.suffix() == "lnk")
        {
            WCHAR wszPath[MAX_PATH] = { 0 };
            WCHAR wszWorkingDirectory[MAX_PATH] = { 0 };
            WCHAR wszDescription[1024 * 8] = { 0 };
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
            if (fi.suffix() != "exe" && fi.suffix() != "msc" && fi.suffix() != "bat")
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

            inserter(util::extractPNGIconFromFile(fi),
                     fileInfo.baseName(),
                     desc,
                     f,
                     a,
                     w,
                     timestamp,
                     fileInfo.lastModified().toMSecsSinceEpoch(),
                     isConsoleApplication(QDir::toNativeSeparators(fileInfo.absoluteFilePath())) ? "terminalCommand" : "shellExecute"
                                                                                                  );
            return;
        }

        QString desc;
        readDescriptionFromResource(f, desc);
        if (desc.isEmpty())
            desc = f;
        inserter(util::extractPNGIconFromFile(fileInfo),
                 fileInfo.fileName(),
                 desc,
                 f,
                 "",
                 QDir::toNativeSeparators(QFileInfo(f).absolutePath()),
                 timestamp,
                 fileInfo.lastModified().toMSecsSinceEpoch(),
                 isConsoleApplication(QDir::toNativeSeparators(fileInfo.absoluteFilePath())) ? "terminalCommand" : "shellExecute"
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
            qCritical() << "CoCreateInstance failed";
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
            qWarning() << "psl->QueryInterface(IID_IPersistFile, (void**)&ppf) failed";
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
            qWarning() << "ppf->Load(lpszLinkFile, STGM_READ) failed";
            return hres;
        }
        // Resolve the link.
        hres = psl->Resolve(hwnd, 0);

        if (FAILED(hres))
        {
            qWarning() << "psl->Resolve(hwnd, 0) failed";
            return hres;
        }

        // Get the path to the link target.
        WIN32_FIND_DATA wfd;
        WCHAR szGotPath[MAX_PATH] = { 0 };
        hres = psl->GetPath(szGotPath, MAX_PATH, &wfd, SLGP_RAWPATH);

        if (FAILED(hres))
        {
            qWarning() << "psl->GetPath(szGotPath, MAX_PATH, (WIN32_FIND_DATA*)&wfd, SLGP_RAWPATH) failed";
            return hres;
        }

        hres = StringCbCopy(lpszPath, MAX_PATH, szGotPath);
        if (FAILED(hres))
        {
            // Handle the error
            qWarning() << "failed StringCbCopy(lpszPath, iPathBufferSize, szGotPath)";
            return hres;
        }

        // Get the description of the target.
        WCHAR szDescription[1024 * 8] = { 0 };
        hres = psl->GetDescription(szDescription, 1024 * 8);

        if (FAILED(hres))
        {
            qWarning() << "psl->GetDescription(szDescription, MAX_PATH) failed";
            return hres;
        }

        hres = StringCbCopy(lpszDescription, 1024 * 8, szDescription);
        if (FAILED(hres))
        {
            // Handle the error
            qWarning() << "failed StringCbCopy(lpszDescription, MAX_PATH, szDescription)";
            return hres;
        }
        // Get the working directory
        WCHAR szWorkingDirectory[MAX_PATH] = { 0 };
        hres = psl->GetWorkingDirectory(szWorkingDirectory, MAX_PATH);
        if (FAILED(hres))
        {
            qWarning() << "psl->GetWorkingDirectory(szWorkingDirectory, MAX_PATH) failed";
            return hres;
        }

        hres = StringCbCopy(lpszWorkingDirectory, MAX_PATH, szWorkingDirectory);
        if (FAILED(hres))
        {
            // Handle the error
            qWarning() << "failed StringCbCopy(lpszWorkingDirectory, MAX_PATH, szWorkingDirectory)";
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
            qWarning() << "psl->GetArguments(pszArguments, argumentsLength) failed";
            return hres;
        }

        hres = StringCbCopy(lpszArguments, argumentsLength, pszArguments);
        if (FAILED(hres))
        {
            // Handle the error
            qWarning() << "failed StringCbCopy(lpszArguments, argumentsLength, pszArguments)";
            return hres;
        }

        return hres;
    }


#define XFER_BUFFER_SIZE 2048

#ifndef IMAGE_SIZEOF_NT_OPTIONAL_HEADER
#define  IMAGE_SIZEOF_NT_OPTIONAL_HEADER  sizeof(IMAGE_OPTIONAL_HEADER)
#endif // IMAGE_SIZEOF_NT_OPTIONAL_HEADER

    bool isConsoleApplication(const QString& path)
    {
        HANDLE hImage;

        //DWORD  bytes;
        //DWORD  iSection;
        DWORD  SectionOffset;
        DWORD  CoffHeaderOffset;
        DWORD  MoreDosHeader[16];

        ULONG  ntSignature;

        IMAGE_DOS_HEADER      image_dos_header;
        IMAGE_FILE_HEADER     image_file_header;
        IMAGE_OPTIONAL_HEADER image_optional_header;
        //IMAGE_SECTION_HEADER  image_section_header;
        
        /*
        *  Open the reference file.
        */
        hImage = CreateFile(path.toStdWString().c_str(),
            GENERIC_READ,
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

        if (INVALID_HANDLE_VALUE == hImage)
        {
            return false;
        }

        /*
        *  Read the MS-DOS image header.
        */
        ReadBytes(hImage,
            &image_dos_header,
            sizeof(IMAGE_DOS_HEADER));

        if (IMAGE_DOS_SIGNATURE != image_dos_header.e_magic)
        {
            qWarning("Sorry, I do not understand this file.\n");
            return false;
        }

        /*
        *  Read more MS-DOS header.       */
        ReadBytes(hImage,
            MoreDosHeader,
            sizeof(MoreDosHeader));

        /*
        *  Get actual COFF header.
        */
        CoffHeaderOffset = AbsoluteSeek(hImage, image_dos_header.e_lfanew) +
            sizeof(ULONG);

        ReadBytes(hImage, &ntSignature, sizeof(ULONG));

        if (IMAGE_NT_SIGNATURE != ntSignature)
        {
            qWarning("Missing NT signature. Unknown file type.\n");
            return false;
        }

        SectionOffset = CoffHeaderOffset + IMAGE_SIZEOF_FILE_HEADER + IMAGE_SIZEOF_NT_OPTIONAL_HEADER;

        ReadBytes(hImage,
            &image_file_header,
            IMAGE_SIZEOF_FILE_HEADER);

        /*
        *  Read optional header.
        */
        ReadBytes(hImage,
            &image_optional_header,
            IMAGE_SIZEOF_NT_OPTIONAL_HEADER);

        switch (image_optional_header.Subsystem)
        {
        case IMAGE_SUBSYSTEM_UNKNOWN:
            //printf("Type is unknown.\n");
            break;

        case IMAGE_SUBSYSTEM_NATIVE:
            //printf("Type is native.\n");
            break;

        case IMAGE_SUBSYSTEM_WINDOWS_GUI:
            //printf("Type is Windows GUI.\n");
            break;

        case IMAGE_SUBSYSTEM_WINDOWS_CUI:
            //printf("Type is Windows CUI.\n");
            return true;
            break;

        case IMAGE_SUBSYSTEM_OS2_CUI:
            //printf("Type is OS/2 CUI.\n");
            return true;
            break;

        case IMAGE_SUBSYSTEM_POSIX_CUI:
            //printf("Type is POSIX CUI.\n");
            return true;
            break;

        case IMAGE_SUBSYSTEM_NATIVE_WINDOWS:
            //printf("Type is native Win9x driver.\n");
            break;

        case IMAGE_SUBSYSTEM_WINDOWS_CE_GUI:
            //printf("Type is Windows CE.\n");
            break;

        default:
            //printf("Unknown type %u.\n", image_optional_header.Subsystem);
            break;
        }
        return false;
    }

}
