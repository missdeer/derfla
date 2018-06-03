#include "stdafx.h"
#include "derflaaction.h"

DerflaAction::DerflaAction(QObject *parent)
    : QObject(parent)
    , actionType_(DAT_UNKNOWN)
    , id_(QUuid::createUuid())
{

}

bool DerflaAction::run()
{
    return run_(this);
}

void DerflaAction::setRunner(const std::function<bool (DerflaAction *)> &run)
{
    run_ = run;
}

QIcon DerflaAction::icon() const
{
    return icon_;
}

void DerflaAction::setIcon(const QIcon& icon)
{
    icon_ = icon;
}

QString DerflaAction::title() const
{
    return title_;
}

void DerflaAction::setTitle(const QString& title)
{
    title_ = title;
}

QString DerflaAction::description() const
{
    return description_;
}

void DerflaAction::setDescription(const QString& description)
{
    description_ = description;
}

QString DerflaAction::target() const
{
    return target_;
}

void DerflaAction::setTarget(const QString& target)
{
    target_ = target;
}

QString DerflaAction::workingDirectory() const
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

DerflaActionType DerflaAction::actionType() const
{
    return actionType_;
}

void DerflaAction::setActionType(const DerflaActionType& actionType)
{
    actionType_ = actionType;
}

QString DerflaAction::arguments() const
{
    return arguments_;
}

void DerflaAction::setArguments(const QString& arguments)
{
    arguments_ = arguments;
}
