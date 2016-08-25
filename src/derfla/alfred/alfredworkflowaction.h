#ifndef ALFREDWORFLOWACTION_H
#define ALFREDWORFLOWACTION_H

#include <QObject>

class AlfredWorflowAction : public QObject
{
    Q_OBJECT
public:
    explicit AlfredWorflowAction(QObject *parent = 0);

signals:

public slots:
};

typedef QSharedPointer<AlfredWorflowAction> AlfredWorflowActionPtr;
typedef QList<AlfredWorflowActionPtr> AlfredWorflowActionList;

#endif // ALFREDWORFLOWACTION_H