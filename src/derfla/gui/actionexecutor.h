#ifndef ACTIONEXECUTOR_H
#define ACTIONEXECUTOR_H

#include <QObject>
#include "derflaaction.h"

class ActionExecutor : public QObject
{
    Q_OBJECT
public:
    explicit ActionExecutor(QObject *parent = nullptr);

    bool operator()(DerflaActionPtr da);
signals:

public slots:

private:
    bool runScript(DerflaActionPtr da);
    bool shellExecute(DerflaActionPtr da);
    bool terminalCommand(DerflaActionPtr da);
    bool openUrl(DerflaActionPtr da);
    bool revealFile(DerflaActionPtr da);
    bool browseInDerfla(DerflaActionPtr da);
};

#endif // ACTIONEXECUTOR_H
