#ifndef AlfredWorkflowAction_H
#define AlfredWorkflowAction_H

#include <QObject>

class AlfredWorkflowAction : public QObject
{
    Q_OBJECT
public:
    explicit AlfredWorkflowAction(QObject *parent = 0);

    void parse(const QString& type, const QUuid uid, const QVariantMap& v);
signals:

public slots:

private:
    QUuid uid_;
    QString action_; // alfred.workflow.action.*
    QString script_;
    QString appleScript_;
    QString url_;
    int type_ = 0;
    int escaping_ = 0;
    bool cacheScript_ = false;
    bool plusSpaces_ = false;
    bool utf8_ = false;
};

typedef QSharedPointer<AlfredWorkflowAction> AlfredWorkflowActionPtr;
typedef QList<AlfredWorkflowActionPtr> AlfredWorkflowActionList;

#endif // AlfredWorkflowAction_H