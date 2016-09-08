#include "stdafx.h"
#if defined(Q_OS_WIN)
#include "win_util.h"
#endif
#include "derflaaction.h"

DerflaAction::DerflaAction(QObject *parent)
    : QObject(parent)
    , actionType_(DAT_UNKNOWN)
    , id_(QUuid::createUuid())
{

}


bool DerflaAction::run()
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

QIcon DerflaAction::icon() const
{
    return icon_;
}

void DerflaAction::setIcon(const QIcon& icon)
{
    icon_ = icon;
}

QString DerflaAction::title() const
{
    return title_;
}

void DerflaAction::setTitle(const QString& title)
{
    title_ = title;
}

QString DerflaAction::description() const
{
    return description_;
}

void DerflaAction::setDescription(const QString& description)
{
    description_ = description;
}

QString DerflaAction::target() const
{
    return target_;
}

void DerflaAction::setTarget(const QString& target)
{
    target_ = target;
}

QString DerflaAction::workingDirectory() const
{
    return workingDirectory_;
}

void DerflaAction::setWorkingDirectory(const QString& workingDirectory)
{
    QFileInfo fi(workingDirectory);
    if (fi.isDir())
        workingDirectory_ = workingDirectory;
    else
        workingDirectory_ = fi.absolutePath();
}

DerflaActionType DerflaAction::actionType() const
{
    return actionType_;
}

void DerflaAction::setActionType(const DerflaActionType& actionType)
{
    actionType_ = actionType;
}

QString DerflaAction::arguments() const
{
    return arguments_;
}

void DerflaAction::setArguments(const QString& arguments)
{
    arguments_ = arguments;
}
