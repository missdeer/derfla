#ifndef ALFREDWORKFLOWINPUT_H
#define ALFREDWORKFLOWINPUT_H

#include <QObject>

class AlfredWorkflowInput : public QObject
{
    Q_OBJECT
public:
    explicit AlfredWorkflowInput(QObject *parent = 0);

signals:

public slots:
};

typedef QSharedPointer<AlfredWorkflowInput> AlfredWorkflowInputPtr;
typedef QList<AlfredWorkflowInputPtr> AlfredWorkflowInputList;

#endif // ALFREDWORKFLOWINPUT_H