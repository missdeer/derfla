#include "stdafx.h"

#include <QDateTime>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>

#include "deepl.h"

DeepL::DeepL(QNetworkAccessManager *nam, QObject *parent) : QObject(parent), m_nam(nam) {}

void DeepL::query(const QString &keyword)
{
    // curl --location 'https://deeplx.minidump.workers.dev/translate' --header 'Content-Type: application/json' --data '{
    //     "text": "hello world",
    //     "source_lang": "EN",
    //     "target_lang": "ZH"
    // }'

    // Translate request data. support lang code using deepL api
    // DE: German
    // EN: English
    // ES: Spanish
    // FR: French
    // IT: Italian
    // JA: Japanese
    // NL: Dutch
    // PL: Polish
    // PT: Portuguese
    // RU: Russian
    // ZH: Chinese
    // BG: Bulgarian
    // CS: Czech
    // DA: Danish
    // EL: Greek
    // ET: Estonian
    // FI: Finnish
    // HU: Hungarian
    // LT: Lithuanian
    // LV: Latvian
    // RO: Romanian
    // SK: Slovakian
    // SL: Slovenian
    // SV: Swedish
    QUrl            url("https://deeplx.minidump.workers.dev/translate");
    QNetworkRequest req(url);
    req.setAttribute(QNetworkRequest::Http2AllowedAttribute, true);

    QJsonObject json;
    json.insert("text", keyword);
    json.insert("source_lang", "EN");
    json.insert("target_lang", "ZH");

    QByteArray postData = QJsonDocument(json).toJson();

    req.setHeader(QNetworkRequest::ContentLengthHeader, postData.length());
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("Accept", "application/json");
    QNetworkReply *reply = m_nam->post(req, postData);

    connect(reply, SIGNAL(finished()), this, SLOT(onFinished()));
    connect(reply, SIGNAL(readyRead()), this, SLOT(onReadyRead()));

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    connect(reply, &QNetworkReply::errorOccurred, this, &DeepL::onError);
#else
    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, &DeepL::onError);
#endif
}

void DeepL::onFinished()
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

    auto docObj            = doc.object();
    auto text              = docObj["data"].toString();
    auto alternativesArray = docObj["alternatives"].toArray();

    QJsonDocument docResp = QJsonDocument::fromJson("[]");
    Q_ASSERT(docResp.isArray());
    QJsonArray arr = docResp.array();

    QByteArray iconData;
    QFile      icon(":/rc/images/deepl.png");
    if (icon.open(QIODevice::ReadOnly))
    {
        auto bytes = icon.readAll();
        icon.close();
        iconData = bytes.toBase64();
    }

    for (auto alternativeText : alternativesArray)
    {
        auto        explainStr = alternativeText.toString();
        QVariantMap varMap;
        varMap.insert("title", explainStr);
        varMap.insert("target", explainStr);
        varMap.insert("description", QObject::tr("[Explain] %1").arg(text));
        varMap.insert("actionType", "copyText");
        if (!iconData.isEmpty())
        {
            varMap.insert("iconData", iconData);
        }
        arr.append(QJsonObject::fromVariantMap(varMap));
    }

    emit receivedExplain(arr);
}

void DeepL::onError(QNetworkReply::NetworkError e)
{
    auto *reply = qobject_cast<QNetworkReply *>(sender());
    Q_ASSERT(reply);
    qDebug() << e << reply->errorString();
}

void DeepL::onReadyRead()
{
    auto *reply = qobject_cast<QNetworkReply *>(sender());
    Q_ASSERT(reply);
    m_content.append(reply->readAll());
}
