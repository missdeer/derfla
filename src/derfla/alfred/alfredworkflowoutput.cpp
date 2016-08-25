#include "stdafx.h"
#include "alfredworkflowoutput.h"

AlfredWorkflowOutput::AlfredWorkflowOutput(const QString& workingDirectory, QObject *parent) 
    : QObject(parent)
    , workingDirectory_(workingDirectory)
{

}

void AlfredWorkflowOutput::parse(const QString& type, const QUuid uid, const QVariantMap& v)
{
    typeId_ = type;
    uid_ = uid;
    if (v.find("escaping") != v.end())
        escaping_ = v["escaping"].toInt();
    if (v.find("lastpathcomponent") != v.end())
        lastPathComponent_ = v["lastpathcomponent"].toBool();
    if (v.find("onlyshowifquerypopulated") != v.end())
        onlyShowIfQueryPopulated_ = v["onlyshowifquerypopulated"].toBool();
    if (v.find("removeextension") != v.end())
        removeExtension_ = v["removeextension"].toBool();
    if (v.find("sticky") != v.end())
        sticky_ = v["sticky"].toBool();
    if (v.find("autopaste") != v.end())
        autoPaste_ = v["autopaste"].toBool();
    if (v.find("output") != v.end())
        output_ = v["output"].toInt();
    if (v.find("type") != v.end())
        type_ = v["type"].toInt();
    if (v.find("script") != v.end())
        script_ = v["script"].toString();
    if (v.find("text") != v.end())
        text_ = v["text"].toString();
    if (v.find("title") != v.end())
        title_ = v["title"].toString();
    if (v.find("clipboardtext") != v.end())
        clipboardText_ = v["clipboardtext"].toString();
}
