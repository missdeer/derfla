#ifndef DEEPL_H
#define DEEPL_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>

class DeepL : public QObject
{
    Q_OBJECT
public:
    explicit DeepL(QObject *parent = nullptr);
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

#endif // DEEPL_H
