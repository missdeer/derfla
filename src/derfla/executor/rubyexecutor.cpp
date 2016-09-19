#include "stdafx.h"
#include "scriptescape.h"
#include "rubyexecutor.h"

#if defined(Q_OS_WIN)
#define EXE "/ruby.exe"
#else
#define EXE "/ruby"
#endif

RubyExecutor::RubyExecutor(QObject *parent)
    : Executor (parent)
{
    findProgram(EXE);
}

bool RubyExecutor::run()
{
    Executor::run();
    ExecutorRunner::instance()->run(uuid_, program_, QStringList() << "-e" << script_);
    return false;
}

void RubyExecutor::doEscaping()
{
    ScriptEscape se(escaping_);

}
