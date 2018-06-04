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
    QStringList envPaths;

    QString path = qgetenv("PATH");
    QStringList environment = QProcess::systemEnvironment();
    auto it = std::find_if(environment.begin(), environment.end(),
                           [&](const QString& env) { return env.startsWith("PATH="); });
    if (environment.end() != it)
        path = it->mid(5);
#if defined(Q_OS_WIN)
    envPaths << path.split(QChar(';'));
#else
    envPaths << path.split(QChar(':'));
#endif

    it = std::find_if(envPaths.begin(), envPaths.end(), [&exe](const QString& p) {
        return QFile::exists(p % exe);
    });
    if (envPaths.end() != it)
        program_ = *it % exe;
}

