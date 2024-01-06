#include "stdafx.h"

#include "actionexecutor.h"
#include "derflaapp.h"
#include "util.h"

ActionExecutor::ActionExecutor(QObject *parent) : QObject(parent) {}

bool ActionExecutor::operator()(const DerflaActionPtr &action)
{
    static const QMap<QString, std::function<bool(DerflaActionPtr)>> actionExecutorMap = {
        {"script", [](const DerflaActionPtr &action) { return runScript(action); }},
        {"runAsAdministrator", [](const DerflaActionPtr &action) { return runAsAdministrator(action); }},
        {"shellExecute", [](const DerflaActionPtr &action) { return shellExecute(action); }},
        {"terminalCommand", [](const DerflaActionPtr &action) { return terminalCommand(action); }},
        {"openUrl", [](const DerflaActionPtr &action) { return openUrl(action); }},
        {"revealFile", [](const DerflaActionPtr &action) { return revealFile(action); }},
        {"browseUrl", [](const DerflaActionPtr &action) { return browseUrl(action); }},
        {"browseAsWeb", [](const DerflaActionPtr &action) { return browseAsWeb(action); }},
        {"viewFile", [](const DerflaActionPtr &action) { return viewFile(action); }},
        {"viewText", [](const DerflaActionPtr &action) { return viewText(action); }},
        {"copyText", [](const DerflaActionPtr &action) { return copyText(action); }},
    };
    const auto iter = actionExecutorMap.find(action->actionType());
    if (actionExecutorMap.end() == iter)
    {
        return false;
    }

    auto executor = iter.value();
    return executor(action);
}

bool ActionExecutor::runScript(const DerflaActionPtr &action)
{
    static const QMap<QString, QString> runnerMap = {
        {"bash", "-c"},
        {"php", "-r"},
        {"ruby", "-e"},
        {"python", "-c"},
        {"perl", "-c"},
        {"zsh", "-c"},
        {"lua", "-e"},
        {"sh", "-c"},
        {"node", "-e"},
        {"tcl", "-c"},
#if defined(Q_OS_MAC)
        {"applescript(as)", "-c"},
        {"applescript(js)", "-c"},
#endif
#if defined(Q_OS_WIN)
        {"powershell", "-Command"},
#endif
    };

    const auto iter = runnerMap.find(action->scriptExecutor());
    if (iter == runnerMap.end())
    {
        return false;
    }

    const auto &option   = iter.value();
    QSettings  &settings = derflaApp->settings();
    auto        exe      = settings.value(action->scriptExecutor()).toString();
    if (!QFile::exists(exe))
    {
        exe = QStandardPaths::findExecutable(action->scriptExecutor());
    }
    if (!QFile::exists(exe))
    {
        return false;
    }
    auto *process = new QProcess;

    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), process, &QObject::deleteLater);
    process->start(exe, QStringList() << option << action->target() << action->arguments().split(QChar(' ')));

#if defined(Q_OS_WIN)
    if (action->scriptExecutor() == "cscript" || action->scriptExecutor() == "wscript")
    {
        // extract resource file
        QString localPath = QDir::tempPath() + "/derfla-temp.vbs";
        QFile   file(localPath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            return false;
        }
        file.write(action->target().toLocal8Bit());
        file.close();
        ::ShellExecuteW(nullptr, L"open", localPath.toStdWString().c_str(), action->arguments().toStdWString().c_str(), nullptr, SW_SHOWNORMAL);
    }
#endif

    return true;
}

bool ActionExecutor::runAsAdministrator(const DerflaActionPtr &action)
{
#if defined(Q_OS_WIN)
    SHELLEXECUTEINFO execInfo;
    ZeroMemory(&execInfo, sizeof(execInfo));
    execInfo.lpFile       = action->target().toStdWString().c_str();
    execInfo.cbSize       = sizeof(execInfo);
    execInfo.lpVerb       = L"runas";
    execInfo.lpParameters = action->arguments().toStdWString().c_str();
    execInfo.lpDirectory  = action->workingDirectory().toStdWString().c_str();
    execInfo.fMask        = SEE_MASK_NOCLOSEPROCESS;
    execInfo.nShow        = SW_SHOWNORMAL;
    return !ShellExecuteEx(&execInfo);
#elif defined(Q_OS_MAC)
#else
#endif

    return true;
}

