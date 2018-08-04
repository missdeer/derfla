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

ExtensionManager::~ExtensionManager()
{
    extensions_.clear();
    prefixExtensionMap_.clear();
}

bool ExtensionManager::loadAllFromLocal()
{
#if defined(Q_OS_MAC)
    QString extensionPath = qApp->applicationDirPath() % "/../Extensions";
#else
    QString extensionPath = qApp->applicationDirPath() % "/extensions";
#endif
    QDir dir(extensionPath);
    if (!dir.exists())
        return false;

    dir.setFilter(QDir::Dirs);
    auto dirs = dir.entryInfoList();

    for (const auto & d : dirs)
    {
        QString filePath = d.absoluteFilePath() % "/extension.cfg";
        QFileInfo fi(filePath);
        if (fi.exists())
        {
            QFile f(filePath);
            if (!f.open(QIODevice::ReadOnly))
            {
                continue;
            }
            QByteArray c = f.readAll();
            f.close();

            QJsonDocument json = QJsonDocument::fromJson(c);
            if (!json.isObject())
            {
                continue;
            }

            QJsonObject o = json.object();
            ExtensionPtr e(new Extension);
            if (o["id"].isString())
                e->setId(o["id"].toString());
            if (o["author"].isString())
                e->setAuthor(o["author"].toString());
            if (o["name"].isString())
                e->setName(o["name"].toString());
            if (o["executable"].isString())
            {
                e->setExecutable(d.absoluteFilePath() % "/" % o["executable"].toString());
#if defined(Q_OS_WIN)
                if (!o["executable"].toString().endsWith(".exe", Qt::CaseInsensitive))
                    e->setExecutable(d.absoluteFilePath() % "/" % o["executable"].toString() % ".exe");
#endif
                if (!QFile::exists(e->executable()))
                    continue;
            }
            if (o["description"].isString())
                e->setDescription(o["description"].toString());
            if (o["executor"].isString())
                e->setExecutor(o["executor"].toString());
            if (o["prefix"].isString())
            {
                e->setPrefix(QStringList() << o["prefix"].toString());
                prefixExtensionMap_.insert(o["prefix"].toString(), e);
            }
            if (o["prefix"].isArray())
            {
                QJsonArray arr = o["prefix"].toArray();
                QStringList prefix;
                for (auto a : arr)
                {
                    if (a.isString())
                    {
                        prefix << a.toString();
                        prefixExtensionMap_.insert(a.toString(), e);
                    }
                }
                e->setPrefix(prefix);
            }
            if (o["prefix"].isNull())
            {
                e->setPrefix(QStringList() << "");
                prefixExtensionMap_.insert("", e);
            }
            if (o["waitIconPath"].isString())
                e->setWaitIconPath(d.absoluteFilePath() % "/" % o["waitIconPath"].toString());
            if (o["waitIconData"].isString())
                e->setWaitIconData(o["waitIconData"].toString());
            if (o["waitTitle"].isString())
                e->setWaitTitle(o["waitTitle"].toString());
            if (o["waitDescription"].isString())
                e->setWaitDescription(o["waitDescription"].toString());
            if (o["daemon"].isBool() && o["daemon"].toBool())
            {
                e->setDaemon(true);
                e->runDaemon();
            }
            extensions_.append(e);
            connect(e.data(), &Extension::queried, this, &ExtensionManager::extensionQueried);
        }
    }

    return true;
}

