#include "stdafx.h"
#include "bashexecutor.h"
#include "zshexecutor.h"
#include "osascriptexecutor.h"
#include "perlexecutor.h"
#include "phpexecutor.h"
#include "pythonexecutor.h"
#include "rubyexecutor.h"
#include "executor.h"

Executor::Executor(QObject *parent)
    : QObject(parent)
{

}

Executor::~Executor()
{

}

void Executor::getStdout(QByteArray& output)
{
    output = process_->readAllStandardOutput();
}

void Executor::getStderr(QByteArray& err)
{
    err = process_->readAllStandardError();
}

Executor* Executor::createExecutor(int type)
{
    QMap<int, std::function<Executor*()>> map{
    {0, []()->Executor* { return new BashExecutor; } },
    {1, []()->Executor* { return new PHPExecutor; } },
    {2, []()->Executor* { return new RubyExecutor; } },
    {3, []()->Executor* { return new PythonExecutor; } },
    {4, []()->Executor* { return new PerlExecutor; } },
    {5, []()->Executor* { return new ZshExecutor; } },
#if defined(Q_OS_MAC)
    {6, []()->Executor* { return new OSAScriptExecutor; } },
    {7, []()->Executor* { return new OSAScriptExecutor; } },
#endif
    };
   
    auto it = map.find(type);
    if (map.end() != it)
        return (*it)();
    return nullptr;
}

Executor* Executor::createExecutor(const QString& type)
{
    QMap<QString, std::function<Executor*()>> map{
        { "bash", []()->Executor* { return new BashExecutor; } },
        { "php", []()->Executor* { return new PHPExecutor; } },
        { "ruby", []()->Executor* { return new RubyExecutor; } },
        { "python", []()->Executor* { return new PythonExecutor; } },
        { "perl", []()->Executor* { return new PerlExecutor; } },
        { "zsh", []()->Executor* { return new ZshExecutor; } },
#if defined(Q_OS_MAC)
        { "applescript(as)", []()->Executor* { return new OSAScriptExecutor; } },
        { "applescript(js)", []()->Executor* { return new OSAScriptExecutor; } },
#endif
    };

    auto it = map.find(type);
    if (map.end() != it)
        return (*it)();
    return nullptr;
}

