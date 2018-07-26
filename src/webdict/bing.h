#ifndef BING_H
#define BING_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>


class Bing : public QObject
{
    Q_OBJECT
public:
    explicit Bing(QObject *parent = nullptr);
    void query(const QString& keyword);
signals:

private slots:
    void onFinished();
    void onError(QNetworkReply::NetworkError e);
    void onReadyRead();
private:
    QNetworkAccessManager m_nam;
    QByteArray m_content;
};

#endif // BING_H
