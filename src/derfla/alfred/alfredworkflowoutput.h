#ifndef ALFREDWORKFLOWOUTPUT_H
#define ALFREDWORKFLOWOUTPUT_H

#include <QObject>

class AlfredWorkflowOutput : public QObject
{
    Q_OBJECT
public:
    explicit AlfredWorkflowOutput(QObject *parent = 0);

    void parse(const QString& type, const QUuid uid, const QVariantMap& v);
signals:

public slots:

private:
    QUuid uid_;
    QString output_; // alfred.workflow.output.*
};

typedef QSharedPointer<AlfredWorkflowOutput> AlfredWorkflowOutputPtr;
typedef QList<AlfredWorkflowOutputPtr> AlfredWorkflowOutputList;
#endif // ALFREDWORKFLOWOUTPUT_H