#include "stdafx.h"
#include "scriptescape.h"
#include "bashexecutor.h"

#if defined(Q_OS_WIN)
#define EXE "/bash.exe"
#else
#define EXE "/bash"
#endif

BashExecutor::BashExecutor(QObject *parent)
    : Executor (parent)
{
    findProgram(EXE);
}

bool BashExecutor::run()
{
    Executor::run();
    start(program_, QStringList() << "-c" << script_);
    return false;
}

void BashExecutor::doEscaping()
{
    ScriptEscape se(escaping_);
}