bool ActionExecutor::shellExecute(const DerflaActionPtr &action)
{
#if defined(Q_OS_WIN)
    ::ShellExecuteW(nullptr,
                    L"open",
                    action->target().toStdWString().c_str(),
                    action->arguments().toStdWString().c_str(),
                    action->workingDirectory().toStdWString().c_str(),
                    SW_SHOWNORMAL);
#elif defined(Q_OS_MAC)
    QStringList args {
        action->target(),
    };
    if (!action->arguments().isEmpty())
        args << "--args" << action->arguments().split(' ');
    QProcess::startDetached("/usr/bin/open", args, action->workingDirectory());
#else
    QProcess::startDetached(action->target(), action->arguments().split(' '), action->workingDirectory());
#endif
    return true;
}

bool ActionExecutor::terminalCommand(const DerflaActionPtr &action)
{
#if defined(Q_OS_WIN)
    QString args = QString("/K %1 %2").arg(action->target(), action->arguments());
    ::ShellExecuteW(nullptr, L"open", L"cmd.exe", args.toStdWString().c_str(), action->workingDirectory().toStdWString().c_str(), SW_SHOWNORMAL);

#elif defined(Q_OS_MAC)
    QString cmdline =
        QString("/usr/bin/osascript -e 'tell application \"Terminal\" to do script \"%1 %2\"'").arg(action->target()).arg(action->arguments());
    system(cmdline.toStdString().c_str());
#else
    // console application, running in a terminal
    // konsole/xterm/gnome-terminal...
    QStringList &paths = util::getEnvPaths();
    for (const QString &path : paths)
    {
        if (QFile::exists(path % "/konsole"))
        {
            QStringList args {
                "--noclose",
                "-e",
                action->target(),
            };
            args << action->arguments();
            QProcess::startDetached(path % "/konsole", args, action->workingDirectory());
            return true;
        }
    }
    for (const QString &path : paths)
    {
        if (QFile::exists(path % "/xterm"))
        {
            QStringList args {
                "-hold",
                "-e",
                action->target(),
            };
            args << action->arguments();
            QProcess::startDetached(path % "/xterm", args, action->workingDirectory());
            return true;
        }
    }
#endif
    return true;
}

bool ActionExecutor::openUrl(const DerflaActionPtr &action)
{
    return QDesktopServices::openUrl(QUrl(action->target()));
}

bool ActionExecutor::revealFile(const DerflaActionPtr &action)
{
#if defined(Q_OS_WIN)
    QString arg = QString("/select,\"%1\"").arg(QDir::toNativeSeparators(action->target()));
    ::ShellExecuteW(nullptr, L"open", L"explorer.exe", arg.toStdWString().c_str(), nullptr, SW_SHOWNORMAL);
#elif defined(Q_OS_MAC)
    QStringList scriptArgs;
    scriptArgs << QLatin1String("-e") << QString::fromLatin1("tell application \"Finder\" to reveal POSIX file \"%1\"").arg(action->target());
    QProcess::execute(QLatin1String("/usr/bin/osascript"), scriptArgs);
    scriptArgs.clear();
    scriptArgs << QLatin1String("-e") << QLatin1String("tell application \"Finder\" to activate");
    QProcess::execute("/usr/bin/osascript", scriptArgs);
#else
#endif
    return true;
}

bool ActionExecutor::browseUrl(const DerflaActionPtr & /*unused*/)
{
    return true;
}

bool ActionExecutor::copyText(const DerflaActionPtr &action)
{
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(action->target());
    return true;
}

bool ActionExecutor::browseAsWeb(const DerflaActionPtr &action)
{
    return true;
}

bool ActionExecutor::viewFile(const DerflaActionPtr &action)
{
    return true;
}

bool ActionExecutor::viewText(const DerflaActionPtr &action)
{
    return true;
}
