#include "stdafx.h"
#include "rubyexecutor.h"

RubyExecutor::RubyExecutor(QObject *parent)
    : Executor (parent)
{
    QStringList& paths = util::getEnvPaths();
#if defined(Q_OS_WIN)
    auto it = std::find_if(paths.begin(), paths.end(), [](const QString& p) {
        return QFile::exists(p % "/ruby.exe");
    });
    if (paths.end() != it)
        program_ = *it % "/ruby.exe";
#else
    auto it = std::find_if(paths.begin(), paths.end(), [](const QString& p) {
        return QFile::exists(p % "/ruby");
    });
    if (paths.end() != it)
        program_ = *it % "/ruby";
#endif
}

bool RubyExecutor::run()
{
    Executor::run();
    process_->start(program_, QStringList() << "-e" << script_);
    return false;
}

void RubyExecutor::doEscaping()
{

}
