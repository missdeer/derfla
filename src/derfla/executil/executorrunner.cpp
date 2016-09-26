#include "stdafx.h"
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

void ExecutorRunner::registerProcess(const QUuid& uuid, QSharedPointer<QProcess> process)
{
    processMap_.insert(uuid, process);

    connect(process.data(), SIGNAL(errorOccurred(QProcess::ProcessError)), this, SIGNAL(errorOccurred(QProcess::ProcessError)));
    connect(process.data(), SIGNAL(finished(int, QProcess::ExitStatus)), this, SIGNAL(finished(int, QProcess::ExitStatus)));
}

void ExecutorRunner::startProcess(const QUuid& uuid, const QString& program, const QStringList& arguments)
{
    auto it = processMap_.find(uuid);
    if (processMap_.end() != it)
    {
        (*it)->start(program, arguments);
    }
}

void ExecutorRunner::run(Executor* executor)
{
    executor->run();
}

void ExecutorRunner::getStdout(const QUuid& uuid, QByteArray& output)
{
    auto it = processMap_.find(uuid);
    if (processMap_.end() != it)
    {
        output = (*it)->readAllStandardOutput();
    }
}

void ExecutorRunner::getStderr(const QUuid& uuid, QByteArray& err)
{
    auto it = processMap_.find(uuid);
    if (processMap_.end() != it)
    {
        err = (*it)->readAllStandardError();
    }
}
