#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <QObject>

class Executor : public QObject
{
    Q_OBJECT
public:
    explicit Executor(QObject *parent = 0);
    virtual ~Executor();

    virtual bool run();
    virtual void escape() = 0;
    virtual void setScript(const QString& script) { script_ = script; }
    virtual void setWorkingDirectory(const QString& dir) { workingDirectory_ = dir; }
    virtual void getStdout(QByteArray& output);
    virtual void getStderr(QByteArray& err);

    static Executor* createExecutor(int type);
    static Executor* createExecutor(const QString& type);
signals:
    void errorOccurred(QProcess::ProcessError error);
    void finished(int exitCode, QProcess::ExitStatus exitStatus);
public slots:

protected:
    QSharedPointer<QProcess> process_;
    QString script_;
    QString workingDirectory_;
private:
};

#endif // EXECUTOR_H