bool ExtensionManager::loadAllFromCache()
{
    QString extensionsPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) % "/extensions/extensions.cfg";
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
        if (o["id"].isString())
            e->setId(o["id"].toString());
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
        {
            e->setPrefix(QStringList() << o["prefix"].toString());
            prefixExtensionMap_.insert(o["prefix"].toString(), e);
        }
        if (o["prefix"].isArray())
        {
            QJsonArray arr = o["prefix"].toArray();
            QStringList prefix;
            for (auto a : arr)
            {
                if (a.isString())
                {
                    prefix << a.toString();
                    prefixExtensionMap_.insert(a.toString(), e);
                }
            }
            e->setPrefix(prefix);
        }
        if (o["prefix"].isNull())
        {
            e->setPrefix(QStringList() << "");
            prefixExtensionMap_.insert("", e);
        }
        if (o["waitIconPath"].isString())
            e->setWaitIconPath(o["waitIconPath"].toString());
        if (o["waitIconData"].isString())
            e->setWaitIconData(o["waitIconData"].toString());
        if (o["waitTitle"].isString())
            e->setWaitTitle(o["waitTitle"].toString());
        if (o["waitDescription"].isString())
            e->setWaitDescription(o["waitDescription"].toString());
        if (o["daemon"].isBool() && o["daemon"].toBool())
        {
            e->setDaemon(true);
            e->runDaemon();
        }
        extensions_.append(e);
        connect(e.data(), &Extension::queried, this, &ExtensionManager::extensionQueried);
    }

    return true;
}

bool ExtensionManager::installExtension(const QString &extensionFile)
{
    QString savePath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) % "/extensions/" % QUuid::createUuid().toString().mid(1, 36).toLower();
    QDir dir(savePath);
    if (!dir.exists())
    {
        if (!dir.mkpath(savePath))
        {
            QString err = QString("mkpath %1 failed").arg(savePath);
            qDebug() << err;
            throw std::runtime_error(err.toStdString());
            return false;
        }
    }
    // extract
    QZipReader zr(extensionFile);
    bool res = zr.extractAll(savePath);
    if (!res)
    {
        dir.removeRecursively();
        QString err = QString("extracting %1 to %2 failed").arg(extensionFile).arg(savePath);
        qDebug() << err;
        throw std::runtime_error(err.toStdString());
        return false;
    }
    // read configuration file
    QString configPath = savePath % "/extension.cfg";
    QFile f(configPath);

    if (!f.exists())
    {
        dir.removeRecursively();
        QString err = QString("missing extension configuration file: %1").arg(extensionFile);
        throw std::runtime_error(err.toStdString());
        return false;
    }

    if (!f.open(QIODevice::ReadOnly))
    {
        dir.removeRecursively();
        throw std::runtime_error("opening extension configuration file failed");
        return false;
    }
    QByteArray c = f.readAll();
    f.close();

    QJsonDocument json = QJsonDocument::fromJson(c);
    if (!json.isObject())
    {
        dir.removeRecursively();
        throw std::runtime_error("extension configuration is expected to be an object:" + QString(c).toStdString());
        return false;
    }

    QJsonObject o = json.object();
	QVariantMap vm;
    ExtensionPtr e(new Extension);
    if (!o["id"].isString())
    {
        dir.removeRecursively();
        throw std::runtime_error("extension id is required");
        return false;
    }
    e->setId(o["id"].toString());

    auto it = std::find_if(extensions_.begin(), extensions_.end(),
                           [&](ExtensionPtr ep){ return ep->id() == e->id();});
    if (extensions_.end() != it)
    {
        dir.removeRecursively();
        //! \todo extension upgrade
        throw std::runtime_error("duplicated extension id");
        return false;
    }

    vm.insert("id", e->id());
    QString newPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) % "/extensions/" % e->id();
    if (!dir.rename(savePath, newPath))
    {
        dir.removeRecursively();
        throw std::runtime_error(QString("creating directory %1 failed").arg(newPath).toStdString());
        return false;
    }
    savePath = newPath;
    dir.setPath(savePath);

    if (!o["author"].isString())
    {
        dir.removeRecursively();
        throw std::runtime_error("extension author is required");
        return false;
    }
    e->setAuthor(o["author"].toString());
    vm.insert("author", e->author());
    if (!o["name"].isString())
    {
        dir.removeRecursively();
        throw std::runtime_error("extension name is required");
        return false;
    }
    e->setName(o["name"].toString());
    vm.insert("name", e->name());
    if (!o["executable"].isString())
    {
        dir.removeRecursively();
        throw std::runtime_error("extension executable is required");
        return false;
    }
    e->setExecutable(savePath % "/" % o["executable"].toString());
