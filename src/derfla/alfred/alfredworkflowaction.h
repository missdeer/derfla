#ifndef AlfredWorkflowAction_H
#define AlfredWorkflowAction_H

#include <QObject>
#include "derflaaction.h"

class AlfredWorkflowAction : public QObject
{
    Q_OBJECT
public:
    explicit AlfredWorkflowAction(const QString& workingDirectory, QObject *parent = 0);

    void getDerflaActions(const QString& input, DerflaActionList& derflaActions);
    void parse(const QString& type, const QUuid uid, const QVariantMap& v);
    const QUuid& getUid() const { return uid_; }

signals:

public slots:

private:
    const QString& workingDirectory_;
    QUuid uid_;
    QString typeId_; // alfred.workflow.action.*
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
