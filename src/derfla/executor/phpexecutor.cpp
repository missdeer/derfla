#include "stdafx.h"
#include "phpexecutor.h"

PHPExecutor::PHPExecutor(QObject *parent)
    : Executor (parent)
{
    QString envPath = qgetenv("PATH");
#if defined(Q_OS_WIN)
    QStringList paths = envPath.split(QChar(';'));
    auto it = std::find_if(paths.begin(), paths.end(), [](const QString& p) {
        return QFile::exists(p % "/php.exe");
    });
    if (paths.end() != it)
        program_ = *it % "/php.exe";
#else
    QStringList paths = envPath.split(QChar(':'));
    auto it = std::find_if(paths.begin(), paths.end(), [](const QString& p) {
        return QFile::exists(p % "/php");
    });
    if (paths.end() != it)
        program_ = *it % "/php";
#endif
}

bool PHPExecutor::run()
{
    Executor::run();
    process_->start(program_, QStringList() << "-r" << script_);
    return false;
}
