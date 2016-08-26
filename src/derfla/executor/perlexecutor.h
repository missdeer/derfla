#ifndef PERLEXECUTOR_H
#define PERLEXECUTOR_H

#include "executor.h"

class PerlExecutor : public Executor
{
    Q_OBJECT
public:
    explicit PerlExecutor(QObject *parent = 0);
    bool run();
signals:

public slots:
};

#endif // PERLEXECUTOR_H
