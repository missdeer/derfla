#ifndef BASHEXECUTOR_H
#define BASHEXECUTOR_H

#include "executor.h"

class BashExecutor : public Executor
{
    Q_OBJECT
public:
    explicit BashExecutor(QObject *parent = 0);
    bool run();
signals:

public slots:

private:
    QString program_;
    void escape();
};

#endif // BASHEXECUTOR_H
