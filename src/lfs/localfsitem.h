#ifndef LocalFSItem_H
#define LocalFSItem_H

#include <QObject>
#include <QIcon>

enum LocalFSItemType
{
    FSIT_UNKNOWN,
    FSIT_CONSOLE,
    FSIT_GUI,
};

class LocalFSItem : public QObject
{
    Q_OBJECT
public:
    explicit LocalFSItem(QObject *parent = 0);
    
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
    QString actionType_;
    QUuid id_;
};

typedef QSharedPointer<LocalFSItem> LocalFSItemPtr;
typedef QList<LocalFSItemPtr> LocalFSItemList;

#endif // LocalFSItem_H
