#ifndef ALFREDWORKFLOWTRIGGER_H
#define ALFREDWORKFLOWTRIGGER_H

#include <QObject>

class AlfredWorkflowTrigger : public QObject
{
    Q_OBJECT
public:
    explicit AlfredWorkflowTrigger(const QString& workingDirectory, QObject *parent = 0);

    void parse(const QString& type, const QUuid uid, const QVariantMap& v);
    const QUuid& getUid() const { return uid_; }
signals:

public slots :

private:
    const QString& workingDirectory_;
    QUuid uid_;
    QString typeId_; // alfred.workflow.trigger.*
    int action_ = 0;
    int argument_ = 0;
    int hotkey_ = 0;
    int hotMod_ = 0;
    int modsMode_ = 0;
    bool leftCursor_ = false;
    bool acceptsMulti_ = false;
    QString hotString_;
    QString name_;
    QString text_;
    QString subtext_;
    QStringList fileTypes_;
};

typedef QSharedPointer<AlfredWorkflowTrigger> AlfredWorkflowTriggerPtr;
typedef QList<AlfredWorkflowTriggerPtr> AlfredWorkflowTriggerList;
#endif // ALFREDWORKFLOWTRIGGER_H