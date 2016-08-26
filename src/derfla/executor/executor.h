#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <QObject>

class Executor : public QObject
{
    Q_OBJECT
public:
    explicit Executor(QObject *parent = 0);

    virtual bool run() = 0;
signals:

public slots:
};

#endif // EXECUTOR_H
