#ifndef ALFREDWORKFLOWOUTPUT_H
#define ALFREDWORKFLOWOUTPUT_H

#include <QObject>

class AlfredWorkflowOutput : public QObject
{
    Q_OBJECT
public:
    explicit AlfredWorkflowOutput(QObject *parent = 0);

signals:

public slots:
};

typedef QSharedPointer<AlfredWorkflowOutput> AlfredWorkflowOutputPtr;
typedef QList<AlfredWorkflowOutputPtr> AlfredWorkflowOutputList;
#endif // ALFREDWORKFLOWOUTPUT_H