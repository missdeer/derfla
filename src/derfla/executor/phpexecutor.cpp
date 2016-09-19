#include "stdafx.h"
#include "scriptescape.h"
#include "phpexecutor.h"

#if defined(Q_OS_WIN)
#define EXE "/php.exe"
#else
#define EXE "/php"
#endif

PHPExecutor::PHPExecutor(QObject *parent)
    : Executor (parent)
{
    findProgram(EXE);
}

bool PHPExecutor::run()
{
    Executor::run();
    ExecutorRunner::instance()->startProcess(uuid_, program_, QStringList() << "-r" << script_);
    return false;
}

void PHPExecutor::doEscaping()
{
    ScriptEscape se(escaping_);

}
