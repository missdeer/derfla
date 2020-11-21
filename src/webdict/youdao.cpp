#include "stdafx.h"
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QUrl>
#include "youdao.h"

Youdao::Youdao(QObject *parent)
    : QObject(parent)
{

}

void Youdao::query(const QString &keyword)
{
    // http://fanyi.youdao.com/openapi.do?keyfrom=f2ec-org&key=1787962561&type=data&doctype=json&version=1.1&q=
    QUrl url("http://fanyi.youdao.com/openapi.do");
    QUrlQuery query;

    query.addQueryItem("keyfrom", "f2ec-org");
    query.addQueryItem("key", "1787962561");
    query.addQueryItem("type", "data");
    query.addQueryItem("doctype", "json");
    query.addQueryItem("version", "1.1");
    query.addQueryItem("q", keyword.toUtf8());

    url.setQuery(query.query());
    QNetworkRequest req(url);
    req.setAttribute(QNetworkRequest::Http2AllowedAttribute, true);

    QNetworkReply* reply = m_nam.get(req);

    connect(reply, SIGNAL(finished()), this, SLOT(onFinished()));
    connect(reply, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(onError(QNetworkReply::NetworkError)));
}

void Youdao::onFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply);
    reply->deleteLater();
    m_content.append( reply->readAll());

    QJsonDocument doc = QJsonDocument::fromJson(m_content);
    if (!doc.isObject())
    {
        QCoreApplication::exit(1);
        return;
    }

    auto o = doc.object();
    auto errorCode = o["errorCode"].toInt();
    if (errorCode != 0)
    {
        QCoreApplication::exit(errorCode);
        return;
    }

    QJsonDocument d = QJsonDocument::fromJson("[]");
    Q_ASSERT(d.isArray());
    QJsonArray arr = d.array();

    auto query = o["query"].toString();
    auto translation = o["translation"].toArray();
    QStringList trans;
    for (auto t : translation)
    {
        trans.append(t.toString());
    }
    auto basic = o["basic"].toObject();
    if (basic["phonetic"].isString())
        trans.append("[" + basic["phonetic"].toString() +"]");
    if (basic["us-phonetic"].isString())
        trans.append("US [" + basic["us-phonetic"].toString() +"]");
    if (basic["uk-phonetic"].isString())
        trans.append("UK [" + basic["uk-phonetic"].toString() +"]");

    QByteArray iconData;
    QFile icon(":/rc/images/youdao.png");
    if (icon.open(QIODevice::ReadOnly))
    {
        auto bytes = icon.readAll();
        icon.close();
        iconData = bytes.toBase64();
    }

    QVariantMap m;
    m.insert("title", trans.join("; ") );
    m.insert("target", trans.join("; ") );
    m.insert("description", QObject::tr("[Translation] ") + query );
    m.insert("actionType", "copyText");
    if (!iconData.isEmpty())
        m.insert("iconData", iconData);
    arr.append(QJsonObject::fromVariantMap(m));

    auto explains = basic["explains"].toArray();
    for (auto e : explains)
    {
        auto i = e.toString();
        QVariantMap m;
        m.insert("title", i );
        m.insert("target", i );
        m.insert("description", QObject::tr("[Explain] ") + query );
        m.insert("actionType", "copyText");
        if (!iconData.isEmpty())
            m.insert("iconData", iconData);
        arr.append(QJsonObject::fromVariantMap(m));
    }

    auto web = o["web"].toArray();
    for (auto w : web)
    {
        auto i = w.toObject();
        QString key = i["key"].toString();
        auto values = i["value"].toArray();
        QStringList value;
        for (auto v : values)
        {
            value.append(v.toString());
        }
        QVariantMap m;
        m.insert("title", value.join("; ") );
        m.insert("target", value.join("; ") );
        m.insert("description", QObject::tr("[Web] ") + key );
        m.insert("actionType", "copyText");
        if (!iconData.isEmpty())
            m.insert("iconData", iconData);
        arr.append(QJsonObject::fromVariantMap(m));
    }

    d.setArray(arr);
    QTextStream ts(stdout);
    ts.setCodec("UTF-8");
    ts << QString(d.toJson(QJsonDocument::Compact));
    QCoreApplication::exit(0);
}

void Youdao::onError(QNetworkReply::NetworkError e)
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply);
    qDebug() << e << reply->errorString() ;
    QCoreApplication::exit(1);
}

void Youdao::onReadyRead()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply);
    m_content.append( reply->readAll());
}
