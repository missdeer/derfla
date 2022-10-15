#include "stdafx.h"

#include <QLocalSocket>

#include "localserver.h"

LocalServer::LocalServer(QObject *parent) : QLocalServer {parent}
{
    connect(this, &QLocalServer::newConnection, this, &LocalServer::onNewConnection);
}

void LocalServer::onNewConnection()
{
    auto *clientConnection = nextPendingConnection();
    Q_ASSERT(clientConnection);
    connect(clientConnection, &QLocalSocket::disconnected, this, &LocalServer::onLocalSocketDisconnected);
    connect(clientConnection, &QLocalSocket::readyRead, this, &LocalServer::onLocalSocketReadyRead);
    connect(clientConnection, &QLocalSocket::errorOccurred, this, &LocalServer::onLocalSocketErrorOccurred);
}

void LocalServer::onLocalSocketDisconnected() {}

void LocalServer::onLocalSocketReadyRead() {}

void LocalServer::onLocalSocketErrorOccurred() {}