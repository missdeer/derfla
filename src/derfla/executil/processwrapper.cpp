#include "stdafx.h"
#include "processwrapper.h"

ProcessWrapper::ProcessWrapper(QObject *parent)
    : QObject(parent)
    , id_(QUuid::createUuid())
    , process_(new QProcess)
{

}

ProcessWrapper::ProcessWrapper(const QUuid &id, QObject *parent)
    : QObject(parent)
    , id_(id)
    , process_(new QProcess)
{

}

ProcessWrapper::~ProcessWrapper()
{
    if (process_->state() == QProcess::Running)
    {
        process_->terminate();
    }
    delete process_;
}

const QUuid& ProcessWrapper::id() const
{
    return id_;
}

void ProcessWrapper::setWorkingDirectory(const QString &workingDirectory)
{
    process_->setWorkingDirectory(workingDirectory);
}
