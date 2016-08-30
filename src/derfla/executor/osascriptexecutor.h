#ifndef OSASCRIPTEXECUTOR_H
#define OSASCRIPTEXECUTOR_H

#include "executor.h"

class OSAScriptExecutor : public Executor
{
    Q_OBJECT
public:
    explicit OSAScriptExecutor(QObject *parent = 0);
    bool run();
signals:

public slots:

private:
    QString program_;
};

#endif // OSASCRIPTEXECUTOR_H
