#include "stdafx.h"
#include "extensionmanager.h"

ExtensionManager::ExtensionManager(QObject *parent)
    : QObject(parent)
{

}

bool ExtensionManager::loadAllFromCache()
{
    return true;
}

bool ExtensionManager::getActions(DerflaActionList &dal, const QString &prefix, int countRequired)
{
    return true;
}
