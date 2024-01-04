#include "stdafx.h"

#include <QJsonDocument>
#include <QTextStream>

#include "responser.h"


Responser::Responser(QObject *parent) : QObject(parent) {}

void Responser::receivedExplain(QJsonArray arr)
{
    for (auto ele : arr)
    {
        m_replied.append(ele);
    }
    checkResult();
}

void Responser::emptyExplain()
{
    checkResult();
}

void Responser::increaseService()
{
    m_serviceCount++;
}
void Responser::checkResult()
{
    m_serviceReplied++;
    if (m_serviceReplied == m_serviceCount)
    {
        QJsonDocument docResp = QJsonDocument::fromJson("[]");
        Q_ASSERT(docResp.isArray());

        docResp.setArray(m_replied);
        QTextStream ts(stdout);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        ts.setCodec("UTF-8");
#else
        ts.setEncoding(QStringConverter::Utf8);
#endif
        ts << QString(docResp.toJson(QJsonDocument::Compact));
        QCoreApplication::exit(0);
    }
}

bool Responser::hasService() const
{
    return m_serviceCount != 0;
}
