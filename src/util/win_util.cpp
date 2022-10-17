#include "stdafx.h"

#include <QRegularExpression>

#include "win_util.h"
#include "directory.h"
#include "scopedguard.h"
#include "util.h"

namespace util
{

    static DWORD AbsoluteSeek(HANDLE hFile, DWORD offset)
    {
        DWORD newOffset;

        if ((newOffset = SetFilePointer(hFile, offset, nullptr, FILE_BEGIN)) == 0xFFFFFFFF)
        {
            qCritical() << "SetFilePointer failed, error " << GetLastError();
        }

        return newOffset;
    }

    static void ReadBytes(HANDLE hFile, LPVOID buffer, DWORD size)
    {
        DWORD bytes = 0;

        if (!ReadFile(hFile, buffer, size, &bytes, nullptr))
        {
            qCritical() << "ReadFile failed, error" << GetLastError();
        }
        else if (size != bytes)
        {
            qCritical() << "Read the wrong number of bytes, expected" << size << ", got" << bytes;
        }
    }

    QString readDescriptionFromResource(const QString &filePath)
    {
        DWORD verHandle = NULL;
        DWORD verSize   = GetFileVersionInfoSize(filePath.toStdWString().c_str(), &verHandle);

        ScopedGuard ch([verHandle]() { CloseHandle(reinterpret_cast<HANDLE>(verHandle)); });

        if (!verSize)
        {
            return {};
        }
        auto *verData = new char[verSize];
        ZeroMemory(verData, verSize);

        ScopedGuard deleteVerData([verData]() { delete[] verData; });
        if (!GetFileVersionInfo(filePath.toStdWString().c_str(), verHandle, verSize, verData))
        {
            return {};
        }

        struct LANGANDCODEPAGE
        {
            WORD wLanguage;
            WORD wCodePage;
        } *lpTranslate = nullptr;

        // Read the list of languages and code pages.
        UINT cbTranslate = 0;
        VerQueryValue(verData, L"\\VarFileInfo\\Translation", (LPVOID *)&lpTranslate, &cbTranslate);

        // Read the file description for each language and code page.
        const size_t subBlockSize           = 50;
        WCHAR        SubBlock[subBlockSize] = {0};
        UINT         dwBytes                = 0;
        for (size_t i = 0; i < (cbTranslate / sizeof(struct LANGANDCODEPAGE)); i++)
        {
            auto hRes = StringCchPrintf(
                SubBlock, subBlockSize, L"\\StringFileInfo\\%04x%04x\\FileDescription", lpTranslate[i].wLanguage, lpTranslate[i].wCodePage);
            if (SUCCEEDED(hRes))
            {
                LPBYTE lpBuffer = nullptr;
                // Retrieve file description for language and code page "i".
                VerQueryValue(verData, SubBlock, (LPVOID *)&lpBuffer, &dwBytes);

                return QString::fromUtf16((const ushort *)lpBuffer);
            }
        }
        return {};
    }

    void processFile(const Directory &directory, const QFileInfo &fileInfo, LFSInserter inserter)
    {
        QString filePath(directory.directory + QDir::separator() + fileInfo.fileName());
        filePath.replace("\\\\", "\\");
        if (fileInfo.suffix() == "lnk")
        {
            WCHAR        wszPath[MAX_PATH]             = {0};
            WCHAR        wszWorkingDirectory[MAX_PATH] = {0};
            WCHAR        wszDescription[1024 * 8]      = {0};
            const size_t argumentsLength               = 65535;
            WCHAR       *pwszArguments                 = new WCHAR[argumentsLength];
            ScopedGuard  sgReleaseArray([pwszArguments]() { delete[] pwszArguments; });
            auto hRes = resolveShellLink(nullptr, filePath.toStdWString().c_str(), wszPath, wszWorkingDirectory, wszDescription, pwszArguments);
            if (FAILED(hRes))
            {
                return;
            }
            QRegularExpression reg("%([^%]+)%");
            filePath  = QString::fromUtf16((const ushort *)wszPath);
            auto iter = reg.globalMatch(filePath);
            while (iter.hasNext())
            {
                auto match = iter.next();
                auto env   = match.captured(1);
                auto val   = qgetenv(env.toStdString().c_str());
                filePath.replace("%" % env % "%", val);
            }
            QFileInfo fileInfo(filePath);
            if (fileInfo.suffix() != "exe" && fileInfo.suffix() != "msc" && fileInfo.suffix() != "bat")
            {
                return;
            }
            auto args = QString::fromUtf16((const ushort *)pwszArguments);
            iter      = reg.globalMatch(args);
            while (iter.hasNext())
            {
                auto match = iter.next();
                auto env   = match.captured(1);
                auto val   = qgetenv(env.toStdString().c_str());
                args.replace("%" % env % "%", val);
            }
            auto workingDirectory = QString::fromUtf16((const ushort *)wszWorkingDirectory);
            iter                  = reg.globalMatch(args);
            while (iter.hasNext())
            {
                auto match = iter.next();
                auto env   = match.captured(1);
                auto val   = qgetenv(env.toStdString().c_str());
                workingDirectory.replace("%" % env % "%", val);
            }

            auto desc = QString::fromUtf16((const ushort *)wszDescription);
            if (desc.isEmpty())
            {
                desc = readDescriptionFromResource(filePath);
            }
            if (desc.isEmpty())
            {
                desc = filePath;
            }

            inserter(util::extractPNGIconFromFile(fileInfo),
                     fileInfo.baseName(),
                     desc,
                     filePath,
                     args,
                     workingDirectory,
                     timestamp,
                     fileInfo.lastModified().toMSecsSinceEpoch(),
                     isConsoleApplication(QDir::toNativeSeparators(fileInfo.absoluteFilePath())) ? QStringLiteral("terminalCommand")
                                                                                                 : QStringLiteral("shellExecute"));
            return;
        }

        QString desc = readDescriptionFromResource(filePath);
        if (desc.isEmpty())
        {
            desc = filePath;
        }
        inserter(util::extractPNGIconFromFile(fileInfo),
                 fileInfo.fileName(),
                 desc,
                 filePath,
                 {},
                 QDir::toNativeSeparators(QFileInfo(filePath).absolutePath()),
                 timestamp,
                 fileInfo.lastModified().toMSecsSinceEpoch(),
                 isConsoleApplication(QDir::toNativeSeparators(fileInfo.absoluteFilePath())) ? QStringLiteral("terminalCommand")
                                                                                             : QStringLiteral("shellExecute"));
    }

