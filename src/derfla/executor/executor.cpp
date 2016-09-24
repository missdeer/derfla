#include "stdafx.h"
#include "executor.h"

Executor::Executor(QObject *parent)
    : ProcessWrapper(parent)
{

}

Executor::~Executor()
{

}


bool Executor::run()
{
    // todo: register executor
    doEscaping();
    return true;
}

void Executor::findProgram(const QString& exe)
{
    QStringList& paths = util::getEnvPaths();
    auto it = std::find_if(paths.begin(), paths.end(), [&exe](const QString& p) {
        return QFile::exists(p % exe);
    });
    if (paths.end() != it)
        program_ = *it % exe;
}

