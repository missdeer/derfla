#include "executor.h"
#include "zshexecutor.h"

ZshExecutor::ZshExecutor(QObject *parent)
    : Executor (parent)
{
    QString envPath = qgetenv("PATH");
#if defined(Q_OS_WIN)
    QStringList paths = envPath.split(QChar(';'));
    auto it = std::find_if(paths.begin(), paths.end(), [](const QString& p) {
        return QFile::exists(p % "/zsh.exe");
    });
    if (paths.end() != it)
        program_ = *it % "/zsh.exe";
#else
    QStringList paths = envPath.split(QChar(':'));
    auto it = std::find_if(paths.begin(), paths.end(), [](const QString& p) {
        return QFile::exists(p % "/zsh");
    });
    if (paths.end() != it)
        program_ = *it % "/zsh";
#endif
}

bool ZshExecutor::run()
{
    Executor::run();
    process_->start(program_, QStringList() << "-c" << cmdline_);
    return false;
}
