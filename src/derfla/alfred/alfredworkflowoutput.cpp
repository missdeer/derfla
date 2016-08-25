#include "stdafx.h"
#include "alfredworkflowoutput.h"

AlfredWorkflowOutput::AlfredWorkflowOutput(QObject *parent) : QObject(parent)
{

}

void AlfredWorkflowOutput::parse(const QString& type, const QUuid uid, const QVariantMap& v)
{
    output_ = type;
    uid_ = uid;
}
