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

static QStringList envPaths;
static QStringList localeNames;

QByteArray extractPNGIconFromFile(const QFileInfo &fi)
{
    QByteArray bytes;
#if defined(Q_OS_WIN) || defined(Q_OS_MAC)
    IconProvider iconProvider;
    QIcon i = iconProvider.icon(fi);
    QPixmap pixmap = i.pixmap(i.actualSize(QSize(32, 32)));
    if (!pixmap.isNull())
    {
        QBuffer buf(&bytes);
        buf.open(QIODevice::WriteOnly);
        pixmap.save(&buf, "PNG");
        buf.close();
    }
#else
    Q_UNUSED(fi);
#endif

    return bytes;
}


QByteArray extractPNGFromIcon(const QString &filePath)
{
    QByteArray bytes;
    QPixmap pixmap;
    if (pixmap.load(filePath))
    {
        QBuffer buf(&bytes);
        buf.open(QIODevice::WriteOnly);
        pixmap.save(&buf, "PNG");
        buf.close();
    }
    else
    {
        qWarning() << "loading icon file failed" << filePath;
    }

    return bytes;
}

QStringList& getLocaleNames()
{
    if (localeNames.isEmpty())
    {
        QStringList uiLanguages = QLocale::system().uiLanguages();
        for (const QString& locale: uiLanguages)
        {
            localeNames << QLocale(locale).name();
        }
    }
    return localeNames;
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

void setTimestamp(qint64 ts)
{
    timestamp = ts;
}

QString findProgram(const QString &exe)
{
    QStringList envPaths;

    QString     path        = qgetenv("PATH");
    QStringList environment = QProcess::systemEnvironment();
    auto        it          = std::find_if(environment.begin(), environment.end(), [&](const QString &env) { return env.startsWith("PATH="); });
    if (environment.end() != it)
        path = it->mid(5);
#if defined(Q_OS_WIN)
    envPaths << path.split(QChar(';'));
#else
    envPaths << path.split(QChar(':'));
#endif

    it = std::find_if(envPaths.begin(), envPaths.end(), [&exe](const QString &p) { return QFile::exists(p % "/" % exe); });
    if (envPaths.end() != it)
        return QDir::toNativeSeparators(*it % "/" % exe);
    return exe;
}
} // namespace util
