#ifndef __WINICONPROVIDER_H
#define __WINICONPROVIDER_H

#include <QFileIconProvider>

#include "util_global.h"

class UTILSHARED_EXPORT WinIconProvider : public QFileIconProvider
{
public:
    WinIconProvider();
    ~WinIconProvider() override;

    [[nodiscard]] QIcon icon(const QFileInfo &info) const override;
    void                setPreferredIconSize(int size);

private:
    bool addIconFromImageList(int imageListIndex, int iconIndex, QIcon &icon) const;
    bool addIconFromShellFactory(const QString &filePath, QIcon &icon) const;

    int preferredSize;
};

#endif
