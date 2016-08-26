#include "stdafx.h"
#include "alfredworkflowinput.h"

AlfredWorkflowInput::AlfredWorkflowInput(const QString& workingDirectory, QObject *parent) 
    : QObject(parent)
    , workingDirectory_(workingDirectory)
{

}

bool AlfredWorkflowInput::hitKeyword(const QString& keyword)
{
    return keyword_ == keyword;
}

void AlfredWorkflowInput::getDerflaActions(const QString& input, DerflaActionList& derflaActions)
{
    if (typeId_ == "alfred.workflow.input.keyword")
    {
        // return the text
        DerflaActionPtr da(new DerflaAction);
        da->setTitle(text_);
        da->setDescription(subtext_);
        QPixmap pixmap;
        pixmap.load(workingDirectory_ % "/icon.png");
        da->setIcon(QIcon(pixmap));
        //da->setActionType(*actionTypeMap.find(action_));
        derflaActions.append(da);
    }
    else if (typeId_ == "alfred.workflow.input.scriptfilter")
    {

    }
    else if (typeId_ == "alfred.workflow.input.filefilter")
    {

    }
    else
    {
        qWarning() << "unsupported workflow";
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
        keyword_ = v["keyword"].toString();
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
    if (v.find("anchorfields") != v.end())
        anchorFields_ = v["anchorfields"].toBool();
    if (v.find("daterange") != v.end())
        dateRange_ = v["daterange"].toInt();
    if (v.find("includesystem") != v.end())
        includeSystem_ = v["includesystem"].toBool();
    auto scopes = v.find("scopes");
    if (scopes != v.end())
    {
        QVariantList l = scopes->toList();
        std::for_each (l.begin(), l.end(), [this](const QVariant& s) { this->scopes_.append(s.toString()); });
    }
    auto types = v.find("types");
    if (types != v.end())
    {
        QVariantList l = types->toList();
        std::for_each(l.begin(), l.end(), [this](const QVariant& t) { this->types_.append(t.toString()); });
    }
    auto fields = v.find("fields");
    if (fields != v.end())
    {
        QVariantList l = fields->toList();
        std::for_each(l.begin(), l.end(), 
            [this](const QVariant& t) { 
            AlfredWorkflowInputFieldPtr awif(new AlfredWorkflowInputField);
            QVariantMap m = t.toMap();
            awif->not_ = m["not"].toBool();
            awif->split_ = m["split"].toBool();
            awif->words_ = m["words"].toBool();
            awif->field_ = m["field"].toString();
            awif->value_ = m["value"].toString();
            this->fields_.append(awif);
        });
    }
}
