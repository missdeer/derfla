#include "stdafx.h"
#include "pythonexecutor.h"

PythonExecutor::PythonExecutor(QObject *parent)
    : Executor (parent)
{
    QStringList& paths = util::getEnvPaths();
#if defined(Q_OS_WIN)
    auto it = std::find_if(paths.begin(), paths.end(), [](const QString& p) {
        return QFile::exists(p % "/python.exe");
    });
    if (paths.end() != it)
        program_ = *it % "/python.exe";
#else
    auto it = std::find_if(paths.begin(), paths.end(), [](const QString& p) {
        return QFile::exists(p % "/python");
    });
    if (paths.end() != it)
        program_ = *it % "/python";
#endif
}

bool PythonExecutor::run()
{
    Executor::run();
    ExecutorRunner::instance()->run(uuid_, program_, QStringList() << "-c" << script_);
    return false;
}

void PythonExecutor::doEscaping()
{

}
