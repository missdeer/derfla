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

    void runDaemon();
    void stopDaemon();
    bool query(const QString& input);

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

    void setPrefix(const QStringList &prefix);
    bool prefixMatched(const QString &text);

    const QString &waitTitle() const;
    void setWaitTitle(const QString &waitTitle);

    const QString &waitDescription() const;
    void setWaitDescription(const QString &waitDescription);

    const QIcon &waitIcon() const;
    void setWaitIcon(const QIcon &waitIcon);
    void setWaitIconPath(const QString& waitIconPath);
    void setWaitIconData(const QString& waitIconData);

    const QString &id() const;
    void setId(const QString &id);

    bool daemon() const;
    void setDaemon(bool daemon);

signals:
    void queried(DerflaActionList &);
public slots:    
    void finished(int, QProcess::ExitStatus);

private:
    QProcess* process_;
    QString author_;
    QString name_;
    QString description_;
    QString executable_;
    QString executor_; // php, python, ruby, perl, bash, etc.
    QString waitTitle_;
    QString waitDescription_;
    QString id_;
    QStringList prefix_;
    QIcon waitIcon_;
    bool daemon_;

    DerflaActionList derflaActions_;

    QString findProgram();
};

typedef QSharedPointer<Extension> ExtensionPtr;

#endif // Extension_H
