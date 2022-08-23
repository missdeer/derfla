#ifndef HEWEATHER_H
#define HEWEATHER_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>

class Heweather : public QObject
{
    Q_OBJECT
public:
    explicit Heweather(QObject *parent = nullptr);

    void forecast(const QString &location);
signals:

private slots:
    void onFinished();
    void onError(QNetworkReply::NetworkError e);
    void onReadyRead();

private:
    QNetworkAccessManager m_nam;
    QByteArray            m_content;
};

#endif // HEWEATHER_H
