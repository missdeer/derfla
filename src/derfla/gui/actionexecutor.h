#ifndef ACTIONEXECUTOR_H
#define ACTIONEXECUTOR_H

#include <QObject>
#include "derflaaction.h"

class ActionExecutor : public QObject
{
    Q_OBJECT
public:
    explicit ActionExecutor(QObject *parent = nullptr);

    bool operator()(DerflaActionPtr da);
signals:

public slots:
};

#endif // ACTIONEXECUTOR_H
