#include "stdafx.h"
#include "directory.h"
#include "util.h"
#include "unix_util.h"

namespace util {

    bool getAbsoluteFilePathArguments(const QString& exec, QString& filePath, QString& arguments)
    {
        QStringList& envPaths = util::getEnvPaths();
        QStringList exe = exec.split(' ');
        auto it = std::find_if(envPaths.begin(), envPaths.end(), [&](const QString& path){
            return QFile::exists(path + QDir::separator() + exe[0]);
        });
        if (envPaths.end() == it)
            return false;
        filePath = *it + QDir::separator() + exe[0];
        exe.removeAll("%f");
        exe.removeAll("%U");
        exe.removeAll("%F");
        exe.removeAll("%u");
        if (exe.length() > 1)
        {
            exe.removeFirst();
            arguments = exe.join(' ');
        }
        return true;
    }

    QString getIconPath(const QString& iconName)
    {
        if (QFile::exists(iconName) && QFileInfo(iconName).isAbsolute())
            return iconName;

        // https://specifications.freedesktop.org/icon-theme-spec/icon-theme-spec-latest.html#directory_layout
        QStringList xdg = QString(qgetenv("XDG_DATA_DIRS")).split(QChar(':'));
        QStringList iconDirs { QString("%1/.icons").arg(qgetenv("HOME").constData()) };
        for (const QString& x : xdg)
        {
            iconDirs << (x % "/icons" );
        }
        iconDirs << "/usr/share/pixmaps";

        QStringList sizes {
            "256x256",
            "192x192",
            "128x128",
            "96x96",
            "72x72",
            "64x64",
            "48x48",
            "32x32",
            "24x24",
            "16x16",
            "8x8",
        };
        QStringList suffixes {
            "png", "xpm", "svg",
        };

        for (const QString& iconDir: iconDirs)
        {
            for (const QString& size: sizes)
            {
                for (const QString& suffix: suffixes)
                {
                    QString f = iconDir % "/hicolor/" % size % "/apps/" % iconName % "." % suffix;
                    if (QFile::exists(f))
                        return f;
                }
            }
        }
        return QString();
    }

    void processFile(const Directory& d, const QFileInfo& fileInfo, LFSInserter inserter)
    {
        QString f(d.directory + QDir::separator() + fileInfo.fileName());
        f.replace("//", "/");
        qWarning() << f;
        QString filePath;
        QString arguments;
        QSettings settings(f, QSettings::IniFormat);
        settings.setIniCodec("UTF-8");
        settings.beginGroup("Desktop Entry");
        if (getAbsoluteFilePathArguments(settings.value("Exec").toString(), filePath, arguments))
        {
            QFileInfo fi(filePath);
            if (fi.permission(QFile::ExeGroup) && fi.isFile())
            {
                QStringList& localeNames = util::getLocaleNames();
                QString name = settings.value("Name").toString();
                QString comment = settings.value("Comment").toString();
                bool terminal = settings.value("Terminal").toBool();
                for (const QString& locale : localeNames)
                {
                    if (!settings.value(QString("Name[%1]").arg(locale)).toString().isEmpty())
                    {
                        name = settings.value(QString("Name[%1]").arg(locale)).toString();
                    }
                    if (!settings.value(QString("Comment[%1]").arg(locale)).toString().isEmpty())
                    {
                        comment = settings.value(QString("Comment[%1]").arg(locale)).toString();
                    }
                }
                QString&& iconPath = getIconPath(settings.value("Icon").toString());
                inserter(util::extractPNGFromIcon(iconPath),
                                            name,
                                            (comment.isEmpty() ? filePath : comment) ,
                                            filePath,
                                            arguments,
                                            (settings.value("Path").toString().isEmpty() ? fi.absolutePath() : settings.value("Path").toString()) ,
                                            timestamp,
                                            fi.lastModified().toMSecsSinceEpoch(),
                                            (terminal ? "terminalCommand" : "shellExecute")
                                            );
            }
        }
        settings.endGroup();
    }
}
