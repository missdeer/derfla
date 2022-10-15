#ifndef LOCALSERVER_H
#define LOCALSERVER_H

#include <QLocalServer>

class LocalServer : public QLocalServer
{
    Q_OBJECT
public:
    explicit LocalServer(QObject *parent = nullptr);

private slots:
    void onNewConnection();
    void onLocalSocketDisconnected();
    void onLocalSocketReadyRead();
    void onLocalSocketErrorOccurred();
};

#endif // LOCALSERVER_H
