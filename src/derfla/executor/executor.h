#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <QObject>

class Executor : public QObject
{
    Q_OBJECT
public:
    explicit Executor(QObject *parent = 0);
    virtual ~Executor();

    virtual bool run() = 0;
    virtual void setCommandline(const QString& cmdline) { cmdline_ = cmdline; }
    virtual void setWorkingDirectory(const QString& dir) { workingDirectory_ = dir; }
    virtual void setProgram(const QString& program) { program_ = program; }
    virtual void setArguments(const QStringList& arguments) { arguments_ = arguments; }
    virtual void getStdout(QByteArray& output);
    virtual void getStderr(QByteArray& err);

    static Executor* createExecutor(int type);
    static Executor* createExecutor(const QString& type);
signals:

public slots:

protected:
    QSharedPointer<QProcess> process_;
    QString cmdline_;
    QString workingDirectory_;
    QString program_;
    QStringList arguments_;
};

#endif // EXECUTOR_H
