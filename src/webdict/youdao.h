#ifndef YOUDAO_H
#define YOUDAO_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class Youdao : public QObject
{
    Q_OBJECT
public:
    explicit Youdao(QObject *parent = nullptr);
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

#endif // YOUDAO_H
