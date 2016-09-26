#include "stdafx.h"
#include "processwrapper.h"

ProcessWrapper::ProcessWrapper(QObject *parent)
    : QProcess(parent)
    , uuid_(QUuid::createUuid())
{

}

ProcessWrapper::ProcessWrapper(const QUuid &uuid, QObject *parent)
    : QProcess(parent)
    , uuid_(uuid)
{

}

ProcessWrapper::~ProcessWrapper()
{
    if (state() == QProcess::Running)
    {
        terminate();
    }
}

const QUuid& ProcessWrapper::id() const
{
    return uuid_;
}
