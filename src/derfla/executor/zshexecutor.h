#ifndef ZSHEXECUTOR_H
#define ZSHEXECUTOR_H

#include "executor.h"

class ZshExecutor : public Executor
{
    Q_OBJECT
public:
    explicit ZshExecutor(QObject *parent = 0);
    bool run();
signals:

public slots:
};

#endif // ZSHEXECUTOR_H