    HRESULT resolveShellLink(
        HWND hwnd, LPCWSTR lpszLinkFile, LPWSTR lpszPath, LPWSTR lpszWorkingDirectory, LPWSTR lpszDescription, LPWSTR lpszArguments)
    {
        IShellLink *psl = nullptr;

        *lpszPath = 0; // Assume failure

        // Get a pointer to the IShellLink interface. It is assumed that CoInitialize
        // has already been called.
        HRESULT hRes = CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID *)&psl);
        if (FAILED(hRes))
        {
            qCritical() << "CoCreateInstance failed";
            return hRes;
        }

        ScopedGuard releasePsl([psl]() { psl->Release(); });

        IPersistFile *ppf = nullptr;

        // Get a pointer to the IPersistFile interface.
        hRes = psl->QueryInterface(IID_IPersistFile, (void **)&ppf);

        if (FAILED(hRes))
        {
            qWarning() << "psl->QueryInterface(IID_IPersistFile, (void**)&ppf) failed";
            return hRes;
        }

        ScopedGuard releasePpf([ppf]() { ppf->Release(); });

        // Load the shortcut.
        hRes = ppf->Load(lpszLinkFile, STGM_READ);

        if (FAILED(hRes))
        {
            qWarning() << "ppf->Load(lpszLinkFile, STGM_READ) failed";
            return hRes;
        }
        // Resolve the link.
        hRes = psl->Resolve(hwnd, 0);

        if (FAILED(hRes))
        {
            qWarning() << "psl->Resolve(hwnd, 0) failed";
            return hRes;
        }

        // Get the path to the link target.
        WIN32_FIND_DATA wfd;
        WCHAR           szGotPath[MAX_PATH] = {0};
        hRes                                = psl->GetPath(szGotPath, MAX_PATH, &wfd, SLGP_RAWPATH);

        if (FAILED(hRes))
        {
            qWarning() << "psl->GetPath(szGotPath, MAX_PATH, (WIN32_FIND_DATA*)&wfd, SLGP_RAWPATH) failed";
            return hRes;
        }

        hRes = StringCbCopy(lpszPath, MAX_PATH, szGotPath);
        if (FAILED(hRes))
        {
            // Handle the error
            qWarning() << "failed StringCbCopy(lpszPath, iPathBufferSize, szGotPath)";
            return hRes;
        }

        // Get the description of the target.
        WCHAR szDescription[1024 * 8] = {0};
        hRes                          = psl->GetDescription(szDescription, 1024 * 8);

        if (FAILED(hRes))
        {
            qWarning() << "psl->GetDescription(szDescription, MAX_PATH) failed";
            return hRes;
        }

        hRes = StringCbCopy(lpszDescription, 1024 * 8, szDescription);
        if (FAILED(hRes))
        {
            // Handle the error
            qWarning() << "failed StringCbCopy(lpszDescription, MAX_PATH, szDescription)";
            return hRes;
        }
        // Get the working directory
        WCHAR szWorkingDirectory[MAX_PATH] = {0};
        hRes                               = psl->GetWorkingDirectory(szWorkingDirectory, MAX_PATH);
        if (FAILED(hRes))
        {
            qWarning() << "psl->GetWorkingDirectory(szWorkingDirectory, MAX_PATH) failed";
            return hRes;
        }

        hRes = StringCbCopy(lpszWorkingDirectory, MAX_PATH, szWorkingDirectory);
        if (FAILED(hRes))
        {
            // Handle the error
            qWarning() << "failed StringCbCopy(lpszWorkingDirectory, MAX_PATH, szWorkingDirectory)";
            return hRes;
        }

        // Get arguments
        const size_t argumentsLength = 65535;
        WCHAR       *pszArguments    = new WCHAR[argumentsLength];
        ScopedGuard  sgReleaseArray([pszArguments]() { delete[] pszArguments; });
        hRes = psl->GetArguments(pszArguments, argumentsLength);
        if (FAILED(hRes))
        {
            qWarning() << "psl->GetArguments(pszArguments, argumentsLength) failed";
            return hRes;
        }

        hRes = StringCbCopy(lpszArguments, argumentsLength, pszArguments);
        if (FAILED(hRes))
        {
            // Handle the error
            qWarning() << "failed StringCbCopy(lpszArguments, argumentsLength, pszArguments)";
            return hRes;
        }

        return hRes;
    }

