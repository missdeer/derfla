#include "stdafx.h"
#include "bashexecutor.h"

BashExecutor::BashExecutor(QObject *parent)
    : Executor (parent)
{
    QStringList& paths = util::getEnvPaths();
#if defined(Q_OS_WIN)
    auto it = std::find_if(paths.begin(), paths.end(), [](const QString& p) {
        return QFile::exists(p % "/bash.exe");
    });
    if (paths.end() != it)
        program_ = *it % "/bash.exe";
#else
    auto it = std::find_if(paths.begin(), paths.end(), [](const QString& p) {
        return QFile::exists(p % "/bash");
    });
    if (paths.end() != it)
        program_ = *it % "/bash";
#endif
}

bool BashExecutor::run()
{
    Executor::run();
    process_->start(program_, QStringList() << "-c" << script_);
    return false;
}

void BashExecutor::doEscaping()
{

}
