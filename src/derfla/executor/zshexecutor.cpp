#include "executor.h"
#include "zshexecutor.h"

ZshExecutor::ZshExecutor(QObject *parent)
    : Executor (parent)
{

}

bool ZshExecutor::run()
{
    return false;
}
