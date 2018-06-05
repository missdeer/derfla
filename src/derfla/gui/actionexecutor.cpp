#include "stdafx.h"
#include "actionexecutor.h"

ActionExecutor::ActionExecutor(QObject *parent)
    : QObject(parent)
{

}

bool ActionExecutor::operator()(DerflaActionPtr da)
{
    /*
     * script
     * shellExecute
     * terminalCommand
     * openUrl
     * revealFile
     * browseInDerfla
     */

    return true;
}
