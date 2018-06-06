#include "stdafx.h"
#include "actionexecutor.h"

ActionExecutor::ActionExecutor(QObject *parent)
    : QObject(parent)
{

}

bool ActionExecutor::operator()(DerflaActionPtr da)
{
    QMap<QString, std::function<bool(DerflaActionPtr)>> m = {
    { "script",          std::bind(&ActionExecutor::runScript,       this, std::placeholders::_1) },
    { "shellExecute",    std::bind(&ActionExecutor::shellExecute,    this, std::placeholders::_1) },
    { "terminalCommand", std::bind(&ActionExecutor::terminalCommand, this, std::placeholders::_1) },
    { "openUrl",         std::bind(&ActionExecutor::openUrl,         this, std::placeholders::_1) },
    { "revealFile",      std::bind(&ActionExecutor::revealFile,      this, std::placeholders::_1) },
    { "browseInDerfla",  std::bind(&ActionExecutor::browseInDerfla,  this, std::placeholders::_1) },
    { "copyText",        std::bind(&ActionExecutor::copyText,        this, std::placeholders::_1) },
};
    if (m.find(da->actionType()) == m.end())
        return false;

    auto f = m[da->actionType()];
    return f(da);
}

void ActionExecutor::errorOccurred()
{

}

void ActionExecutor::finished(int exitCode, QProcess::ExitStatus status)
{
    QProcess* e = qobject_cast<QProcess*>(sender());
    e->deleteLater();
}

bool ActionExecutor::runScript(DerflaActionPtr da)
{
    QMap<QString, QString> m = {
        { "bash",           "-c"},
        { "php",            "-r"},
        { "ruby",           "-e"},
        { "python",         "-c"},
        { "perl",           "-c"},
        { "zsh",            "-c"},
#if defined(Q_OS_MAC)
        { "applescript(as)","-c"},
        { "applescript(js)","-c"},
#endif
    };

    auto it = m.find(da->scriptExecutor());
    if (m.end() != it)
    {
        auto option = m[da->scriptExecutor()];
#if defined(Q_OS_WIN)
        QString exe = findProgram("/" % da->scriptExecutor() % ".exe");
#else
        QString exe = findProgram("/" % da->scriptExecutor());
#endif
        QProcess* e = new QProcess;

        connect(e, &QProcess::errorOccurred, this, &ActionExecutor::errorOccurred);
        connect(e, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &ActionExecutor::finished);

        e->start(exe, QStringList() << option << da->target() << da->arguments().split(QChar(' ')));
    }

#if defined(Q_OS_WIN)
    if (da->scriptExecutor() == "cscript" || da->scriptExecutor() == "wscript")
    {
        // extract resource file
        QString localPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) % "/derfla-temp.vbs";
        QFile f(localPath);
        if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
            return false;
        f.write(da->target().toLocal8Bit());
        f.close();
        ::ShellExecuteW(NULL, L"open", localPath.toStdWString().c_str(), da->arguments().toStdWString().c_str(), NULL, SW_SHOWNORMAL);
    }
#endif

    return true;
}

bool ActionExecutor::shellExecute(DerflaActionPtr da)
{
#if defined(Q_OS_WIN)
    ::ShellExecuteW(NULL,
                    L"open",
                    da->target().toStdWString().c_str(),
                    da->arguments().toStdWString().c_str(),
                    da->workingDirectory().toStdWString().c_str(),
                    SW_SHOWNORMAL);
#elif defined(Q_OS_MAC)
    QStringList args {
        da->target(),
    };
    if (!da->arguments().isEmpty())
        args << "--args"
             << da->arguments().split(' ');
    QProcess::startDetached("/usr/bin/open", args, da->workingDirectory());
#else
    QProcess::startDetached(da->target(), da->arguments().split(' '), da->workingDirectory());
#endif
    return true;
}

bool ActionExecutor::terminalCommand(DerflaActionPtr da)
{
#if defined(Q_OS_WIN)
    QString args = QString("/K %1 %2").arg(da->target()).arg(da->arguments());
    ::ShellExecuteW(NULL,
                    L"open",
                    L"cmd.exe",
                    args.toStdWString().c_str(),
                    da->workingDirectory().toStdWString().c_str(),
                    SW_SHOWNORMAL);

#elif defined(Q_OS_MAC)
    QString cmdline = QString("osascript -e 'tell application \"Terminal\" to do script \"%1 %2\"'").arg(da->target()).arg(da->arguments());
    system(cmdline.toStdString().c_str());
#else
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
                da->target(),
            };
            args << da->arguments();
            QProcess::startDetached(path % "/konsole", args, da->workingDirectory());
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
                da->target(),
            };
            args << da->arguments();
            QProcess::startDetached(path % "/xterm", args, da->workingDirectory());
            return true;
        }
    }
#endif
    return true;
}

bool ActionExecutor::openUrl(DerflaActionPtr da)
{
    return QDesktopServices::openUrl(QUrl(da->target()));
}

bool ActionExecutor::revealFile(DerflaActionPtr da)
{
#if defined(Q_OS_WIN)
    QString arg = QString("/select,\"%1\"").arg(QDir::toNativeSeparators(da->target()));
    ::ShellExecuteW(NULL, L"open", L"explorer.exe", arg.toStdWString().c_str(), NULL, SW_SHOWNORMAL);
#elif defined(Q_OS_MAC)
    QStringList scriptArgs;
    scriptArgs << QLatin1String("-e")
               << QString::fromLatin1("tell application \"Finder\" to reveal POSIX file \"%1\"").arg(da->target());
    QProcess::execute(QLatin1String("/usr/bin/osascript"), scriptArgs);
    scriptArgs.clear();
    scriptArgs << QLatin1String("-e")
               << QLatin1String("tell application \"Finder\" to activate");
    QProcess::execute("/usr/bin/osascript", scriptArgs);
#else
#endif
    return true;
}

bool ActionExecutor::browseInDerfla(DerflaActionPtr da)
{
    return true;
}

bool ActionExecutor::copyText(DerflaActionPtr da)
{
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(da->target());
    return true;
}

QString ActionExecutor::findProgram(const QString &exe)
{
    QStringList envPaths;

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

    it = std::find_if(envPaths.begin(), envPaths.end(), [&exe](const QString& p) {
        return QFile::exists(p % exe);
    });
    if (envPaths.end() != it)
        return *it % exe;
    return exe;
}
