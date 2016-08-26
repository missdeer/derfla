#include "stdafx.h"
#include "rubyexecutor.h"

RubyExecutor::RubyExecutor(QObject *parent)
    : Executor (parent)
{

}

bool RubyExecutor::run()
{
    return false;
}
