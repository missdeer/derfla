#ifndef LocalFSItem_H
#define LocalFSItem_H

#include <QObject>
#include <QIcon>

enum LocalFSItemType
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

class LocalFSItem : public QObject
{
    Q_OBJECT
public:
    explicit LocalFSItem(QObject *parent = 0);
    
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

    LocalFSItemType actionType() const;
    void setActionType(const LocalFSItemType& actionType);

    bool operator==(const LocalFSItem& da)
    {
        return id_ == da.id_;
    }

private:
    QIcon icon_;
    QString title_;
    QString description_;
    QString target_;
    QString arguments_;
    QString workingDirectory_;
    LocalFSItemType actionType_;
    QUuid id_;
};

typedef QSharedPointer<LocalFSItem> LocalFSItemPtr;
typedef QList<LocalFSItemPtr> LocalFSItemList;

#endif // LocalFSItem_H
