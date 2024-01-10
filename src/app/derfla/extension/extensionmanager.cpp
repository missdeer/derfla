#include "stdafx.h"

#include <private/qzipreader_p.h>

#include "extensionmanager.h"
#include "luavm.h"

using std::logical_or;

ExtensionManager::ExtensionManager(QObject *parent) : QObject(parent)
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QDir    dir(path);
    if (!dir.exists())
    {
        dir.mkpath(path);
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
    QString extensionPath = QCoreApplication::applicationDirPath() % "/../Extensions";
#else
    QString extensionPath = QCoreApplication::applicationDirPath() % "/extensions";
#endif
    QDir dir(extensionPath);
    if (!dir.exists())
    {
        return false;
    }

    dir.setFilter(QDir::Dirs);
    auto dirs = dir.entryInfoList();

    for (const auto &dirInfo : dirs)
    {
        QDir    dir(dirInfo.absoluteFilePath());
        QString filePath = dir.absoluteFilePath("extension.derflaext");
        if (!QFile::exists(filePath))
        {
            continue;
        }
        LuaVM luaVM;

        if (!luaVM.doFile(filePath))
        {
            continue;
        }
        ExtensionPtr extension(new Extension);
        extension->setId(luaVM.getQString("id"));
        extension->setAuthor(luaVM.getQString("author"));
        extension->setName(luaVM.getQString("name"));
        QString executable = luaVM.getQString("executable");
        if (!executable.isEmpty())
        {
            extension->setExecutable(dir.absoluteFilePath(executable));
            if (!QFile::exists(extension->executable()))
            {
                continue;
            }
        }
        extension->setDescription(luaVM.getQString("description"));
        extension->setExecutor(luaVM.getQString("executor"));
        if (!luaVM.getQString("prefix").isEmpty())
        {
            extension->setPrefix(QStringList() << luaVM.getQString("prefix"));
            prefixExtensionMap_.insert(luaVM.getQString("prefix"), extension);
        }
        else
        {
            QStringList prefixes;
            if (luaVM.getQStringArray("prefix", prefixes))
            {
                for (const auto &prefix : prefixes)
                {
                    prefixExtensionMap_.insert(prefix, extension);
                }
                extension->setPrefix(prefixes);
            }
            else
            {
                extension->setPrefix(QStringList() << "");
                prefixExtensionMap_.insert("", extension);
            }
        }
        extension->setWaitIconPath(dir.absoluteFilePath(luaVM.getQString("waitIconPath")));
        extension->setWaitIconData(luaVM.getQString("waitIconData"));
        extension->setWaitTitle(luaVM.getQString("waitTitle"));
        extension->setWaitDescription(luaVM.getQString("waitDescription"));
        if (luaVM.getBool("daemon"))
        {
            extension->setDaemon(true);
            extension->runDaemon();
        }
        extensions_.push_back(extension);
        connect(extension.get(), &Extension::queried, this, &ExtensionManager::extensionQueried);
    }

    return true;
}

void ExtensionManager::extensionQueried(DerflaActionList &dal)
{
    emit actionUpdated(dal);
}

const std::vector<ExtensionPtr> &ExtensionManager::extensions() const
{
    return extensions_;
}

void ExtensionManager::query(const QString &input)
{
    for (const auto &extension : extensions_)
    {
        extension->stopQuery();
    }

    QStringList inputs = input.split(QChar(' '));
    QString     prefix;

    if (inputs.length() > 1)
    {
        prefix = inputs.at(0);
    }

    QList<ExtensionPtr> extensions = prefixExtensionMap_.values(prefix);

    if (!extensions.isEmpty())
    {
        for (const auto &extension : extensions)
        {
            extension->query(input);
        }
    }
    else
    {
        bool handled = false;
        for (const auto &extension : extensions_)
        {
            handled |= extension->query(input);
        }
        if (!handled)
        {
            emit emptyAction();
        }
    }
}
