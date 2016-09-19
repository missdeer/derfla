#include "stdafx.h"
#include "scriptescape.h"
#include "osascriptexecutor.h"

OSAScriptExecutor::OSAScriptExecutor(QObject *parent)
    : Executor (parent)
{
    findProgram("/osascript");
}

bool OSAScriptExecutor::run()
{
    Executor::run();
    ExecutorRunner::instance()->startProcess(uuid_, program_, QStringList() << "-c" << script_);
    return false;
}

void OSAScriptExecutor::doEscaping()
{
    ScriptEscape se(escaping_);
}
