#include "executor.h"
#include "scriptescape.h"
#include "zshexecutor.h"

#if defined(Q_OS_WIN)
#define EXE "/zsh.exe"
#else
#define EXE "/zsh"
#endif

ZshExecutor::ZshExecutor(QObject *parent)
    : Executor (parent)
{
    findProgram(EXE);
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
