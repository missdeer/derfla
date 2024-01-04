#include "stdafx.h"

#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>

#include "bingdict.h"


BingDict::BingDict(QObject *parent) : QObject(parent) {}

void BingDict::query(const QString &keyword)
{
    // https://cn.bing.com/dict/search?q=
    QUrl      url("https://cn.bing.com/dict/search");
    QUrlQuery query;

    query.addQueryItem("q", keyword.toUtf8());

    url.setQuery(query.query());
    QNetworkRequest req(url);
    req.setAttribute(QNetworkRequest::Http2AllowedAttribute, true);

    QNetworkReply *reply = m_nam.get(req);

    connect(reply, SIGNAL(finished()), this, SLOT(onFinished()));
    connect(reply, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    connect(reply, &QNetworkReply::errorOccurred, this, &BingDict::onError);
#else
    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, &BingDict::onError);
#endif
}

void BingDict::onFinished()
{
    auto *reply = qobject_cast<QNetworkReply *>(sender());
    Q_ASSERT(reply);
    reply->deleteLater();
    m_content.append(reply->readAll());

    QRegularExpression      regex("<meta name=\"description\" content=\"必应词典为您提供.+的释义，(.+)\" ?/>");
    QRegularExpressionMatch match = regex.match(m_content);
    if (match.hasMatch())
    {
        QString definition = match.captured(1);
        int     index      = definition.indexOf(QRegularExpression(" *\\\" */>"));
        if (index > 0)
        {
            definition = definition.mid(0, index);
        }

        QJsonDocument docResp = QJsonDocument::fromJson("[]");
        Q_ASSERT(docResp.isArray());
        QJsonArray arr = docResp.array();

        QByteArray iconData;
        QFile      icon(":/rc/images/bing.png");
        if (icon.open(QIODevice::ReadOnly))
        {
            auto bytes = icon.readAll();
            icon.close();
            iconData = bytes.toBase64();
        }

        int                nLastIndex = 0;
        QRegularExpression regex(R"(((美|英)\[[^\]]+\])，)");
        auto               iter = regex.globalMatch(definition);
        QStringList pronouces;
        while (iter.hasNext())
        {
            auto    match      = iter.next();
            QString definition = match.captured(1);
            pronouces.push_back(definition);
            nLastIndex         = match.capturedEnd(0);
        }
        definition.remove(0, nLastIndex);

        if (!pronouces.isEmpty())
        {
            QVariantMap varMap;
            varMap.insert("title", pronouces.join(QStringLiteral(", ")));
            varMap.insert("target", pronouces.join(QStringLiteral(", ")));
            // varMap.insert("description", QObject::tr("[Explain] %1 [Entry] %2").arg(query, entryStr));
            varMap.insert("actionType", "copyText");
            if (!iconData.isEmpty())
            {
                varMap.insert("iconData", iconData);
            }
            arr.append(QJsonObject::fromVariantMap(varMap));
        }
        QStringList definitions = definition.split("； ");


        for (const auto &definition : definitions)
        {
            QVariantMap varMap;
            varMap.insert("title", definition);
            varMap.insert("target", definition);
            // varMap.insert("description", QObject::tr("[Explain] %1 [Entry] %2").arg(query, entryStr));
            varMap.insert("actionType", "copyText");
            if (!iconData.isEmpty())
            {
                varMap.insert("iconData", iconData);
            }
            arr.append(QJsonObject::fromVariantMap(varMap));
        }
        docResp.setArray(arr);
        QTextStream ts(stdout);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        ts.setCodec("UTF-8");
#else
        ts.setEncoding(QStringConverter::Utf8);
#endif
        ts << QString(docResp.toJson(QJsonDocument::Compact));
    }

    QCoreApplication::exit(0);
}

void BingDict::onError(QNetworkReply::NetworkError e)
{
    auto *reply = qobject_cast<QNetworkReply *>(sender());
    Q_ASSERT(reply);
    qDebug() << e << reply->errorString();
    QCoreApplication::exit(1);
}

void BingDict::onReadyRead()
{
    auto *reply = qobject_cast<QNetworkReply *>(sender());
    Q_ASSERT(reply);
    m_content.append(reply->readAll());
}
