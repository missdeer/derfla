#include "stdafx.h"
#include "util.h"

#if defined(Q_OS_WIN)
#include "WinIconProvider.h"
typedef WinIconProvider IconProvider;
#else
#include <QFileIconProvider>
typedef QFileIconProvider IconProvider;
#endif

namespace util {

QByteArray extractPNGIconFromFile(const QFileInfo &fi)
{
    IconProvider iconProvider;
    QIcon i = iconProvider.icon(fi);
    QPixmap pixmap = i.pixmap(i.actualSize(QSize(32, 32)));
    QByteArray bytes;
    QBuffer buf(&bytes);
    buf.open(QIODevice::WriteOnly);
    pixmap.save(&buf, "PNG");

    return bytes;
}


QByteArray extractPNGFromIcon(const QString &filePath)
{
    QPixmap pixmap;
    pixmap.load(filePath);
    QByteArray bytes;
    QBuffer buf(&bytes);
    buf.open(QIODevice::WriteOnly);
    pixmap.save(&buf, "PNG");

    return bytes;
}

}
