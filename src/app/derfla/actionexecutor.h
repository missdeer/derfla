#ifndef ACTIONEXECUTOR_H
#define ACTIONEXECUTOR_H

#include <QMap>
#include <QObject>
#include <QProcess>

#include "derflaaction.h"

class ActionExecutor : public QObject
{
    Q_OBJECT
public:
    explicit ActionExecutor(QObject *parent = nullptr);

    bool operator()(const DerflaActionPtr &action);
signals:

private:
    static bool runScript(const DerflaActionPtr &action);
    static bool runAsAdministrator(const DerflaActionPtr &action);
    static bool shellExecute(const DerflaActionPtr &action);
    static bool terminalCommand(const DerflaActionPtr &action);
    static bool openUrl(const DerflaActionPtr &action);
    static bool revealFile(const DerflaActionPtr &action);
    static bool browseUrl(const DerflaActionPtr &action);
    static bool browseAsWeb(const DerflaActionPtr &action);
    static bool viewFile(const DerflaActionPtr &action);
    static bool viewText(const DerflaActionPtr &action);
    static bool copyText(const DerflaActionPtr &action);
};

#endif // ACTIONEXECUTOR_H
