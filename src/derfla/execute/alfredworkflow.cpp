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
        qWarning() << path << "doesn't exist";
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
        qWarning() << "extracting" << path << "to" << dirName << "failed";
        return false;
    }
    return loadFromDirectory(dirName);
}

bool AlfredWorkflow::loadFromDirectory(const QString &dirName)
{
    QString plistName = dirName % "/info.plist";

    if (!QFile::exists(plistName))
    {
        qWarning() << plistName << "doesn't exist";
        return false;
    }

    QFile *f = new QFile(plistName);
    if (!f->open(QIODevice::ReadOnly))
    {
        qWarning() << "can't open" << plistName;
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
            input_ = type;
            inputEscaping_ = config["escaping"].toInt();
            inputKeywords_.append(config["keyword"].toString());
            if (config.find("title") != config.end())
                inputTitle_ = config["title"].toString();
            else if (config.find("text") != config.end())
                inputTitle_ = config["text"].toString();
            if (config.find("subtext") != config.end())
                inputSubtext_ = config["subtext"].toString();
            if (config.find("script") != config.end())
                inputScript_ = config["script"].toString();
            if (config.find("runningsubtext") != config.end())
                inputRunningSubtext_ = config["runningsubtext"].toString();
            if (config.find("type") != config.end())
                inputType_ = config["type"].toInt();
            if (config.find("withspace") != config.end())
                inputWithSpace_ = config["withspace"].toBool();
        }
        else if (type.startsWith("alfred.workflow.output."))
        {
            output_ = type;
        }
        else if (type.startsWith("alfred.workflow.action."))
        {
            action_ = type;
            actionEscaping_ = config["escaping"].toInt();
            if (config.find("script") != config.end())
                actionScript_ = config["script"].toString();
            if (config.find("type") != config.end())
                actionType_ = config["type"].toInt();            
        }
    }

    installDirectory_ = dirName;
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
    return inputKeywords_.contains(keyword.split(' ').at(0));
}

DerflaActionList& AlfredWorkflow::getActions(const QString& input)
{
    action_.clear();
    if (hitKeyword(input))
    {
        if (input_ == "alfred.workflow.input.keyword")
        {
            // return the text
            DerflaActionPtr da(new DerflaAction);
            da->setTitle(inputTitle_);
            QPixmap pixmap;
            pixmap.load(installDirectory_ % "/icon.png");
            da->setIcon(QIcon(pixmap));
            actions_.append(da);
        }
    }
    return actions_;
}

bool AlfredWorkflow::triggerAction(int index)
{
    return false;
}
