#ifndef EXECUTORRUNNER_H
#define EXECUTORRUNNER_H

#include <QObject>

class ExecutorRunner : public QObject
{
    Q_OBJECT
public:
    static ExecutorRunner* instance();

    void run(const QUuid& uuid, QSharedPointer<QProcess> process);
    void run(const QUuid& uuid, const QString& program, const QStringList& arguments);

    void getStdout(const QUuid& uuid, QByteArray& output);
    void getStderr(const QUuid& uuid, QByteArray& err);

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