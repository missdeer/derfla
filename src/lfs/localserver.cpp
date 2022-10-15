#include "stdafx.h"

#include "localserver.h"
#include "dbrw.h"

LocalServer::LocalServer(DBRW &dbrw, QObject *parent) : QLocalServer {parent}, dbrw_(dbrw)
{
    connect(this, &QLocalServer::newConnection, this, &LocalServer::onNewConnection);
}

void LocalServer::onNewConnection()
{
    auto *localSocket = nextPendingConnection();
    Q_ASSERT(localSocket);
    connect(localSocket, &QLocalSocket::disconnected, this, &LocalServer::onLocalSocketDisconnected);
    connect(localSocket, &QLocalSocket::readyRead, this, &LocalServer::onLocalSocketReadyRead);
    connect(localSocket, &QLocalSocket::errorOccurred, this, &LocalServer::onLocalSocketErrorOccurred);
}

void LocalServer::onLocalSocketDisconnected()
{
    auto *localSocket = qobject_cast<QLocalSocket *>(sender());
    Q_ASSERT(localSocket);
    localSocket->deleteLater();
}

void LocalServer::onLocalSocketReadyRead()
{
    auto *localSocket = qobject_cast<QLocalSocket *>(sender());
    Q_ASSERT(localSocket);
    auto msg = localSocket->readAll();
    auto res = dbrw_.search(QString(msg));
    localSocket->write(res.toUtf8());
    localSocket->flush();
    localSocket->close();
}

void LocalServer::onLocalSocketErrorOccurred(QLocalSocket::LocalSocketError socketError)
{
    auto *localSocket = qobject_cast<QLocalSocket *>(sender());
    Q_ASSERT(localSocket);
    qCritical() << "local socket error" << socketError;
}