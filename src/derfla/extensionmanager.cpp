#include "stdafx.h"
#include <private/qzipreader_p.h>
#include "extensionmanager.h"

ExtensionManager::ExtensionManager(QObject *parent)
    : QObject(parent)
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QDir d(path);
    if (!d.exists())
    {
        d.mkpath(path);
    }
}

bool ExtensionManager::loadAllFromCache()
{
    QString extensionsPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) % "/extensions.cfg";
    QFile f(extensionsPath);

    if (!f.exists())
    {
        qDebug() << "missing cached extensions configuration:" << extensionsPath;
        return false;
    }

    if (!f.open(QIODevice::ReadOnly))
    {
        qDebug() << "opening cached extensions configuration file failed";
        return false;
    }
    QByteArray c = f.readAll();
    f.close();

    QJsonDocument json = QJsonDocument::fromJson(c);

    if (!json.isArray())
    {
        qDebug() << "cached extensions configuration is expected to be an array";
        return false;
    }

    QJsonArray array = json.array();
    for (auto a : array)
    {
        Q_ASSERT(a.isObject());
        QJsonObject o = a.toObject();
        ExtensionPtr e(new Extension);
        if (o["author"].isString())
            e->setAuthor(o["author"].toString());
        if (o["name"].isString())
            e->setName(o["name"].toString());
        if (o["executable"].isString())
            e->setExecutable(o["executable"].toString());
        if (o["description"].isString())
            e->setDescription(o["description"].toString());
        if (o["executor"].isString())
            e->setExecutor(o["executor"].toString());
        if (o["prefix"].isString())
            e->setPrefix(o["prefix"].toString());
        if (o["waitIconPath"].isString())
            e->setWaitIconPath(o["waitIconPath"].toString());
        if (o["waitIconData"].isString())
            e->setWaitIconData(o["waitIconData"].toString());
        if (o["waitTitle"].isString())
            e->setWaitTitle(o["waitTitle"].toString());
        if (o["waitDescription"].isString())
            e->setWaitDescription(o["waitDescription"].toString());
        extensions_.append(e);
    }

    return true;
}

bool ExtensionManager::getActions(DerflaActionList &dal, const QString &prefix, int countRequired)
{
    return true;
}

bool ExtensionManager::installExtension(const QString &extensionFile)
{
    QString savePath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) % "/" % QUuid::createUuid().toString();
    // extract
    QZipReader zr(extensionFile);
    bool res = zr.extractAll(savePath);
    if (!res)
    {
        qDebug() << "extracting" << extensionFile << "to" << savePath << "failed";
        return false;
    }
    // read configuration file
    QString configPath = savePath % "/extension.cfg";
    QFile f(configPath);

    if (!f.exists())
    {
        qDebug() << "missing extension configuration file:" << extensionFile;
        return false;
    }

    if (!f.open(QIODevice::ReadOnly))
    {
        qDebug() << "opening extension configuration file failed";
        return false;
    }
    QByteArray c = f.readAll();
    f.close();

    QJsonDocument json = QJsonDocument::fromJson(c);
    if (!json.isObject())
    {
        qDebug() << "cached extension configuration is expected to be an object";
        return false;
    }

    QJsonObject o = json.object();
    QJsonObject newObj;
    ExtensionPtr e(new Extension);
    if (o["author"].isString())
    {
        e->setAuthor(o["author"].toString());
        newObj.insert("author", e->author());
    }
    if (o["name"].isString())
    {
        e->setName(o["name"].toString());
        newObj.insert("name", e->name());
    }
    if (o["executable"].isString())
    {
        e->setExecutable(savePath % o["executable"].toString());
        newObj.insert("executable", e->executable());
    }
    if (o["description"].isString())
    {
        e->setDescription(o["description"].toString());
        newObj.insert("description", e->description());
    }
    if (o["executor"].isString())
    {
        e->setExecutor(o["executor"].toString());
        newObj.insert("executor", e->executor());
    }
    if (o["prefix"].isString())
    {
        e->setPrefix(o["prefix"].toString());
        newObj.insert("prefix", e->prefix());
    }
    if (o["waitIconPath"].isString())
    {
        QString waitIconPath = savePath % "/" % o["waitIconPath"].toString();
        e->setWaitIconPath(waitIconPath);
        newObj.insert("waitIconPath", waitIconPath);
    }
    if (o["waitIconData"].isString())
    {
        e->setWaitIconData(o["waitIconData"].toString());
        newObj.insert("waitIconData", o["waitIconData"].toString());
    }
    if (o["waitTitle"].isString())
    {
        e->setWaitTitle(o["waitTitle"].toString());
        newObj.insert("waitTitle", e->waitTitle());
    }
    if (o["waitDescription"].isString())
    {
        e->setWaitDescription(o["waitDescription"].toString());
        newObj.insert("waitDescription", e->waitDescription());
    }
    extensions_.append(e);
    // cache
    QString extensionsPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) % "/extensions.cfg";
    f.setFileName(extensionsPath);

    if (!f.open(QIODevice::ReadOnly))
    {
        qDebug() << "opening cached extensions configuration file failed";
        return false;
    }
    c = f.readAll();
    f.close();

    json = QJsonDocument::fromJson(c);

    Q_ASSERT (json.isArray());
    QJsonArray array = json.array();
    array.append(newObj);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        qDebug() << "opening cached extensions configuration file for writing failed";
        return false;
    }
    f.write(json.toJson());
    f.close();
    return true;
}
