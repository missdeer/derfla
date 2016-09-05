#include "stdafx.h"
#include "perlexecutor.h"

PerlExecutor::PerlExecutor(QObject *parent)
    : Executor (parent)
{
    QStringList& paths = util::getEnvPaths();
#if defined(Q_OS_WIN)
    auto it = std::find_if(paths.begin(), paths.end(), [](const QString& p) {
        return QFile::exists(p % "/perl.exe");
    });
    if (paths.end() != it)
        program_ = *it % "/perl.exe";
#else
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

void PerlExecutor::doEscaping()
{

}
