#ifndef DERFLAACTION_H
#define DERFLAACTION_H

#include <QObject>

enum DerflaActionType
{
    DAT_UNKNOWN,
    DAT_CONSOLE,
    DAT_GUI,
    DAT_SCRIPT,
};

class DerflaAction : public QObject
{
    Q_OBJECT
public:
    explicit DerflaAction(QObject *parent = 0);

    bool run();

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

public slots:

private:
    QIcon icon_;
    QString title_;
    QString description_;
    QString target_;
    QString arguments_;
    QString workingDirectory_;
    DerflaActionType actionType_;
};

typedef QSharedPointer<DerflaAction> DerflaActionPtr;
typedef QList<DerflaActionPtr> DerflaActionList;

#endif // DERFLAACTION_H
