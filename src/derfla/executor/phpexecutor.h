#ifndef PHPEXECUTOR_H
#define PHPEXECUTOR_H

#include "executor.h"

class PHPExecutor : public Executor
{
    Q_OBJECT
public:
    explicit PHPExecutor(QObject *parent = 0);
    bool run();
signals:

public slots:
};

#endif // PHPEXECUTOR_H