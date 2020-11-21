#include "stdafx.h"
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QUrl>
#include "bing.h"

Bing::Bing(QObject *parent) : QObject(parent)
{

}

void Bing::query(const QString &keyword)
{
    // http://xtk.azurewebsites.net/BingDictService.aspx?Word=
    QUrl url("http://xtk.azurewebsites.net/BingDictService.aspx");
    QUrlQuery query;

    query.addQueryItem("Word", keyword.toUtf8());

    url.setQuery(query.query());
    QNetworkRequest req(url);
    req.setAttribute(QNetworkRequest::Http2AllowedAttribute, true);

    QNetworkReply* reply = m_nam.get(req);

    connect(reply, SIGNAL(finished()), this, SLOT(onFinished()));
    connect(reply, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(onError(QNetworkReply::NetworkError)));
}

void Bing::onFinished()
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

    QJsonDocument d = QJsonDocument::fromJson("[]");
    Q_ASSERT(d.isArray());
    QJsonArray arr = d.array();

    auto o = doc.object();
    auto query = o["word"].toString();

    QByteArray iconData;
    QFile icon(":/rc/images/bing.png");
    if (icon.open(QIODevice::ReadOnly))
    {
        auto bytes = icon.readAll();
        icon.close();
        iconData = bytes.toBase64();
    }

    auto pronunciation = o["pronunciation"].toObject();
    QStringList pron;
    auto us = pronunciation["AmE"];
    if (!us.isNull())
        pron.append("US [" + us.toString() + "]");
    auto uk = pronunciation["BrE"];
    if (!uk.isNull())
        pron.append("UK [" + uk.toString() + "]");
    if (!pron.isEmpty())
    {
        QVariantMap m;
        m.insert("title", pron.join("; ") );
        m.insert("target", pron.join("; ") );
        m.insert("description", QObject::tr("[Pronounce] ") + query );
        m.insert("actionType", "copyText");
        if (!iconData.isEmpty())
            m.insert("iconData", iconData);
        arr.append(QJsonObject::fromVariantMap(m));
    }

    auto defs = o["defs"].toArray();
    for (auto def : defs)
    {
        auto d = def.toObject();
        QString pos = d["pos"].toString();
        QString define = d["def"].toString();

        QVariantMap m;
        m.insert("title", pos + " " + define );
        m.insert("target", pos + " " + define );
        m.insert("description", "[Explain] " + query );
        m.insert("actionType", "copyText");
        if (!iconData.isEmpty())
            m.insert("iconData", iconData);
        arr.append(QJsonObject::fromVariantMap(m));
    }
    auto sams = o["sams"].toArray();
    for (auto sam : sams)
    {
        auto s = sam.toObject();
        QString eng = s["eng"].toString();
        QString chn = s["chn"].toString();

        QVariantMap m;
        m.insert("title", chn );
        m.insert("target", chn );
        m.insert("description", QObject::tr("[Sample] ") + eng );
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

void Bing::onError(QNetworkReply::NetworkError e)
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply);
    qDebug() << e << reply->errorString() ;
    QCoreApplication::exit(1);
}

void Bing::onReadyRead()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply);
    m_content.append( reply->readAll());
}
