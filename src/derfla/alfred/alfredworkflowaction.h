#ifndef AlfredWorkflowAction_H
#define AlfredWorkflowAction_H

#include <QObject>

class AlfredWorkflowAction : public QObject
{
    Q_OBJECT
public:
    explicit AlfredWorkflowAction(QObject *parent = 0);

signals:

public slots:
};

typedef QSharedPointer<AlfredWorkflowAction> AlfredWorkflowActionPtr;
typedef QList<AlfredWorkflowActionPtr> AlfredWorkflowActionList;

#endif // AlfredWorkflowAction_H