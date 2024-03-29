#include "stdafx.h"

#include "win_util.h"
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#    include <QtWin>
#endif
#include <CommonControls.h>
#include <shellapi.h>

#include <QImage>
#include <QPixmap>
#include <QRegularExpression>

#include "WinIconProvider.h"
#include "scopedguard.h"

// Temporary work around to avoid having to install the latest Windows SDK
#ifndef __IShellItemImageFactory_INTERFACE_DEFINED__
#    define __IShellItemImageFactory_INTERFACE_DEFINED__

#    define SHIL_JUMBO 0x4
/* IShellItemImageFactory::GetImage() flags */
enum _SIIGB
{
    SIIGBF_RESIZETOFIT   = 0x00000000,
    SIIGBF_BIGGERSIZEOK  = 0x00000001,
    SIIGBF_MEMORYONLY    = 0x00000002,
    SIIGBF_ICONONLY      = 0x00000004,
    SIIGBF_THUMBNAILONLY = 0x00000008,
    SIIGBF_INCACHEONLY   = 0x00000010
};
typedef int SIIGBF;

const GUID IID_IShellItemImageFactory = {0xbcc18b79, 0xba16, 0x442f, {0x80, 0xc4, 0x8a, 0x59, 0xc3, 0x0c, 0x46, 0x3b}};

class IShellItemImageFactory : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE GetImage(SIZE size, SIIGBF flags, /*__RPC__deref_out_opt*/ HBITMAP *phbm) = 0;
};

#endif

HRESULT(WINAPI *fnSHCreateItemFromParsingName)(PCWSTR, IBindCtx *, REFIID, void **) = nullptr;

WinIconProvider::WinIconProvider() : preferredSize(32)
{
    // Load Vista/7 specific API pointers
    HMODULE hLib = GetModuleHandleW(L"shell32");
    if (hLib)
    {
        (FARPROC &)fnSHCreateItemFromParsingName = GetProcAddress(hLib, "SHCreateItemFromParsingName");
    }
}

WinIconProvider::~WinIconProvider() = default;

void WinIconProvider::setPreferredIconSize(int size)
{
    preferredSize = size;
}

// This also exists in plugin_interface, need to remove both if I make a 64 build
QString wicon_aliasTo64(QString path)
{
    auto env  = QProcessEnvironment::systemEnvironment();
    auto pf32 = env.value("PROGRAMFILES");
    auto pf64 = env.value("PROGRAMW6432");

    // On 64 bit windows, 64 bit shortcuts don't resolve correctly from 32 bit executables, fix it here
    QFileInfo pInfo(path);

    if (env.contains("PROGRAMW6432") && pInfo.isSymLink() && pf32 != pf64)
    {
        if (QDir::toNativeSeparators(pInfo.symLinkTarget()).contains(pf32))
        {
            auto path64 = QDir::toNativeSeparators(pInfo.symLinkTarget());
            path64.replace(pf32, pf64);
            if (QFileInfo(path64).exists())
            {
                path = path64;
            }
        }
        else if (pInfo.symLinkTarget().contains("system32"))
        {
            auto path32 = QDir::toNativeSeparators(pInfo.symLinkTarget());
            if (!QFileInfo(path32).exists())
            {
                path = path32.replace("system32", "sysnative");
            }
        }
    }
    return path;
}

