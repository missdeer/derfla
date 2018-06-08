#ifndef DERFLAACTION_H
#define DERFLAACTION_H

#include <QUuid>
#include <QIcon>

class DerflaAction
{
public:
    DerflaAction();

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

    const QString &scriptExecutor() const;
    void setScriptExecutor(const QString &scriptExecutor);

private:
    QIcon icon_;
    QString title_;
    QString description_;
    QString target_;
    QString arguments_;
    QString workingDirectory_;
    QString actionType_;
    QString scriptExecutor_;

    bool disabled_ = false;
    QUuid id_;
};

typedef QSharedPointer<DerflaAction> DerflaActionPtr;
typedef QList<DerflaActionPtr> DerflaActionList;

#endif // DERFLAACTION_H
