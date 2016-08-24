#ifndef ALFREDWORKFLOW_H
#define ALFREDWORKFLOW_H

#include <QObject>
#include "derflaaction.h"

class AlfredWorkflow : public QObject
{
    Q_OBJECT
public:
    explicit AlfredWorkflow(QObject *parent = 0);

    bool installFromBundle(const QString& path);
    bool loadFromDirectory(const QString& dirName);
    const QString& bundleId() const;
    const QString& category() const;
    const QString& author() const;
    const QString& description() const;
    const QString& readme() const;
    const QString& name() const;
    const QString& webaddress() const;
    bool disabled() const;
    bool hitKeyword(const QString& keyword);
    DerflaActionList& getActions(const QString& input);
    bool triggerAction(int index);
signals:

public slots:

private:
    QString bundleId_;
    QString category_;
    QString author_;
    QString description_;
    QString readme_;
    QString name_;
    QString webaddress_;
    bool disabled_;
    QStringList keywords_;
    QString inputTitle_;
    QString inputSubtext_;
    int inputType_;
    bool inputWithSpace_;
    DerflaActionList actions_;
};


typedef QSharedPointer<AlfredWorkflow> AlfredWorkflowPtr;
typedef QList<AlfredWorkflowPtr> AlfredWorkflowList;

#endif // ALFREDWORKFLOW_H
