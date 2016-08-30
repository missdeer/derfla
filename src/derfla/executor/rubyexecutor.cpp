#include "stdafx.h"
#include "rubyexecutor.h"

RubyExecutor::RubyExecutor(QObject *parent)
    : Executor (parent)
{
    QString envPath = qgetenv("PATH");
#if defined(Q_OS_WIN)
    QStringList paths = envPath.split(QChar(';'));
    auto it = std::find_if(paths.begin(), paths.end(), [](const QString& p) {
        return QFile::exists(p % "/ruby.exe");
    });
    if (paths.end() != it)
        program_ = *it % "/ruby.exe";
#else
    QStringList paths = envPath.split(QChar(':'));
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
    process_->start(program_, QStringList() << "-e" << cmdline_);
    return false;
}
