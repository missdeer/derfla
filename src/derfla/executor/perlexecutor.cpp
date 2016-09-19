#include "stdafx.h"
#include "scriptescape.h"
#include "perlexecutor.h"

#if defined(Q_OS_WIN)
#define EXE "/perl.exe"
#else
#define EXE "/perl"
#endif

PerlExecutor::PerlExecutor(QObject *parent)
    : Executor (parent)
{
    findProgram(EXE);
}

bool PerlExecutor::run()
{
    Executor::run();
    ExecutorRunner::instance()->startProcess(uuid_, program_, QStringList() << "-c" << script_);
    return false;
}

void PerlExecutor::doEscaping()
{
    ScriptEscape se(escaping_);
}
