#include "stdafx.h"
#include "perlexecutor.h"

PerlExecutor::PerlExecutor(QObject *parent)
    : Executor (parent)
{

}

bool PerlExecutor::run()
{
    return false;
}
