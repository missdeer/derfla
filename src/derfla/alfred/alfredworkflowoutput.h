#ifndef ALFREDWORKFLOWOUTPUT_H
#define ALFREDWORKFLOWOUTPUT_H

#include <QObject>

class AlfredWorkflowOutput : public QObject
{
    Q_OBJECT
public:
    explicit AlfredWorkflowOutput(const QString& workingDirectory, QObject *parent = 0);

    void parse(const QString& type, const QUuid uid, const QVariantMap& v);
    const QUuid& getUid() const { return uid_; }
signals:

public slots:

private:
    QString workingDirectory_;
    QUuid uid_;
    QString typeId_; // alfred.workflow.output.*
    bool lastPathComponent_ = false;
    bool onlyShowIfQueryPopulated_ = false;
    bool removeExtension_ = false;
    bool sticky_ = false;
    bool autoPaste_ = false;
    int output_ = 0;
    int escaping_ = 0;
    int type_ = 0;
    QString script_;
    QString text_;
    QString title_;
    QString clipboardText_;
};

typedef QSharedPointer<AlfredWorkflowOutput> AlfredWorkflowOutputPtr;
typedef QList<AlfredWorkflowOutputPtr> AlfredWorkflowOutputList;
#endif // ALFREDWORKFLOWOUTPUT_H