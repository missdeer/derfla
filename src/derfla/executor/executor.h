#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <QObject>
#include "executorrunner.h"

class Executor : public QObject
{
    Q_OBJECT
public:
    explicit Executor(QObject *parent = 0);
    virtual ~Executor();

    virtual bool run();
    virtual void escaping(int esc) { escaping_ = esc;  }
    virtual void setScript(const QString& script) { script_ = script; doEscaping(); }
    virtual void setWorkingDirectory(const QString& dir) { workingDirectory_ = dir; }
    virtual void getStdout(QByteArray& output);
    virtual void getStderr(QByteArray& err);

    static Executor* createExecutor(int type);
    static Executor* createExecutor(const QString& type);

    const QUuid& getUuid() const;
    void setUuid(const QUuid &uuid);
    void findProgram(const QString& exe);
signals:

public slots:

protected:
    QString script_;
    QString workingDirectory_;
    QString program_;
    int escaping_;
    QUuid uuid_;
private:
    virtual void doEscaping() = 0;
};

#endif // EXECUTOR_H
