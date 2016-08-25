#include "stdafx.h"
#include "derflaaction.h"
#include "alfredworkflowaction.h"

static QMap<QString, DerflaActionType> actionTypeMap{
    { "alfred.workflow.action.script", DAT_SCRIPT },
    { "alfred.workflow.action.openurl", DAT_OPENURL },
    { "alfred.workflow.action.applescript", DAT_APPLESCRIPT },
    { "alfred.workflow.action.systemwebsearch", DAT_SYSTEMWEBSEARCH },
    { "alfred.workflow.action.revealfile", DAT_REVEALFILE },
    { "alfred.workflow.action.terminalcommand", DAT_TERMINALCOMMAND },
    { "alfred.workflow.action.openfile", DAT_OPENFILE },
    { "alfred.workflow.action.browseinalfred", DAT_BROWSEINALFRED },
    { "alfred.workflow.action.lauchfiles", DAT_LAUNCHFILES },
};

AlfredWorkflowAction::AlfredWorkflowAction(QObject *parent) : QObject(parent)
{

}

void AlfredWorkflowAction::parse(const QString& type, const QUuid uid, const QVariantMap& v)
{
    action_ = type;
    uid_ = uid;
    if (v.find("escaping") != v.end())
        escaping_ = v["escaping"].toInt();
    if (v.find("script") != v.end())
        script_ = v["script"].toString();
    if (v.find("url") != v.end())
        url_ = v["url"].toString();
    if (v.find("applescript") != v.end())
        appleScript_ = v["applescript"].toString();
    if (v.find("type") != v.end())
        type_ = v["type"].toInt();
    if (v.find("cachescript") != v.end())
        cacheScript_ = v["cachescript"].toBool();
    if (v.find("plusspaces") != v.end())
        plusSpaces_ = v["plusspaces"].toBool();
    if (v.find("utf8") != v.end())
        utf8_ = v["utf8"].toBool();
}
