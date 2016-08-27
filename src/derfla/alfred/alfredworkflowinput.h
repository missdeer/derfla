#ifndef ALFREDWORKFLOWINPUT_H
#define ALFREDWORKFLOWINPUT_H

#include <QObject>
#include "derflaaction.h"

struct AlfredWorkflowInputField
{
    bool not_ = false;
    bool split_ = true;
    bool words_ = true;
    QString field_;
    QString value_;
};

typedef QSharedPointer<AlfredWorkflowInputField> AlfredWorkflowInputFieldPtr;
typedef QList<AlfredWorkflowInputFieldPtr> AlfredWorkflowInputFieldList;

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
    QString workingDirectory_;
    QUuid uid_;
    QString typeId_; // alfred.workflow.input.*

    // keyword
    QString text_;
    QString subtext_;

    // script filter
    QString title_;
    QString script_;
    QString runningSubtext_;

    QString keyword_;
    int argumentType_ = 2; // 0 - arguments required, 1- arguments optional, 2- none
    int type_ = 0; // 0-bash, 1-php, 2-ruby, 3-python, 4-perl, 5-zsh, 6-osascript(AS), 7-osascript(JS)
    int escaping_ = 0;
    int dateRange_ = 0;
    int queueMode_ = 0;
    int queueDelayMode_ = 0;
    int queueDelayCustom_ = 0;
    bool queueDelayImmediatelyInitially = false;
    bool withSpace_ = false;
    bool anchorFields_ = true;
    bool includeSystem_ = false;
    QStringList scopes_;
    QStringList types_;
    AlfredWorkflowInputFieldList fields_;
};

typedef QSharedPointer<AlfredWorkflowInput> AlfredWorkflowInputPtr;
typedef QList<AlfredWorkflowInputPtr> AlfredWorkflowInputList;

#endif // ALFREDWORKFLOWINPUT_H
