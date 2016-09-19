#include "stdafx.h"
#include "scriptescape.h"
#include "pythonexecutor.h"

#if defined(Q_OS_WIN)
#define EXE "/python.exe"
#else
#define EXE "/python"
#endif

PythonExecutor::PythonExecutor(QObject *parent)
    : Executor (parent)
{
    findProgram(EXE);
}

bool PythonExecutor::run()
{
    Executor::run();
    ExecutorRunner::instance()->startProcess(uuid_, program_, QStringList() << "-c" << script_);
    return false;
}

void PythonExecutor::doEscaping()
{
    ScriptEscape se(escaping_);

}
