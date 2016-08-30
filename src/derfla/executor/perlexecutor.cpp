#include "stdafx.h"
#include "perlexecutor.h"

PerlExecutor::PerlExecutor(QObject *parent)
    : Executor (parent)
{
    QString envPath = qgetenv("PATH");
#if defined(Q_OS_WIN)
    QStringList paths = envPath.split(QChar(';'));
    auto it = std::find_if(paths.begin(), paths.end(), [](const QString& p) {
        return QFile::exists(p % "/perl.exe");
    });
    if (paths.end() != it)
        program_ = *it % "/perl.exe";
#else
    QStringList paths = envPath.split(QChar(':'));
    auto it = std::find_if(paths.begin(), paths.end(), [](const QString& p) {
        return QFile::exists(p % "/perl");
    });
    if (paths.end() != it)
        program_ = *it % "/perl";
#endif
}

bool PerlExecutor::run()
{
    Executor::run();
    process_->start(program_, QStringList() << "-c" << script_);
    return false;
}

void PerlExecutor::escape()
{

}
