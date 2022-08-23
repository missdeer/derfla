#ifndef __WINICONPROVIDER_H
#define __WINICONPROVIDER_H

#include <QFileIconProvider>

#include "util_global.h"

class UTILSHARED_EXPORT WinIconProvider : QFileIconProvider
{
public:
    WinIconProvider();
    ~WinIconProvider();

    virtual QIcon icon(const QFileInfo &info) const;
    void          setPreferredIconSize(int size);

private:
    bool addIconFromImageList(int imageListIndex, int iconIndex, QIcon &icon) const;
    bool addIconFromShellFactory(QString filePath, QIcon &icon) const;

    int preferredSize;
};

#endif