#if defined(Q_OS_WIN)
	if (!o["executable"].toString().endsWith(".exe", Qt::CaseInsensitive))
		e->setExecutable(savePath % "/" % o["executable"].toString() % ".exe");
#endif
    vm.insert("executable", e->executable());
    if (o["description"].isString())
    {
        e->setDescription(o["description"].toString());
        vm.insert("description", e->description());
    }
    if (o["executor"].isString())
    {
        e->setExecutor(o["executor"].toString());
        vm.insert("executor", e->executor());
    }
    if (o["prefix"].isString())
    {
        e->setPrefix(QStringList() << o["prefix"].toString());
        vm.insert("prefix", QStringList() << o["prefix"].toString());
        prefixExtensionMap_.insert(o["prefix"].toString(), e);
    }
    if (o["prefix"].isArray())
    {
        QJsonArray arr = o["prefix"].toArray();
        QStringList prefix;
        for (auto a : arr)
        {
            if (a.isString())
            {
                prefix << a.toString();
                prefixExtensionMap_.insert(a.toString(), e);
            }
        }
        e->setPrefix(prefix);
        vm.insert("prefix", prefix);
    }
    if (o["prefix"].isNull())
    {
        e->setPrefix(QStringList() << "");
        prefixExtensionMap_.insert("", e);
        vm.insert("prefix", "");
    }
    if (o["waitIconPath"].isString() && !o["waitIconPath"].toString().isEmpty())
    {
        QString waitIconPath = savePath % "/" % o["waitIconPath"].toString();
        e->setWaitIconPath(waitIconPath);
        vm.insert("waitIconPath", waitIconPath);
    }
    if (o["waitIconData"].isString())
    {
        e->setWaitIconData(o["waitIconData"].toString());
        vm.insert("waitIconData", o["waitIconData"].toString());
    }
    if (o["waitTitle"].isString())
    {
        e->setWaitTitle(o["waitTitle"].toString());
        vm.insert("waitTitle", e->waitTitle());
    }
    if (o["waitDescription"].isString())
    {
        e->setWaitDescription(o["waitDescription"].toString());
        vm.insert("waitDescription", e->waitDescription());
    }
    if (o["daemon"].isBool() && o["daemon"].toBool())
    {
        e->setDaemon(true);
        e->runDaemon();
    }
    extensions_.append(e);
    connect(e.data(), &Extension::queried, this, &ExtensionManager::extensionQueried);
    // cache
    QString extensionsPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) % "/extension/extensions.cfg";
    f.setFileName(extensionsPath);
	c.clear();
    if (!f.open(QIODevice::ReadOnly))
    {
        qDebug() << "opening cached extensions configuration file failed";
    }
	else
	{
		c = f.readAll();
		f.close();
	}

    if (c.isEmpty())
        json = QJsonDocument::fromJson("[]");
    else
        json = QJsonDocument::fromJson(c);

    Q_ASSERT (json.isArray());
    QJsonArray array = json.array();
    array.append(QJsonObject::fromVariantMap(vm));
	json.setArray(array);
	qDebug() << QString(json.toJson());
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        dir.removeRecursively();
        throw std::runtime_error("opening cached extensions configuration file for writing failed");
        return false;
    }
    f.write(json.toJson());
    f.close();
    return true;
}

void ExtensionManager::extensionQueried(DerflaActionList &dal)
{
    emit actionUpdated(dal);
}

const QList<ExtensionPtr> &ExtensionManager::extensions() const
{
    return extensions_;
}

void ExtensionManager::query(const QString &input)
{
    for (auto e : extensions_)
        e->stopQuery();

    QStringList inputs = input.split(QChar(' '));
    QString prefix;

    if (inputs.length() > 1)
        prefix = inputs.at(0);

    QList<ExtensionPtr> extensions = prefixExtensionMap_.values(prefix);

    for (auto e : extensions)
        e->query(input);

    if (extensions.isEmpty())
        emit emptyAction();
}
