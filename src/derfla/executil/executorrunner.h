#ifndef EXECUTORRUNNER_H
#define EXECUTORRUNNER_H

#include <QObject>

class Executor;

class ExecutorRunner : public QObject
{
    Q_OBJECT
public:
    static ExecutorRunner* instance();

    void registerProcess(const QUuid& uuid, QSharedPointer<QProcess> process);
    void startProcess(const QUuid& uuid, const QString& program, const QStringList& arguments);

    void getStdout(const QUuid& uuid, QByteArray& output);
    void getStderr(const QUuid& uuid, QByteArray& err);

    void run(Executor* executor);
signals:
    void errorOccurred(QProcess::ProcessError error);
    void finished(int exitCode, QProcess::ExitStatus exitStatus);

public slots:

private:
    QMap<QUuid, QSharedPointer<QProcess>> processMap_;

    static ExecutorRunner* instance_;
    ExecutorRunner();
};

#endif // EXECUTORRUNNER_H