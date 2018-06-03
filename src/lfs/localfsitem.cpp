#include "stdafx.h"
#if defined(Q_OS_WIN)
#include "win_util.h"
#endif
#include "LocalFSItem.h"

LocalFSItem::LocalFSItem(QObject *parent)
    : QObject(parent)
    , actionType_(DAT_UNKNOWN)
    , id_(QUuid::createUuid())
{

}


bool LocalFSItem::run()
{
#if defined(Q_OS_WIN)
    if (QFileInfo(target_).suffix() == "exe" && QFileInfo(target_).fileName() != "cmd.exe" && util::isConsoleApplication(target_))
    {
        QString args = QString("/K %1 %2").arg(target_).arg(arguments_);
        ::ShellExecuteW(NULL,
            L"open",
            L"cmd.exe",
            args.toStdWString().c_str(),
            workingDirectory_.toStdWString().c_str(),
            SW_SHOWNORMAL);
    }
    else
    {
        ::ShellExecuteW(NULL,
            L"open",
            target_.toStdWString().c_str(),
            arguments_.toStdWString().c_str(),
            workingDirectory_.toStdWString().c_str(),
            SW_SHOWNORMAL);
    }
#elif defined(Q_OS_MAC)
    if (QFileInfo(target_).isDir())
    {
        QStringList args {
            target_,
        };
        if (!arguments_.isEmpty())
            args << "--args"
                 << arguments_.split(' ');
        QProcess::startDetached("/usr/bin/open", args, workingDirectory_);
    }
    else
    {
        QString cmdline = QString("osascript -e 'tell application \"Terminal\" to do script \"%1 %2\"'").arg(target_).arg(arguments_);
        system(cmdline.toStdString().c_str());
    }
#else
    if (actionType_ == DAT_CONSOLE && !QFileInfo(target_).baseName().contains("term", Qt::CaseInsensitive))
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
                    target_,
                };
                args << arguments_;
                QProcess::startDetached(path % "/konsole", args, workingDirectory_);
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
                    target_,
                };
                args << arguments_;
                QProcess::startDetached(path % "/xterm", args, workingDirectory_);
                return true;
            }
        }
    }

    QProcess::startDetached(target_, arguments_.split(' '), workingDirectory_);
#endif
    return true;
}

QIcon LocalFSItem::icon() const
{
    return icon_;
}

void LocalFSItem::setIcon(const QIcon& icon)
{
    icon_ = icon;
}

QString LocalFSItem::title() const
{
    return title_;
}

void LocalFSItem::setTitle(const QString& title)
{
    title_ = title;
}

QString LocalFSItem::description() const
{
    return description_;
}

void LocalFSItem::setDescription(const QString& description)
{
    description_ = description;
}

QString LocalFSItem::target() const
{
    return target_;
}

void LocalFSItem::setTarget(const QString& target)
{
    target_ = target;
}

QString LocalFSItem::workingDirectory() const
{
    return workingDirectory_;
}

void LocalFSItem::setWorkingDirectory(const QString& workingDirectory)
{
    QFileInfo fi(workingDirectory);
    if (fi.isDir())
        workingDirectory_ = workingDirectory;
    else
        workingDirectory_ = fi.absolutePath();
}

LocalFSItemType LocalFSItem::actionType() const
{
    return actionType_;
}

void LocalFSItem::setActionType(const LocalFSItemType& actionType)
{
    actionType_ = actionType;
}

QString LocalFSItem::arguments() const
{
    return arguments_;
}

void LocalFSItem::setArguments(const QString& arguments)
{
    arguments_ = arguments;
}
