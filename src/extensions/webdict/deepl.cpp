#include "stdafx.h"

#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>

#include "deepl.h"

#include <QDateTime>
#include <QRandomGenerator>

int get_i_count(const QString &translateText)
{
    return translateText.count('i');
}

int getRandomNumber()
{
    return QRandomGenerator::global()->bounded(100000, 109999) * 1000;
}

qint64 get_timestamp(int iCount)
{
    qint64 ts = QDateTime::currentMSecsSinceEpoch();
    if (iCount != 0)
    {
        iCount += 1;
        return ts - ts % iCount + iCount;
    }

    return ts;
}

DeepL::DeepL(QObject *parent) : QObject(parent) {}

void DeepL::query(const QString &keyword)
{
    // curl -X POST https://www2.deepl.com/jsonrpc \
    //     -H "Content-Type: application/json" \
    //     -d '{
    //         "jsonrpc": "2.0",
    //         "method": "LMT_handle_texts",
    //         "id": 100000,
    //         "params": {
    //             "splitting": "newlines",
    //             "lang": {
    //                 "source_lang_user_selected": "EN",
    //                 "target_lang": "ZH"
    //             },
    //             "texts": [{
    //                 "text": "Hello, world",
    //                 "requestAlternatives": 3
    //             }],
    //             "timestamp": 123456789
    //         }
    //     }'

    QUrl      url("https://www2.deepl.com/jsonrpc");
    QNetworkRequest req(url);
    req.setAttribute(QNetworkRequest::Http2AllowedAttribute, true);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject params;
    params.insert("splitting", "newlines");
    QJsonObject lang;
    lang.insert("source_lang_user_selected", "EN");
    lang.insert("target_lang", "ZH");
    params.insert("lang", lang);

    QJsonObject textObject;
    textObject.insert("text", keyword);
    textObject.insert("requestAlternatives", 3);
    QJsonArray textsArray = {textObject};
    params.insert("texts", textsArray);
    params.insert("timestamp", get_timestamp(get_i_count(keyword)));

    auto id = getRandomNumber();
    QJsonObject json;
    json.insert("jsonrpc", "2.0");
    json.insert("method", "LMT_handle_texts");
    json.insert("id", id);
    json.insert("params", params);

    QByteArray postData = QJsonDocument(json).toJson();

    if ((id + 5) % 29 == 0 || (id + 3) % 13 == 0)
    {
        postData = postData.replace(R"("method":")", R"("method" : ")");
    }
    else
    {
        postData = postData.replace(R"("method":")", R"("method": ")");
    }

    QNetworkReply *reply = m_nam.post(req, postData);

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
        QCoreApplication::exit(1);
        return;
    }

    auto docObj    = doc.object();
    auto resultObj = docObj["result"].toObject();

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

    auto texts = resultObj["texts"].toArray();
    if (texts.isEmpty()) {
        QCoreApplication::exit(-1);
    }
    auto textsZero         = texts.at(0);
    auto textObj           = textsZero.toObject();
    auto alternativesArray = textObj["alternatives"].toArray();
    auto text              = textObj["text"].toString();
    for (auto alternativeText : alternativesArray)
    {
        auto explainObj = alternativeText.toObject();
        auto explainStr = explainObj["text"].toString();
        if (explainStr.isEmpty())
        {
            continue;
        }
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

    docResp.setArray(arr);
    QTextStream ts(stdout);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    ts.setCodec("UTF-8");
#else
    ts.setEncoding(QStringConverter::Utf8);
#endif
    ts << QString(docResp.toJson(QJsonDocument::Compact));
    QCoreApplication::exit(0);
}

void DeepL::onError(QNetworkReply::NetworkError e)
{
    auto *reply = qobject_cast<QNetworkReply *>(sender());
    Q_ASSERT(reply);
    qDebug() << e << reply->errorString();
    QCoreApplication::exit(1);
}

void DeepL::onReadyRead()
{
    auto *reply = qobject_cast<QNetworkReply *>(sender());
    Q_ASSERT(reply);
    m_content.append(reply->readAll());
}
