#include "stdafx.h"
#include "alfredworkflowtrigger.h"

AlfredWorkflowTrigger::AlfredWorkflowTrigger(const QString& workingDirectory, QObject *parent) 
    : QObject(parent)
    , workingDirectory_(workingDirectory)
{

}

void AlfredWorkflowTrigger::parse(const QString& type, const QUuid uid, const QVariantMap& v)
{
    typeId_ = type;
    uid_ = uid;
    if (v.find("action") != v.end())
        action_ = v["action"].toInt();
    if (v.find("argument") != v.end())
        argument_ = v["argument"].toInt();
    if (v.find("hotkey") != v.end())
        hotkey_ = v["hotkey"].toInt();
    if (v.find("hotmod") != v.end())
        hotMod_ = v["hotmod"].toInt();
    if (v.find("modsmode") != v.end())
        modsMode_ = v["modsmode"].toInt();
    if (v.find("leftcursor") != v.end())
        leftCursor_ = v["leftcursor"].toBool();
    if (v.find("acceptsmulti") != v.end())
        acceptsMulti_ = v["acceptsmulti"].toBool();
    if (v.find("hotstring") != v.end())
        hotString_ = v["hotstring"].toString();
    if (v.find("name") != v.end())
        name_ = v["name"].toString();
    if (v.find("text") != v.end())
        text_ = v["text"].toString();
    if (v.find("subtext") != v.end())
        subtext_ = v["subtext"].toString();

    auto fileTypes = v.find("filetypes");
    if (fileTypes != v.end())
    {
        QVariantList l = fileTypes->toList();
        std::for_each(l.begin(), l.end(), [this](const QVariant& t) { this->fileTypes_.append(t.toString()); });
    }
}
