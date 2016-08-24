#ifndef ALFREDWORKFLOW_H
#define ALFREDWORKFLOW_H

#include <QObject>

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
};


typedef QSharedPointer<AlfredWorkflow> AlfredWorkflowPtr;
typedef QList<AlfredWorkflowPtr> AlfredWorkflowList;

#endif // ALFREDWORKFLOW_H
