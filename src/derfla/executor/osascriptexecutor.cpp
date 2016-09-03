#include "stdafx.h"
#include "osascriptexecutor.h"

OSAScriptExecutor::OSAScriptExecutor(QObject *parent)
    : Executor (parent)
{
    QString envPath = qgetenv("PATH");
    QStringList paths = envPath.split(QChar(':'));
    auto it = std::find_if(paths.begin(), paths.end(), [](const QString& p) {
        return QFile::exists(p % "/osascript");
    });
    if (paths.end() != it)
        program_ = *it % "/osascript";
}

bool OSAScriptExecutor::run()
{
    return false;
}

void OSAScriptExecutor::doEscaping()
{

}
