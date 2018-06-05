#include "stdafx.h"
#include "LocalFSItem.h"

LocalFSItem::LocalFSItem(QObject *parent)
    : QObject(parent)
    , actionType_(FSIT_UNKNOWN)
    , id_(QUuid::createUuid())
{

}

const QIcon &LocalFSItem::icon() const
{
    return icon_;
}

void LocalFSItem::setIcon(const QIcon& icon)
{
    icon_ = icon;
}

const QString &LocalFSItem::title() const
{
    return title_;
}

void LocalFSItem::setTitle(const QString& title)
{
    title_ = title;
}

const QString &LocalFSItem::description() const
{
    return description_;
}

void LocalFSItem::setDescription(const QString& description)
{
    description_ = description;
}

const QString &LocalFSItem::target() const
{
    return target_;
}

void LocalFSItem::setTarget(const QString& target)
{
    target_ = target;
}

const QString &LocalFSItem::workingDirectory() const
{
    return workingDirectory_;
}

void LocalFSItem::setWorkingDirectory(const QString& workingDirectory)
{
    QFileInfo fi(workingDirectory);
    if (fi.isDir())
        workingDirectory_ = workingDirectory;
    else
        workingDirectory_ = fi.absolutePath();
}

const QString &LocalFSItem::actionType() const
{
    return actionType_;
}

void LocalFSItem::setActionType(const QString &actionType)
{
    actionType_ = actionType;
}

const QString &LocalFSItem::arguments() const
{
    return arguments_;
}

void LocalFSItem::setArguments(const QString& arguments)
{
    arguments_ = arguments;
}
