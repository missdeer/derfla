#ifndef LOCALSOCKET_H
#define LOCALSOCKET_H

#include <QLocalSocket>
#include <QTextStream>

class LocalSocket : public QLocalSocket
{
    Q_OBJECT
public:
    explicit LocalSocket(QTextStream &stream, QObject *parent = nullptr);

    void sendToServer(const QByteArray &data);
private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onErrorOccurred(QLocalSocket::LocalSocketError socketError);
    void onReadChannelFinished();

private:
    bool         connected_ {false};
    QTextStream &stream_;
    QByteArray   msgToWrite_;
    QByteArray   msgRead_;
    void         flushCachedMsg();
};

#endif // LOCALSOCKET_H
