#include "stdafx.h"
#include "dbrw.h"
#include "localfsscanner.h"
#include "util.h"
#include "unix_util.h"

namespace unix_util {

    qint64 timestamp = 0;

    bool getAbsoluteFilePathArguments(const QString& exec, const QStringList& paths, QString& filePath, QString& arguments)
    {
        QStringList exe = exec.split(' ');
        auto it = std::find_if(paths.begin(), paths.end(), [&](const QString& path){
            return QFile::exists(path + QDir::separator() + exe[0]);
        });
        if (paths.end() == it)
            return false;
        filePath = *it + QDir::separator() + exe[0];
        if (exe.length() > 1)
        {
            exe.removeFirst();
            arguments = exe.join(' ');
        }
        return false;
    }

    void processFile(const Directory& d, const QFileInfo& fileInfo)
    {
        QString path = qgetenv("PATH");
        QStringList environment = QProcess::systemEnvironment();
        auto it = std::find_if(environment.begin(), environment.end(),
                               [&](const QString& env) { return env.startsWith("PATH="); });
        if (environment.end() != it)
             path = it->mid(5);
        QStringList&& paths = path.split(':');

        QString f(d.directory + QDir::separator() + fileInfo.fileName());
        f.replace("//", "/");

        QString filePath;
        QString arguments;
        QSettings settings(f, QSettings::IniFormat);
        settings.beginGroup("Desktop Entry");
        if (getAbsoluteFilePathArguments(settings.value("Exec").toString(), paths, filePath, arguments))
        {
            QFileInfo fi(filePath);
            QString iconPath = settings.value("Icon").toString();
            if (!QFileInfo(iconPath).isAbsolute() && QFile::exists(iconPath))
            {
                iconPath = d.directory + QDir::separator() + iconPath;
            }
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
