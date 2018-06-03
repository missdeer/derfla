#include "stdafx.h"
#include "LocalFSItem.h"

LocalFSItem::LocalFSItem(QObject *parent)
    : QObject(parent)
    , actionType_(DAT_UNKNOWN)
    , id_(QUuid::createUuid())
{

}

QIcon LocalFSItem::icon() const
{
    return icon_;
}

void LocalFSItem::setIcon(const QIcon& icon)
{
    icon_ = icon;
}

QString LocalFSItem::title() const
{
    return title_;
}

void LocalFSItem::setTitle(const QString& title)
{
    title_ = title;
}

QString LocalFSItem::description() const
{
    return description_;
}

void LocalFSItem::setDescription(const QString& description)
{
    description_ = description;
}

QString LocalFSItem::target() const
{
    return target_;
}

void LocalFSItem::setTarget(const QString& target)
{
    target_ = target;
}

QString LocalFSItem::workingDirectory() const
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

LocalFSItemType LocalFSItem::actionType() const
{
    return actionType_;
}

void LocalFSItem::setActionType(const LocalFSItemType& actionType)
{
    actionType_ = actionType;
}

QString LocalFSItem::arguments() const
{
    return arguments_;
}

void LocalFSItem::setArguments(const QString& arguments)
{
    arguments_ = arguments;
}
