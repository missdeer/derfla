#ifndef ALFREDWORKFLOWINPUT_H
#define ALFREDWORKFLOWINPUT_H

#include <QObject>
#include "derflaaction.h"

class AlfredWorkflowInput : public QObject
{
    Q_OBJECT
public:
    explicit AlfredWorkflowInput(const QString& workingDirectory, QObject *parent = 0);

    bool hitKeyword(const QString& keyword);
    void getDerflaActions(const QString& input, DerflaActionList& derflaActions);
    void parse(const QString& type, const QUuid uid, const QVariantMap& v);
    const QUuid& getUid() const { return uid_; }

signals:

public slots:

private:
    const QString& workingDirectory_;
    QUuid uid_;
    QString typeId_; // alfred.workflow.input.*
    QString text_;
    QString title_;
    QString subtext_;
    QString script_;
    QString runningSubtext_;
    int argumentType_ = 0;
    int type_ = 0;
    int escaping_ = 0;
    bool withSpace_ = false;
    QStringList keywords_;
};

typedef QSharedPointer<AlfredWorkflowInput> AlfredWorkflowInputPtr;
typedef QList<AlfredWorkflowInputPtr> AlfredWorkflowInputList;

#endif // ALFREDWORKFLOWINPUT_H
