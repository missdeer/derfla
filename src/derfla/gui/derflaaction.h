#ifndef DERFLAACTION_H
#define DERFLAACTION_H

#include <QObject>

enum DerflaActionType
{
    DAT_UNKNOWN,
    DAT_CONSOLE,
    DAT_GUI,
    // Alfred work flow action types are declared below
    DAT_SCRIPT, // shell/bash/zsh, perl, php, python, ruby, etc.
    DAT_OPENURL,
    DAT_APPLESCRIPT,
    DAT_SYSTEMWEBSEARCH,
    DAT_REVEALFILE,
    DAT_TERMINALCOMMAND,
    DAT_OPENFILE,
    DAT_BROWSEINDERFLA,
    DAT_LAUNCHFILES,
};

class DerflaAction : public QObject
{
    Q_OBJECT
public:
    explicit DerflaAction(QObject *parent = 0);

    bool operator==(const DerflaAction& da)
    {
        return id_ == da.id_;
    }
    
    const QIcon& icon() const;
    void setIcon(const QIcon& icon);

    const QString& title() const;
    void setTitle(const QString& title);

    const QString& description() const;
    void setDescription(const QString& description);

    const QString& target() const;
    void setTarget(const QString& target);

    const QString& arguments() const;
    void setArguments(const QString& arguments);

    const QString& workingDirectory() const;
    void setWorkingDirectory(const QString& workingDirectory);

    const QString& actionType() const;
    void setActionType(const QString& actionType);

    bool disabled() const { return disabled_; }
    void setDisabled(bool val) { disabled_ = val; }
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
    QString actionType_;

    bool disabled_ = false;
    QUuid id_;
};

typedef QSharedPointer<DerflaAction> DerflaActionPtr;
typedef QList<DerflaActionPtr> DerflaActionList;

#endif // DERFLAACTION_H
