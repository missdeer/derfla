#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <QObject>
#include "processwrapper.h"

class Executor : public ProcessWrapper
{
    Q_OBJECT
public:
    explicit Executor(QObject *parent = 0);
    virtual ~Executor();

    virtual bool run();
    virtual void escaping(int esc) { escaping_ = esc; doEscaping(); }
    virtual void setScript(const QString& script) { script_ = script; doEscaping(); }

    void findProgram(const QString& exe);
signals:

public slots:

protected:
    QString script_;
    QString program_;
    int escaping_;
private:
    virtual void doEscaping(){}
};

#endif // EXECUTOR_H
