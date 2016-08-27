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
    for (QVariantMap::const_iterator it = connections.constBegin(); connections.constEnd() != it; ++it)
    {
        QVariantList l = it.value().toList();
        QList<QUuid> destinationIds;
        std::for_each(l.begin(), l.end(), 
            [&destinationIds](const QVariant& v) {destinationIds.append(v.toMap()["destinationuid"].toUuid()); });
        connections_.insert(QUuid(it.key()), destinationIds);
    }

    QVariantMap uidata = map["uidata"].toMap();
    QVariantList objects = map["objects"].toList();
    for (const QVariant& obj : objects)
    {
        QVariantMap o = obj.toMap();
        QString type = o["type"].toString();
        QUuid uid(o["uid"].toUuid());

        QVariantMap config = o["config"].toMap();
        if (type.startsWith("alfred.workflow.input."))
        {
            AlfredWorkflowInputPtr awi(new AlfredWorkflowInput(dirName, this));
            awi->parse(type, uid, config);
            inputList_.append(awi);
        }
        else if (type.startsWith("alfred.workflow.output."))
        {
            AlfredWorkflowOutputPtr awo(new AlfredWorkflowOutput(dirName, this));
            awo->parse(type, uid, config);
            outputList_.append(awo);
        }
        else if (type.startsWith("alfred.workflow.action."))
        {
            AlfredWorkflowActionPtr awa(new AlfredWorkflowAction(dirName, this));
            awa->parse(type, uid, config);
            actionList_.append(awa);
        }
        else if (type.startsWith("alfred.workflow.trigger."))
        {
            AlfredWorkflowTriggerPtr awt(new AlfredWorkflowTrigger(dirName, this));
            awt->parse(type, uid, config);
            triggerList_.append(awt);
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
    auto it = std::find_if(inputList_.begin(), inputList_.end(),
        [&keyword](AlfredWorkflowInputPtr awi) {
        return awi->hitKeyword(keyword);
    });
    return inputList_.end() != it;
}

DerflaActionList& AlfredWorkflow::getActions(const QString& input)
{
    derflaActions_.clear();
    for (AlfredWorkflowInputPtr awi : inputList_)
    {
        awi->getDerflaActions(input, derflaActions_);
    }
    return derflaActions_;
}
