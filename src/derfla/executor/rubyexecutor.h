#ifndef RUBYEXECUTOR_H
#define RUBYEXECUTOR_H

#include "executor.h"

class RubyExecutor : public Executor
{
    Q_OBJECT
public:
    explicit RubyExecutor(QObject *parent = 0);
    bool run();
signals:

public slots :

private:
    void doEscaping();
};

#endif // RUBYEXECUTOR_H