QIcon WinIconProvider::icon(const QFileInfo &info) const
{
    QString fileExtension = info.suffix().toLower();

    if (fileExtension == "png" || fileExtension == "bmp" || fileExtension == "jpg" || fileExtension == "jpeg")
    {
        return QIcon(info.filePath());
    }
    if (fileExtension == "cpl")
    {
        HICON   hIcon;
        QString filePath = QDir::toNativeSeparators(info.filePath());
        ExtractIconEx((const wchar_t *)filePath.utf16(), 0, &hIcon, nullptr, 1);
        ScopedGuard destroyIcon([hIcon] { DestroyIcon(hIcon); });
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        return QIcon(QPixmap::fromImage(QImage::fromHICON(hIcon)));
#else
        return QIcon(QtWin::fromHICON(hIcon));
#endif
    }

    // This 64 bit mapping needs to go away if we produce a 64 bit build of launchy
    QString filePath = wicon_aliasTo64(QDir::toNativeSeparators(info.filePath()));

    // Get the icon index using SHGetFileInfo
    SHFILEINFO sfi = {0};

    QRegularExpression reg(R"(\\\\([a-z0-9\-]+\\?)?$)", QRegularExpression::CaseInsensitiveOption);
    auto               matched = reg.match(filePath);
    if (matched.hasMatch())
    {
        // To avoid network hangs, explicitly fetch the My Computer icon for UNCs
        LPITEMIDLIST pidl = nullptr;
        if (SHGetSpecialFolderLocation(nullptr, CSIDL_DRIVES, &pidl) == S_OK)
        {
            SHGetFileInfo((LPCTSTR)pidl, 0, &sfi, sizeof(sfi), SHGFI_PIDL | SHGFI_SYSICONINDEX);
            // Set the file path to the My Computer GUID for any later fetches
            filePath = "::{20D04FE0-3AEA-1069-A2D8-08002B30309D}";
        }
    }
    if (sfi.iIcon == 0)
    {
        SHGetFileInfo((const wchar_t *)filePath.utf16(), 0, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX);
    }

    // An icon index of 3 is the generic file icon
    if (sfi.iIcon > 0 && sfi.iIcon != 3)
    {
        // Retrieve the system image list.
        // To get the 48x48 icons, use SHIL_EXTRALARGE
        // To get the 256x256 icons (Vista only), use SHIL_JUMBO
        int imageListIndex = SHIL_JUMBO;
        if (preferredSize <= 16)
        {
            imageListIndex = SHIL_SMALL;
        }
        else if (preferredSize <= 32)
        {
            imageListIndex = SHIL_LARGE;
        }
        else if (preferredSize <= 48)
        {
            imageListIndex = SHIL_EXTRALARGE;
        }

        QIcon retIcon;
        // If the OS supports SHCreateItemFromParsingName, get a 256x256 icon
        if (!addIconFromShellFactory(filePath, retIcon))
        {
            // otherwise get the largest appropriate size
            if (!addIconFromImageList(imageListIndex, sfi.iIcon, retIcon) && imageListIndex == SHIL_JUMBO)
            {
                addIconFromImageList(SHIL_EXTRALARGE, sfi.iIcon, retIcon);
            }
        }

        // Ensure there's also a 32x32 icon - extralarge and above often only contain
        // a large frame with the 32x32 icon in the middle or looks blurry
        if (imageListIndex == SHIL_EXTRALARGE || imageListIndex == SHIL_JUMBO)
        {
            addIconFromImageList(SHIL_LARGE, sfi.iIcon, retIcon);
        }
        return retIcon;
    }
    if (info.isSymLink() || fileExtension == "lnk") // isSymLink is case sensitive when it perhaps shouldn't be
    {
        QFileInfo targetInfo(info.symLinkTarget());
        return icon(targetInfo);
    }

    return QFileIconProvider::icon(info);
}

bool WinIconProvider::addIconFromImageList(int imageListIndex, int iconIndex, QIcon &icon) const
{
    IImageList *imageList = nullptr;
#if !defined(_MSC_VER)
    // For MinGW:
    static const IID iID_IImageList = {0x46eb5926, 0x582e, 0x4017, {0x9f, 0xdf, 0xe8, 0x99, 0x8d, 0xaa, 0x9, 0x50}};
    HRESULT          hResult        = SHGetImageList(imageListIndex, iID_IImageList, (void **)&imageList);
#else
    HRESULT hResult = SHGetImageList(imageListIndex, IID_IImageList, (void **)&imageList);
#endif
    if (hResult != S_OK)
    {
        return false;
    }
    ScopedGuard releaseImageList([imageList] { imageList->Release(); });
    HICON       hIcon = nullptr;
    hResult           = imageList->GetIcon(iconIndex, ILD_TRANSPARENT, &hIcon);
    if (hResult != S_OK || !hIcon)
    {
        return false;
    }
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    icon.addPixmap(QPixmap::fromImage(QImage::fromHICON(hIcon)));
#else
    icon.addPixmap(QtWin::fromHICON(hIcon));
#endif
    DestroyIcon(hIcon);

    return true;
}

// On Vista or 7 we could use SHIL_JUMBO to get a 256x256 icon,
// but we'll use SHCreateItemFromParsingName as it'll give an identical
// icon to the one shown in explorer and it scales automatically.
bool WinIconProvider::addIconFromShellFactory(const QString &filePath, QIcon &icon) const
{
    if (!fnSHCreateItemFromParsingName)
    {
        return false;
    }

    IShellItem *psi  = nullptr;
    HRESULT     hRes = fnSHCreateItemFromParsingName((const wchar_t *)filePath.utf16(), nullptr, IID_IShellItem, (void **)&psi);
    if (hRes != S_OK)
    {
        return false;
    }
    ScopedGuard             releasePsi([psi] { psi->Release(); });
    IShellItemImageFactory *psiif = nullptr;
    hRes                          = psi->QueryInterface(IID_IShellItemImageFactory, (void **)&psiif);
    if (hRes != S_OK)
    {
        return false;
    }
    ScopedGuard releasePsiif([psiif] { psiif->Release(); });
    HBITMAP     iconBitmap = nullptr;
    SIZE        iconSize   = {preferredSize, preferredSize};
    hRes                   = psiif->GetImage(iconSize, SIIGBF_RESIZETOFIT | SIIGBF_ICONONLY, &iconBitmap);
    if (hRes != S_OK)
    {
        return false;
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QPixmap iconPixmap = QPixmap::fromImage(QImage::fromHBITMAP(iconBitmap));
#else
    QPixmap iconPixmap = QtWin::fromHBITMAP(iconBitmap, QtWin::HBitmapPremultipliedAlpha);
#endif
    icon.addPixmap(iconPixmap);
    ::DeleteObject(iconBitmap);

    return true;
}
