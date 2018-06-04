#include "stdafx.h"
#include "extension.h"

Extension::Extension(QObject *parent) 
	: QObject(parent)
{

}

bool Extension::load(const QString& configuration)
{
	return true;
}

bool Extension::query(const QString& prefix)
{
	return true;
}

bool Extension::run(DerflaAction* action)
{
	return true;
}

const QString &Extension::author() const
{
    return author_;
}

void Extension::setAuthor(const QString &author)
{
    author_ = author;
}

const QString &Extension::name() const
{
    return name_;
}

void Extension::setName(const QString &name)
{
    name_ = name;
}

const QString &Extension::description() const
{
    return description_;
}

void Extension::setDescription(const QString &description)
{
    description_ = description;
}

const QString &Extension::executable() const
{
    return executable_;
}

void Extension::setExecutable(const QString &executable)
{
    executable_ = executable;
}

const QString &Extension::executor() const
{
    return executor_;
}

void Extension::setExecutor(const QString &executor)
{
    executor_ = executor;
}

const QString &Extension::prefix() const
{
    return prefix_;
}

void Extension::setPrefix(const QString &prefix)
{
    prefix_ = prefix;
}

const QString &Extension::waitTitle() const
{
    return waitTitle_;
}

void Extension::setWaitTitle(const QString &waitTitle)
{
    waitTitle_ = waitTitle;
}

const QString &Extension::waitDescription() const
{
    return waitDescription_;
}

void Extension::setWaitDescription(const QString &waitDescription)
{
    waitDescription_ = waitDescription;
}

const QIcon &Extension::waitIcon() const
{
    return waitIcon_;
}

void Extension::setWaitIcon(const QIcon &waitIcon)
{
    waitIcon_ = waitIcon;
}

void Extension::setWaitIconPath(const QString &waitIconPath)
{
    QPixmap pixmap;
    if (pixmap.load(waitIconPath))
    {
        waitIcon_ = QIcon(pixmap);
    }
}

void Extension::setWaitIconData(const QString &waitIconData)
{
    QByteArray c = QByteArray::fromBase64(waitIconData.toUtf8());
    QPixmap pixmap;
    if (pixmap.loadFromData(c))
    {
        waitIcon_ = QIcon(pixmap);
    }
}
