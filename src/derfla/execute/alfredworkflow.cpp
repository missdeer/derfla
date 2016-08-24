#include "stdafx.h"
#include <JlCompress.h>
#include <plistparser.h>
#include "alfredworkflow.h"

AlfredWorkflow::AlfredWorkflow(QObject *parent) : QObject(parent)
{

}

bool AlfredWorkflow::installFromBundle(const QString &path)
{
    if (!QFile::exists(path))
    {
        //qDebug() << path << "doesn't exist";
        return false;
    }

    QFileInfo fi(path);
    QString dirName = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) % "/alfredworkflow/" % fi.completeBaseName();
    QDir d(dirName);
    if (!d.exists())
    {
        d.mkpath(dirName);
    }
    // extract
    QStringList files = JlCompress::extractDir(path, dirName);
    if (files.isEmpty())
    {
        //qDebug() << "extracting" << path << "to" << dirName << "failed";
        return false;
    }
    return loadFromDirectory(dirName);
}

bool AlfredWorkflow::loadFromDirectory(const QString &dirName)
{
    QString plistName = dirName % "/info.plist";

    if (!QFile::exists(plistName))
    {
        //qDebug() << plistName << "doesn't exist";
        return false;
    }

    QFile *f = new QFile(plistName);
    if (!f->open(QIODevice::ReadOnly))
    {
        //qDebug() << "can't open" << plistName;
        return false;
    }

    BOOST_SCOPE_EXIT(f) {
        f->close();
        delete f;
    } BOOST_SCOPE_EXIT_END

    auto v = PListParser::parsePList(f);
    QVariantMap map = v.toMap();
    bundleId_ = map["bundleid"].toString();
    category_ = map["category"].toString();
    author_ = map["createdby"].toString();
    description_ = map["description"].toString();
    disabled_ = map["disabled"].toBool();
    name_ = map["name"].toString();
    readme_ = map["readme"].toString();
    webaddress_ = map["webaddress"].toString();

    QVariantMap connections = map["connections"].toMap();
    QVariantMap uidata = map["uidata"].toMap();
    QVariantList objects = map["objects"].toList();
    for (QVariant obj : objects)
    {
        QVariantMap o = obj.toMap();
        QString type = o["type"].toString();
        //qDebug() << "type:" << type;

        QVariantMap config = o["config"].toMap();
        //qDebug() << "config:" << config;
        if (type.startsWith("alfred.workflow.input."))
        {
            keywords_.append(config["keyword"].toString());
            inputTitle_ = config["title"].toString();
            inputSubtext_ = config["subtext"].toString();
            inputType_ = config["type"].toInt();
            inputWithSpace_ = config["withspace"].toBool();
        }
        else if (type.startsWith("alfred.workflow.output."))
        {

        }
        else if (type.startsWith("alfred.workflow.action."))
        {

        }
    }
    return true;
}

const QString &AlfredWorkflow::bundleId() const
{
    return bundleId_;
}

const QString& AlfredWorkflow::category() const
{
    return category_;
}

const QString& AlfredWorkflow::author() const
{
    return author_;
}

const QString& AlfredWorkflow::description() const
{
    return description_;
}

const QString& AlfredWorkflow::readme() const
{
    return readme_;
}

const QString& AlfredWorkflow::name() const
{
    return name_;
}

const QString& AlfredWorkflow::webaddress() const
{
    return webaddress_;
}

bool AlfredWorkflow::disabled() const
{
    return disabled_;
}

bool AlfredWorkflow::hitKeyword(const QString &keyword)
{
    return keywords_.contains(keyword.split(' ').at(0));
}

DerflaActionList& AlfredWorkflow::getActions(const QString& input)
{
    return actions_;
}

bool AlfredWorkflow::triggerAction(int index)
{
    return false;
}
