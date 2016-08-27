#ifndef ALFREDWORKFLOW_H
#define ALFREDWORKFLOW_H

#include <QObject>
#include "derflaaction.h"
#include "alfredworkflowaction.h"
#include "alfredworkflowinput.h"
#include "alfredworkflowoutput.h"
#include "alfredworkflowtrigger.h"

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
    int connectionCount() { return connections_.size();  }
    int triggerCount() { return triggerList_.size();  }
    int inputCount() { return inputList_.size();  }
    int outputCount() { return outputList_.size();  }
    int actionCount() { return actionList_.size(); }
signals:

public slots:

private:
    QString installDirectory_;

    QString bundleId_;
    QString category_;
    QString author_;
    QString description_;
    QString readme_;
    QString name_;
    QString webaddress_;
    bool disabled_ = false;
    
    QMap<QUuid, QList<QUuid>> connections_;
    AlfredWorkflowTriggerList triggerList_;
    AlfredWorkflowInputList inputList_;
    AlfredWorkflowOutputList outputList_;
    AlfredWorkflowActionList actionList_;
    DerflaActionList derflaActions_;
};

typedef QSharedPointer<AlfredWorkflow> AlfredWorkflowPtr;
typedef QList<AlfredWorkflowPtr> AlfredWorkflowList;

#endif // ALFREDWORKFLOW_H