#define XFER_BUFFER_SIZE 2048

#ifndef IMAGE_SIZEOF_NT_OPTIONAL_HEADER
#    define IMAGE_SIZEOF_NT_OPTIONAL_HEADER sizeof(IMAGE_OPTIONAL_HEADER)
#endif // IMAGE_SIZEOF_NT_OPTIONAL_HEADER

    bool isConsoleApplication(const QString &path)
    {
        DWORD SectionOffset    = 0;
        DWORD CoffHeaderOffset = 0;
        DWORD MoreDosHeader[16];

        ULONG ntSignature = 0;

        IMAGE_DOS_HEADER      image_dos_header;
        IMAGE_FILE_HEADER     image_file_header;
        IMAGE_OPTIONAL_HEADER image_optional_header;

        /*
         *  Open the reference file.
         */
        HANDLE hImage =
            CreateFile(path.toStdWString().c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

        if (INVALID_HANDLE_VALUE == hImage)
        {
            return false;
        }

        ScopedGuard sgCloseHandle([&hImage] { CloseHandle(hImage); });
        /*
         *  Read the MS-DOS image header.
         */
        ReadBytes(hImage, &image_dos_header, sizeof(IMAGE_DOS_HEADER));

        if (IMAGE_DOS_SIGNATURE != image_dos_header.e_magic)
        {
#if !defined(QT_NO_DEBUG)
            qWarning("Sorry, I do not understand this file.\n");
#endif
            return false;
        }

        /*
         *  Read more MS-DOS header.       */
        ReadBytes(hImage, MoreDosHeader, sizeof(MoreDosHeader));

        /*
         *  Get actual COFF header.
         */
        CoffHeaderOffset = AbsoluteSeek(hImage, image_dos_header.e_lfanew) + sizeof(ULONG);

        ReadBytes(hImage, &ntSignature, sizeof(ULONG));

        if (IMAGE_NT_SIGNATURE != ntSignature)
        {
#if !defined(QT_NO_DEBUG)
            qWarning("Missing NT signature. Unknown file type.\n");
#endif
            return false;
        }

        SectionOffset = CoffHeaderOffset + IMAGE_SIZEOF_FILE_HEADER + IMAGE_SIZEOF_NT_OPTIONAL_HEADER;

        ReadBytes(hImage, &image_file_header, IMAGE_SIZEOF_FILE_HEADER);

        /*
         *  Read optional header.
         */
        ReadBytes(hImage, &image_optional_header, IMAGE_SIZEOF_NT_OPTIONAL_HEADER);

        switch (image_optional_header.Subsystem)
        {
        case IMAGE_SUBSYSTEM_UNKNOWN:
            // printf("Type is unknown.\n");
            break;

        case IMAGE_SUBSYSTEM_NATIVE:
            // printf("Type is native.\n");
            break;

        case IMAGE_SUBSYSTEM_WINDOWS_GUI:
            // printf("Type is Windows GUI.\n");
            break;

        case IMAGE_SUBSYSTEM_WINDOWS_CUI:
            // printf("Type is Windows CUI.\n");
            return true;

        case IMAGE_SUBSYSTEM_OS2_CUI:
            // printf("Type is OS/2 CUI.\n");
            return true;

        case IMAGE_SUBSYSTEM_POSIX_CUI:
            // printf("Type is POSIX CUI.\n");
            return true;

        case IMAGE_SUBSYSTEM_NATIVE_WINDOWS:
            // printf("Type is native Win9x driver.\n");
            break;

        case IMAGE_SUBSYSTEM_WINDOWS_CE_GUI:
            // printf("Type is Windows CE.\n");
            break;

        default:
            // printf("Unknown type %u.\n", image_optional_header.Subsystem);
            break;
        }
        return false;
    }

} // namespace util
