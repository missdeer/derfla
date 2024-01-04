#include "stdafx.h"

#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>

#include "youdao.h"

Youdao::Youdao(QNetworkAccessManager *nam, QObject *parent) : QObject(parent), m_nam(nam) {}

void Youdao::query(const QString &keyword)
{
    // https://dict.youdao.com/suggest?num=5&ver=3.0&doctype=json&cache=false&le=en&q=
    QUrl      url("https://dict.youdao.com/suggest");
    QUrlQuery query;

    query.addQueryItem("num", "5");
    query.addQueryItem("ver", "3.0");
    query.addQueryItem("cache", "false");
    query.addQueryItem("doctype", "json");
    query.addQueryItem("le", "en");
    query.addQueryItem("q", keyword.toUtf8());

    url.setQuery(query.query());
    QNetworkRequest req(url);
    req.setAttribute(QNetworkRequest::Http2AllowedAttribute, true);

    QNetworkReply *reply = m_nam->get(req);

    connect(reply, SIGNAL(finished()), this, SLOT(onFinished()));
    connect(reply, SIGNAL(readyRead()), this, SLOT(onReadyRead()));

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    connect(reply, &QNetworkReply::errorOccurred, this, &Youdao::onError);
#else
    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, &Youdao::onError);
#endif
}

void Youdao::onFinished()
{
    auto *reply = qobject_cast<QNetworkReply *>(sender());
    Q_ASSERT(reply);
    reply->deleteLater();
    m_content.append(reply->readAll());

    QJsonDocument doc = QJsonDocument::fromJson(m_content);
    if (!doc.isObject())
    {
        emit emptyExplain();
        return;
    }

    auto docObj    = doc.object();
    auto resultObj = docObj["result"].toObject();
    auto errorCode = resultObj["code"].toInt();
    if (errorCode != 200)
    {
        emit emptyExplain();
        return;
    }

    QJsonDocument docResp = QJsonDocument::fromJson("[]");
    Q_ASSERT(docResp.isArray());
    QJsonArray arr = docResp.array();

    auto dataObj        = docObj["data"].toObject();
    auto query          = dataObj["query"].toString();
    auto translationArr = dataObj["entries"].toArray();

    QByteArray iconData;
    QFile      icon(":/rc/images/youdao.png");
    if (icon.open(QIODevice::ReadOnly))
    {
        auto bytes = icon.readAll();
        icon.close();
        iconData = bytes.toBase64();
    }

    auto explains = dataObj["entries"].toArray();
    for (auto explain : explains)
    {
        auto explainObj = explain.toObject();
        auto explainStr = explainObj["explain"].toString();
        if (explainStr.isEmpty())
        {
            continue;
        }
        auto        entryStr = explainObj["entry"].toString();
        QVariantMap varMap;
        varMap.insert("title", explainStr);
        varMap.insert("target", explainStr);
        varMap.insert("description", QObject::tr("[Explain] %1 [Entry] %2").arg(query, entryStr));
        varMap.insert("actionType", "copyText");
        if (!iconData.isEmpty())
        {
            varMap.insert("iconData", iconData);
        }
        arr.append(QJsonObject::fromVariantMap(varMap));
    }
    emit receivedExplain(arr);
}

void Youdao::onError(QNetworkReply::NetworkError e)
{
    auto *reply = qobject_cast<QNetworkReply *>(sender());
    Q_ASSERT(reply);
    qDebug() << e << reply->errorString();
}

void Youdao::onReadyRead()
{
    auto *reply = qobject_cast<QNetworkReply *>(sender());
    Q_ASSERT(reply);
    m_content.append(reply->readAll());
}
