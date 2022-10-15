#ifndef LOCALSERVER_H
#define LOCALSERVER_H

#include <QLocalServer>
#include <QLocalSocket>

class DBRW;

class LocalServer : public QLocalServer
{
    Q_OBJECT
public:
    explicit LocalServer(DBRW &dbrw, QObject *parent = nullptr);

private slots:
    void onNewConnection();
    void onLocalSocketDisconnected();
    void onLocalSocketReadyRead();
    void onLocalSocketErrorOccurred(QLocalSocket::LocalSocketError socketError);

private:
    DBRW &dbrw_;
};

#endif // LOCALSERVER_H
