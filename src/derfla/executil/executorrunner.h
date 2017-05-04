#ifndef EXECUTORRUNNER_H
#define EXECUTORRUNNER_H

#include <QObject>

class Executor;

class ExecutorRunner : public QObject
{
    Q_OBJECT
public:
    static ExecutorRunner* instance();

    Executor* createExecutor(int type);
    Executor* createExecutor(const QString& type);

    void run(Executor* executor);
signals:
    void errorOccurred(QProcess::ProcessError error);
    void finished(int exitCode, QProcess::ExitStatus exitStatus);

public slots:

private:
    QList<Executor*> prcocessList_;

    static ExecutorRunner* instance_;
    ExecutorRunner();
};

#endif // EXECUTORRUNNER_H
