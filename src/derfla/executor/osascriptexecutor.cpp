#include "stdafx.h"
#include "osascriptexecutor.h"

OSAScriptExecutor::OSAScriptExecutor(QObject *parent)
    : Executor (parent)
{
    QString envPath = qgetenv("PATH");
    QStringList paths = envPath.split(QChar(':'));
    auto it = std::find_if(paths.begin(), paths.end(), [](const QString& p) {
        return QFile::exists(p % "/bash");
    });
    if (paths.end() != it)
        program_ = *it % "/bash";
}

bool OSAScriptExecutor::run()
{
    return false;
}
