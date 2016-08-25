#ifndef DERFLAACTION_H
#define DERFLAACTION_H

#include <QObject>

enum DerflaActionType
{
    DAT_UNKNOWN,
    DAT_CONSOLE,
    DAT_GUI,
    // Alfred work flow action types are declared below
    DAT_SCRIPT,
    DAT_OPENURL,
    DAT_APPLESCRIPT,
    DAT_SYSTEMWEBSEARCH,
    DAT_REVEALFILE,
    DAT_TERMINALCOMMAND,
    DAT_OPENFILE,
    DAT_BROWSEINALFRED,
    DAT_LAUNCHFILES,
};

class AlfredWorkflowAction;

class DerflaAction : public QObject
{
    Q_OBJECT
        friend AlfredWorkflowAction;
public:
    explicit DerflaAction(QObject *parent = 0);
    
    QIcon icon() const;
    void setIcon(const QIcon& icon);

    QString title() const;
    void setTitle(const QString& title);

    QString description() const;
    void setDescription(const QString& description);

    QString target() const;
    void setTarget(const QString& target);

    QString arguments() const;
    void setArguments(const QString& arguments);

    QString workingDirectory() const;
    void setWorkingDirectory(const QString& workingDirectory);

    DerflaActionType actionType() const;
    void setActionType(const DerflaActionType& actionType);

signals:

public slots :
    bool run();

private:
    QIcon icon_;
    QString title_;
    QString description_;
    QString target_;
    QString arguments_;
    QString workingDirectory_;
    DerflaActionType actionType_;
    // members from AlfredWorkflowAction
    QString script_;
    QString appleScript_;
    QString url_;
    int type_ = 0;
    int escaping_ = 0;
    bool cacheScript_ = false;
    bool plusSpaces_ = false;
    bool utf8_ = false;
};

typedef QSharedPointer<DerflaAction> DerflaActionPtr;
typedef QList<DerflaActionPtr> DerflaActionList;

#endif // DERFLAACTION_H
