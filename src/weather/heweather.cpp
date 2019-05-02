#include "stdafx.h"
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QUrl>
#include "heweather.h"

Heweather::Heweather(QObject *parent) : QObject(parent)
{

}

void Heweather::forecast(const QString &location)
{
    // https://free-api.heweather.net/s6/weather/forecast?parameters
    QUrl url("https://free-api.heweather.net/s6/weather/forecast");
    QUrlQuery query;

    query.addQueryItem("location", location.toUtf8());
    query.addQueryItem("key", "b2063000cdee4cdc8f7b0ae59f682799");

    url.setQuery(query.query());
    QNetworkRequest req(url);

    QNetworkReply* reply = m_nam.get(req);

    connect(reply, SIGNAL(finished()), this, SLOT(onFinished()));
    connect(reply, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(onError(QNetworkReply::NetworkError)));
}

void Heweather::onFinished()
{
    auto* reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply);
    reply->deleteLater();
    m_content.append( reply->readAll());

    QJsonDocument doc = QJsonDocument::fromJson(m_content);
    if (!doc.isObject())
    {
        qApp->exit(1);
        return;
    }

    QJsonDocument d = QJsonDocument::fromJson("[]");
    Q_ASSERT(d.isArray());
    QJsonArray arr = d.array();

    auto o = doc.object();

    QTextStream ts(stdout);
    ts.setCodec("UTF-8");
    auto w = o["HeWeather6"];
    if (!w.isArray())
    {
        ts << "no root node";
        qApp->exit(2);
    }

    auto weather = w.toArray();
    auto weatherItem = weather[0];
    if (!weatherItem.isObject())
    {
        ts << "invalid first array element";
        qApp->exit(3);
    }

    auto item = weatherItem.toObject();

    auto status = item["status"].toString();
    if (status != "ok")
    {
        ts << "status is " << status;
        qApp->exit(4);
    }
    auto basic = item["basic"].toObject();
    QString location = basic["location"].toString();

    auto dailyForecast = item["daily_forecast"].toArray();
    for (auto f : dailyForecast)
    {
        if (!f.isObject())
            continue;
        auto forecast = f.toObject();

        QVariantMap m;
        QString title = QString(tr("Day: %1, Night: %2, Tempe: %3℃~%4℃")).arg(forecast["cond_txt_d"].toString(),
                forecast["cond_txt_n"].toString(),
                forecast["tmp_min"].toString(),
                forecast["tmp_max"].toString());
        m.insert("title", title );
        m.insert("target", title );

        QString description = QString(tr("%1 %2 %3 %4L"))
                .arg(forecast["date"].toString(),
                location,
                forecast["wind_dir"].toString(),
                forecast["wind_sc"].toString());
        m.insert("description", description);
        m.insert("actionType", "copyText");

        QFile icon(QString(":/rc/images/%1.png").arg(forecast["cond_code_d"].toString()));
        if (icon.open(QIODevice::ReadOnly))
        {
            auto bytes = icon.readAll();
            icon.close();
            m.insert("iconData", bytes.toBase64());
        }
        arr.append(QJsonObject::fromVariantMap(m));
    }

    d.setArray(arr);
    ts << QString(d.toJson(QJsonDocument::Compact));
    qApp->exit(0);
}

void Heweather::onError(QNetworkReply::NetworkError e)
{
    auto* reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply);
    qDebug() << e << reply->errorString() ;
    qApp->exit(1);
}

void Heweather::onReadyRead()
{
    auto* reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply);
    m_content.append( reply->readAll());
}
