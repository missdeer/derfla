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

    bool disabled() const { return disabled_; }
    void setDisabled(bool val) { disabled_ = val; }

    void setRunner(const std::function<bool (DerflaAction *)> &run);

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

    bool disabled_ = false;
    QUuid id_;
	std::function<bool(DerflaAction*)> run_;
};

typedef QSharedPointer<DerflaAction> DerflaActionPtr;
typedef QList<DerflaActionPtr> DerflaActionList;

#endif // DERFLAACTION_H
