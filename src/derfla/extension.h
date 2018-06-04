#ifndef EXTENSION_H
#define EXTENSION_H

#include <QObject>
#include "derflaaction.h"

QT_BEGIN_NAMESPACE
class QProcess;
QT_END_NAMESPACE

class Extension : public QObject
{
    Q_OBJECT
public:
    explicit Extension(QObject *parent = nullptr);
    ~Extension();

    bool query(const QString& prefix);

    const QString &author() const;
    void setAuthor(const QString &author);

    const QString &name() const;
    void setName(const QString &name);

    const QString &description() const;
    void setDescription(const QString &description);

    const QString &executable() const;
    void setExecutable(const QString &executable);

    const QString &executor() const;
    void setExecutor(const QString &executor);

    const QString &prefix() const;
    void setPrefix(const QString &prefix);

    const QString &waitTitle() const;
    void setWaitTitle(const QString &waitTitle);

    const QString &waitDescription() const;
    void setWaitDescription(const QString &waitDescription);

    const QIcon &waitIcon() const;
    void setWaitIcon(const QIcon &waitIcon);
    void setWaitIconPath(const QString& waitIconPath);
    void setWaitIconData(const QString& waitIconData);
signals:
    void queried(DerflaActionList &);
public slots:    
    void finished(int, QProcess::ExitStatus);

private:
    QString author_;
    QString name_;
    QString description_;
    QString executable_;
    QString executor_; // php, python, ruby, perl, bash, etc.
    QString prefix_;
    QString waitTitle_;
    QString waitDescription_;
    QIcon waitIcon_;
    DerflaActionList derflaActions_;
    QProcess* process_;
};

typedef QSharedPointer<Extension> ExtensionPtr;

#endif // Extension_H
