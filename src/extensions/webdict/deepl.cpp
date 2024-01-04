#include "stdafx.h"

#include <chrono>
#include <cmath>
#include <random>

#include <QDateTime>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>

#include "deepl.h"

int64_t generateNextId()
{
    auto            seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937_64 randProvider(seed);

    int64_t randomNumber = randProvider();

    int64_t sqrtValue = static_cast<int64_t>(std::sqrt(10000));

    int64_t nextId = sqrtValue * randomNumber;

    return nextId;
}

int get_i_count(const QString &translateText)
{
    return translateText.count('i');
}

qint64 get_timestamp(int iCount)
{
    qint64 ts = QDateTime::currentMSecsSinceEpoch();
    if (iCount != 0)
    {
        iCount++;
        return ts - ts % iCount + iCount;
    }

    return ts;
}

DeepL::DeepL(QNetworkAccessManager *nam, QObject *parent) : QObject(parent), m_nam(nam) {}

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
    QUrl            url("https://www2.deepl.com/jsonrpc");
    QNetworkRequest req(url);
    req.setAttribute(QNetworkRequest::Http2AllowedAttribute, true);

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

    auto        id = generateNextId();
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

    req.setHeader(QNetworkRequest::ContentLengthHeader, postData.length());
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("Referer", "https://www.deepl.com");
    req.setRawHeader("Accept", "*/*");
    req.setRawHeader("x-app-os-name", "iOS");
    req.setRawHeader("x-app-os-version", "16.3.0");
    req.setRawHeader("Accept-Language", "en-US,en;q=0.9");
    req.setRawHeader("x-app-device", "iPhone13,2");
    req.setRawHeader("User-Agent", "DeepL-iOS/2.9.1 iOS 16.3.0 (iPhone13,2)");
    req.setRawHeader("x-app-build", "510265");
    req.setRawHeader("x-app-version", "2.9.1");
    req.setRawHeader("Connection", "keep-alive");
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
    if (texts.isEmpty())
    {
        emit emptyExplain();
        return;
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
