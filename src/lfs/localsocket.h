#ifndef LOCALSOCKET_H
#define LOCALSOCKET_H

#include <QLocalSocket>

class LocalSocket : public QLocalSocket
{
    Q_OBJECT
public:
    explicit LocalSocket(QObject *parent = nullptr);
};

#endif // LOCALSOCKET_H
