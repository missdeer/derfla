#ifndef PYTHONEXECUTOR_H
#define PYTHONEXECUTOR_H

#include "executor.h"

class PythonExecutor : public Executor
{
    Q_OBJECT
public:
    explicit PythonExecutor(QObject *parent = 0);
    bool run();
signals:

public slots:
};

#endif // PYTHONEXECUTOR_H
