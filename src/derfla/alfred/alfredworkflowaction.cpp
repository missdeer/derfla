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

AlfredWorkflowAction::AlfredWorkflowAction(const QString& workingDirectory, QObject *parent) 
    : QObject(parent)
    , workingDirectory_(workingDirectory)
{

}

void AlfredWorkflowAction::getDerflaActions(const QString& input, DerflaActionList& derflaActions)
{
    DerflaActionPtr da(new DerflaAction);
    //da->setTitle(text_);
    QPixmap pixmap;
    pixmap.load(workingDirectory_ % "/icon.png");
    da->setIcon(QIcon(pixmap));

    da->setActionType(*actionTypeMap.find(typeId_));
    // copy
    da->script_ = script_;
    da->appleScript_ = appleScript_;
    da->url_ = url_;
    da->type_ = type_;
    da->escaping_ = escaping_;
    da->utf8_ = utf8_;
    da->plusSpaces_ = plusSpaces_;
    da->cacheScript_ = cacheScript_;
    derflaActions.append(da);
}

void AlfredWorkflowAction::parse(const QString& type, const QUuid uid, const QVariantMap& v)
{
    typeId_ = type;
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