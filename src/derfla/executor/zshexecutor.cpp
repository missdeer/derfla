#include "executor.h"
#include "scriptescape.h"
#include "zshexecutor.h"

ZshExecutor::ZshExecutor(QObject *parent)
    : Executor (parent)
{
    QStringList& paths = util::getEnvPaths();
#if defined(Q_OS_WIN)
    auto it = std::find_if(paths.begin(), paths.end(), [](const QString& p) {
        return QFile::exists(p % "/zsh.exe");
    });
    if (paths.end() != it)
        program_ = *it % "/zsh.exe";
#else
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
    ExecutorRunner::instance()->run(uuid_, program_, QStringList() << "-c" << script_);
    return false;
}

void ZshExecutor::doEscaping()
{
    ScriptEscape se(escaping_);

}
