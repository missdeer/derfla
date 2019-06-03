#include "stdafx.h"
#include <private/qzipreader_p.h>
#include "luavm.h"
#include "scopedguard.h"
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
    QString extensionPath = QCoreApplication::applicationDirPath() % "/../Extensions";
#else
    QString extensionPath = QCoreApplication::applicationDirPath() % "/extensions";
#endif
    QDir dir(extensionPath);
    if (!dir.exists())
        return false;

    dir.setFilter(QDir::Dirs);
    auto dirs = dir.entryInfoList();

    for (const auto & d : dirs)
    {
        QString filePath = d.absoluteFilePath() % "/extension.derflaext";
        if (QFile::exists(filePath))
        {
            LuaVM vm;
            vm.start();
            ScopedGuard sg([&vm](){vm.shutdown();});
            
            if (!vm.doFile(filePath))
                continue;
            ExtensionPtr e(new Extension);
            e->setId(vm.getString("id"));
            e->setAuthor(vm.getString("author"));
            e->setName(vm.getString("name"));
            QString executable = vm.getString("executable");
            if (!executable.isEmpty())
            {
                e->setExecutable(d.absoluteFilePath() % "/" % executable);
                if (!QFile::exists(e->executable()))
                    continue;
            }
            e->setDescription(vm.getString("description"));
            e->setExecutor(vm.getString("executor"));
            if (!vm.getString("prefix").isEmpty())
            {
                e->setPrefix(QStringList() << vm.getString("prefix"));
                prefixExtensionMap_.insert(vm.getString("prefix"), e);
            }
            else
            {
                QStringList prefixes;
                if (vm.getStringArray("prefix", prefixes))
                {
                    for (const auto & p : prefixes)
                    {
                        prefixExtensionMap_.insert(p, e);
                    }
                    e->setPrefix(prefixes);
                }
                else 
                {
                    e->setPrefix(QStringList() << "");
                    prefixExtensionMap_.insert("", e);
                }
            }
            e->setWaitIconPath(d.absoluteFilePath() % "/" % vm.getString("waitIconPath"));
            e->setWaitIconData(vm.getString("waitIconData"));
            e->setWaitTitle(vm.getString("waitTitle"));
            e->setWaitDescription(vm.getString("waitDescription"));
            if (vm.getBool("daemon"))
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
