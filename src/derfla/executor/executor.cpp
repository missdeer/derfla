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
}

void Executor::getStderr(QByteArray& err)
{

}

Executor* Executor::createExecutor(int type)
{
    return nullptr;
}

Executor* Executor::createExecutor(const QString& type)
{
    return nullptr;
}
