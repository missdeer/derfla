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
        qApp->exit(1);
        return;
    }

    auto o = doc.object();
    auto errorCode = o["errorCode"].toInt();
    if (errorCode != 0)
    {
        qApp->exit(errorCode);
        return;
    }
    auto translation = o["translation"];
    auto basic = o["basic"];
    auto query = o["query"];
    auto web = o["web"];

    qApp->exit(0);
}

void Youdao::onError(QNetworkReply::NetworkError e)
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply);
    qDebug() << e << reply->errorString() ;
    qApp->exit(1);
}

void Youdao::onReadyRead()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply);
    m_content.append( reply->readAll());
}
