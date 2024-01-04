#ifndef BINGDICT_H
#define BINGDICT_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>

class BingDict : public QObject
{
    Q_OBJECT
public:
    explicit BingDict(QObject *parent = nullptr);
    void query(const QString &keyword);
signals:

private slots:
    void onFinished();
    void onError(QNetworkReply::NetworkError e);
    void onReadyRead();

private:
    QNetworkAccessManager m_nam;
    QByteArray            m_content;
};

#endif // BINGDICT_H
