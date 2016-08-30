#include "stdafx.h"
#include "pythonexecutor.h"

PythonExecutor::PythonExecutor(QObject *parent)
    : Executor (parent)
{
    QString envPath = qgetenv("PATH");
#if defined(Q_OS_WIN)
    QStringList paths = envPath.split(QChar(';'));
    auto it = std::find_if(paths.begin(), paths.end(), [](const QString& p) {
        return QFile::exists(p % "/python.exe");
    });
    if (paths.end() != it)
        program_ = *it % "/python.exe";
#else
    QStringList paths = envPath.split(QChar(':'));
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
    process_->start(program_, QStringList() << "-c" << cmdline_);
    return false;
}
