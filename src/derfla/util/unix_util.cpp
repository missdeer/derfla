#include "stdafx.h"
#include "dbrw.h"
#include "localfsscanner.h"
#include "util.h"
#include "unix_util.h"

namespace unix_util {

    qint64 timestamp = 0;

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
        if (exe.length() > 1)
        {
            exe.removeFirst();
            arguments = exe.join(' ');
        }
        return false;
    }

    QString getIconPath(const QString& iconName)
    {
        if (QFileInfo(iconName).isAbsolute() && QFile::exists(iconName))
            return iconName;

        // find icon file
        // https://specifications.freedesktop.org/icon-theme-spec/icon-theme-spec-latest.html#directory_layout
        QString iconPath = d.directory + QDir::separator() + iconName;
    }

    void processFile(const Directory& d, const QFileInfo& fileInfo)
    {
        QString f(d.directory + QDir::separator() + fileInfo.fileName());
        f.replace("//", "/");

        QString filePath;
        QString arguments;
        QSettings settings(f, QSettings::IniFormat);
        settings.beginGroup("Desktop Entry");
        if (getAbsoluteFilePathArguments(settings.value("Exec").toString(), filePath, arguments))
        {
            QFileInfo fi(filePath);
            QString&& iconPath = getIconPath(settings.value("Icon").toString());
            DBRW::instance()->insertLFS(util::extractPNGFromIcon(iconPath),
                                        settings.value("Name").toString(),
                                        (settings.value("Comment").toString().isEmpty() ? f : settings.value("Comment").toString()) ,
                                        filePath,
                                        arguments,
                                        (settings.value("Path").toString().isEmpty() ? fi.absolutePath() : settings.value("Path").toString()) ,
                                        timestamp,
                                        fi.lastModified().toMSecsSinceEpoch(),
                                        "g"
                                        );
        }
        settings.endGroup();
    }
}
