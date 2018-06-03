#include "stdafx.h"
#include "qtsingleapplication.h"
#include <boost/scope_exit.hpp>
#include <QTextStream>
#include <iostream>
#if defined(Q_OS_WIN)
#include "win_util.h"
#endif
#include "dbrw.h"
#include "localfsitem.h"
#include "localfsscanner.h"

using namespace std;


bool run(LocalFSItemPtr item)
{
#if defined(Q_OS_WIN)
    if (QFileInfo(item->target()).suffix() == "exe" && QFileInfo(item->target()).fileName() != "cmd.exe" && util::isConsoleApplication(item->target()))
    {
        QString args = QString("/K %1 %2").arg(item->target()).arg(item->arguments());
        ::ShellExecuteW(NULL,
            L"open",
            L"cmd.exe",
            args.toStdWString().c_str(),
            item->workingDirectory().toStdWString().c_str(),
            SW_SHOWNORMAL);
    }
    else
    {
        ::ShellExecuteW(NULL,
            L"open",
            item->target().toStdWString().c_str(),
            item->arguments().toStdWString().c_str(),
            item->workingDirectory().toStdWString().c_str(),
            SW_SHOWNORMAL);
    }
#elif defined(Q_OS_MAC)
    if (QFileInfo(item->target()).isDir())
    {
        QStringList args {
            item->target(),
        };
        if (!item->arguments().isEmpty())
            args << "--args"
                 << item->arguments().split(' ');
        QProcess::startDetached("/usr/bin/open", args, item->workingDirectory());
    }
    else
    {
        QString cmdline = QString("osascript -e 'tell application \"Terminal\" to do script \"%1 %2\"'").arg(item->target()).arg(item->arguments());
        system(cmdline.toStdString().c_str());
    }
#else
    if (actionType_ == DAT_CONSOLE && !QFileInfo(item->target()).baseName().contains("term", Qt::CaseInsensitive))
    {
        // console application, running in a terminal
        // konsole/xterm/gnome-terminal...
        QStringList& paths = util::getEnvPaths();
        for (const QString& path : paths)
        {
            if (QFile::exists(path % "/konsole"))
            {
                QStringList args {
                    "--noclose",
                    "-e",
                    item->target(),
                };
                args << item->arguments();
                QProcess::startDetached(path % "/konsole", args, item->workingDirectory());
                return true;
            }
        }
        for (const QString& path : paths)
        {
            if (QFile::exists(path % "/xterm"))
            {
                QStringList args {
                    "-hold",
                    "-e",
                    item->target(),
                };
                args << item->arguments();
                QProcess::startDetached(path % "/xterm", args, item->workingDirectory());
                return true;
            }
        }
    }

    QProcess::startDetached(item->target(), item->arguments().split(' '), item->workingDirectory());
#endif
    return true;
}

int main(int argc, char *argv[])
{
#if !defined(Q_OS_WIN)
    // increase the number of file that can be opened.
    struct rlimit rl;
    getrlimit(RLIMIT_NOFILE, &rl);

    rl.rlim_cur = qMin(rl.rlim_cur, rl.rlim_max);
    setrlimit(RLIMIT_NOFILE, &rl);
#endif
    SharedTools::QtSingleApplication a("LFS", argc, argv);

    a.setApplicationName("LFS");
    a.setApplicationVersion("1.0");
    a.setOrganizationDomain("dfordsoft.com");
    a.setOrganizationName("Derfla");

#if defined(Q_OS_WIN)
    CoInitialize(NULL);
    BOOST_SCOPE_EXIT(void) {
        CoUninitialize();
    } BOOST_SCOPE_EXIT_END
#endif

    DBRW dbrw;
    if (argc == 2)
    {
        if (QString(argv[1]).compare("/exit", Qt::CaseInsensitive) == 0 && a.isRunning())
        {
            a.sendMessage("/exit");
            return 0;
        }
        QString res = dbrw.search(QString(argv[1]));
        QTextStream ts( stdout );
        ts << res;
    }

    if (argc == 3 && QString(argv[1]).compare("/run", Qt::CaseInsensitive) == 0)
    {
        QByteArray i = QByteArray::fromRawData((const char*)argv[2], strlen((const char*)argv[2]));
        QJsonDocument doc = QJsonDocument::fromJson(i);
        if (doc.isObject())
        {
            QJsonObject o = doc.object();
            LocalFSItemPtr item(new LocalFSItem);
            if (o["title"].isString())
                item->setTitle(o["title"].toString());
            if (o["description"].isString())
                item->setDescription(o["description"].toString());
            if (o["workingDir"].isString())
                item->setWorkingDirectory(o["workingDir"].toString());
            if (o["arguments"].isString())
                item->setArguments(o["arguments"].toString());
            if (o["target"].isString())
                item->setTarget(o["target"].toString());
            run(item);
        }
    }

    if (a.isRunning())
        return 0;


    LocalFSScanner scanner(dbrw);
    scanner.start();

    QObject::connect(&a, &SharedTools::QtSingleApplication::messageReceived, [&](const QString &message, QObject *socket){
        if (message.compare("/exit", Qt::CaseInsensitive) == 0)
        {
            scanner.stop();
            a.quit();
        }
    });

    return a.exec();
}
