#ifndef ACTIONEXECUTOR_H
#define ACTIONEXECUTOR_H

#include <QObject>
#include <QProcess>
#include "derflaaction.h"

class ActionExecutor : public QObject
{
    Q_OBJECT
public:
    explicit ActionExecutor(QObject *parent = nullptr);

    bool operator()(DerflaActionPtr da);
signals:

public slots:
    void errorOccurred();
    void finished(int exitCode, QProcess::ExitStatus status);
private:
    bool runScript(DerflaActionPtr da);
    bool shellExecute(DerflaActionPtr da);
    bool terminalCommand(DerflaActionPtr da);
    bool openUrl(DerflaActionPtr da);
    bool revealFile(DerflaActionPtr da);
    bool browseInDerfla(DerflaActionPtr da);
    bool copyText(DerflaActionPtr da);

    QString findProgram(const QString& exe);
};

#endif // ACTIONEXECUTOR_H
