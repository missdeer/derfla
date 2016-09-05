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

QStringList envPaths;

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

QStringList &getEnvPaths()
{
    if (envPaths.isEmpty())
    {
        QString path = qgetenv("PATH");
        QStringList environment = QProcess::systemEnvironment();
        auto it = std::find_if(environment.begin(), environment.end(),
                               [&](const QString& env) { return env.startsWith("PATH="); });
        if (environment.end() != it)
            path = it->mid(5);
#if defined(Q_OS_WIN)
        envPaths << path.split(QChar(';'));
#else
        envPaths << path.split(QChar(':'));
#endif
    }
    return envPaths;
}

}
