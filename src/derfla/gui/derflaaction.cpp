#include "stdafx.h"
#include "derflaaction.h"

DerflaAction::DerflaAction()
    : id_(QUuid::createUuid())
{

}

const QIcon &DerflaAction::icon() const
{
    return icon_;
}

void DerflaAction::setIcon(const QIcon& icon)
{
    icon_ = icon;
}

const QString &DerflaAction::title() const
{
    return title_;
}

void DerflaAction::setTitle(const QString& title)
{
    title_ = title;
}

const QString &DerflaAction::description() const
{
    return description_;
}

void DerflaAction::setDescription(const QString& description)
{
    description_ = description;
}

const QString &DerflaAction::target() const
{
    return target_;
}

void DerflaAction::setTarget(const QString& target)
{
    target_ = target;
}

const QString &DerflaAction::workingDirectory() const
{
    return workingDirectory_;
}

void DerflaAction::setWorkingDirectory(const QString& workingDirectory)
{
    QFileInfo fi(workingDirectory);
    if (fi.isDir())
        workingDirectory_ = workingDirectory;
    else
        workingDirectory_ = fi.absolutePath();
}

const QString &DerflaAction::actionType() const
{
    return actionType_;
}

void DerflaAction::setActionType(const QString &actionType)
{
    actionType_ = actionType;
}

const QString &DerflaAction::arguments() const
{
    return arguments_;
}

void DerflaAction::setArguments(const QString& arguments)
{
    arguments_ = arguments;
}
