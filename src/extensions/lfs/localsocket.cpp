#include "stdafx.h"

#include <QtCore>

#include "localsocket.h"

LocalSocket::LocalSocket(QTextStream &stream, QObject *parent) : QLocalSocket {parent}, stream_(stream)
{
    connect(this, &QLocalSocket::connected, this, &LocalSocket::onConnected);
    connect(this, &QLocalSocket::disconnected, this, &LocalSocket::onDisconnected);
    connect(this, &QLocalSocket::errorOccurred, this, &LocalSocket::onErrorOccurred);
    connect(this, &QLocalSocket::readyRead, this, &LocalSocket::onReadyRead);
    connect(this, &QIODevice::readChannelFinished, this, &LocalSocket::onReadChannelFinished);
}

void LocalSocket::onConnected()
{
    qDebug() << __FUNCTION__;
    connected_ = true;
    flushCachedMsg();
}

void LocalSocket::onDisconnected()
{
    qWarning() << "local socket disconnected from server";
    connected_ = false;
    QCoreApplication::quit();
}

void LocalSocket::onErrorOccurred(QLocalSocket::LocalSocketError socketError)
{
    qCritical() << "local socket error " << socketError;
    auto *localSocket = qobject_cast<QLocalSocket *>(sender());
    Q_ASSERT(localSocket);
    if (connected_)
    {
        localSocket->disconnectFromServer();
    }
}

void LocalSocket::sendToServer(const QByteArray &data)
{
    if (connected_)
    {
        write(data);
    }
    else
    {
        msgToWrite_ = data;
    }
}
void LocalSocket::flushCachedMsg()
{
    if (!msgToWrite_.isEmpty())
    {
        write(msgToWrite_);
    }
}
void LocalSocket::onReadyRead()
{
    auto *localSocket = qobject_cast<QLocalSocket *>(sender());
    Q_ASSERT(localSocket);
    auto res =QString(localSocket->readAll());
    qDebug() << __FUNCTION__ << res;
    stream_ << res;
    localSocket->disconnectFromServer();
}

void LocalSocket::onReadChannelFinished()
{
    auto *localSocket = qobject_cast<QLocalSocket *>(sender());
    Q_ASSERT(localSocket);
    auto res =QString(localSocket->readAll());
    qDebug() << __FUNCTION__ << res;
    stream_ << res;
    localSocket->disconnectFromServer();
}
