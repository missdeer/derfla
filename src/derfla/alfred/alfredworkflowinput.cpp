#include "stdafx.h"
#include "alfredworkflowinput.h"

AlfredWorkflowInput::AlfredWorkflowInput(const QString& workingDirectory, QObject *parent) 
    : QObject(parent)
    , workingDirectory_(workingDirectory)
{

}

bool AlfredWorkflowInput::hitKeyword(const QString& keyword)
{
    return keywords_.contains(keyword);
}

void AlfredWorkflowInput::getDerflaActions(const QString& input, DerflaActionList& derflaActions)
{
    if (typeId_ == "alfred.workflow.input.keyword")
    {
        // return the text
        DerflaActionPtr da(new DerflaAction);
        da->setTitle(title_);
        QPixmap pixmap;
        pixmap.load(workingDirectory_ % "/icon.png");
        da->setIcon(QIcon(pixmap));
        //da->setActionType(*actionTypeMap.find(action_));
        derflaActions.append(da);
    }
}

void AlfredWorkflowInput::parse(const QString& type, const QUuid uid, const QVariantMap& v)
{
    typeId_ = type;
    uid_ = uid;
    if (v.find("escaping") != v.end())
        escaping_ = v["escaping"].toInt();
    if (v.find("argumenttype") != v.end())
        argumentType_ = v["argumenttype"].toInt();
    if (v.find("keyword") != v.end())
        keywords_.append(v["keyword"].toString());
    if (v.find("title") != v.end())
        title_ = v["title"].toString();
    else if (v.find("text") != v.end())
        text_ = v["text"].toString();
    if (v.find("subtext") != v.end())
        subtext_ = v["subtext"].toString();
    if (v.find("script") != v.end())
        script_ = v["script"].toString();
    if (v.find("runningsubtext") != v.end())
        runningSubtext_ = v["runningsubtext"].toString();
    if (v.find("type") != v.end())
        type_ = v["type"].toInt();
    if (v.find("withspace") != v.end())
        withSpace_ = v["withspace"].toBool();
}
