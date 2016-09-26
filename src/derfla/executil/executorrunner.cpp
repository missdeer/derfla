#include "stdafx.h"
#include "bashexecutor.h"
#include "zshexecutor.h"
#include "osascriptexecutor.h"
#include "perlexecutor.h"
#include "phpexecutor.h"
#include "pythonexecutor.h"
#include "rubyexecutor.h"
#include "executor.h"
#include "executorrunner.h"

ExecutorRunner* ExecutorRunner::instance_;

ExecutorRunner::ExecutorRunner() 
{

}

ExecutorRunner* ExecutorRunner::instance()
{
    if (!instance_)
    {
        instance_ = new ExecutorRunner;
    }

    return instance_;
}

void ExecutorRunner::run(Executor* executor)
{
    executor->run();
}

Executor* ExecutorRunner::createExecutor(int type)
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
    {
        Executor *e = (*it)();
        prcocessList_.append(e);
        connect(e, &QProcess::errorOccurred, this, &ExecutorRunner::errorOccurred);
        connect(e, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &ExecutorRunner::finished);
        return e;
    }
    return nullptr;
}

Executor* ExecutorRunner::createExecutor(const QString& type)
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
    {
        Executor *e = (*it)();
        prcocessList_.append(e);
        connect(e, &QProcess::errorOccurred, this, &ExecutorRunner::errorOccurred);
        connect(e, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &ExecutorRunner::finished);
        return e;
    }
    return nullptr;
}
