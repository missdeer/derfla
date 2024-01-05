#ifndef DERFLAACTION_H
#define DERFLAACTION_H

#include <QIcon>
#include <QUuid>

#include "util_global.h"

class UTILSHARED_EXPORT DerflaAction
{
public:
    DerflaAction();

    bool operator==(const DerflaAction &da)
    {
        return id_ == da.id_;
    }

    [[nodiscard]] const QIcon &icon() const;
    void                       setIcon(const QIcon &icon);

    [[nodiscard]] const QString &title() const;
    void                         setTitle(const QString &title);

    [[nodiscard]] const QString &description() const;
    void                         setDescription(const QString &description);

    [[nodiscard]] const QString &target() const;
    void                         setTarget(const QString &target);

    [[nodiscard]] const QString &arguments() const;
    void                         setArguments(const QString &arguments);

    [[nodiscard]] const QString &workingDirectory() const;
    void                         setWorkingDirectory(const QString &workingDirectory);

    [[nodiscard]] const QString &actionType() const;
    void                         setActionType(const QString &actionType);

    [[nodiscard]] bool disabled() const
    {
        return disabled_;
    }
    void setDisabled(bool val)
    {
        disabled_ = val;
    }

    [[nodiscard]] const QString &scriptExecutor() const;
    void                         setScriptExecutor(const QString &scriptExecutor);

private:
    QIcon   icon_;
    QString title_;
    QString description_;
    QString target_;
    QString arguments_;
    QString workingDirectory_;
    QString actionType_;
    QString scriptExecutor_;

    bool  disabled_ = false;
    QUuid id_;
};

using DerflaActionPtr  = std::shared_ptr<DerflaAction>;
using DerflaActionList = std::vector<DerflaActionPtr>;

#endif // DERFLAACTION_H
