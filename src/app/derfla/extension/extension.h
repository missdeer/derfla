#ifndef EXTENSION_H
#define EXTENSION_H

#include <QObject>
#include <QProcess>

#include "derflaaction.h"

class Extension : public QObject
{
    Q_OBJECT
public:
    explicit Extension(QObject *parent = nullptr);
    ~Extension() override;

    void runDaemon();
    void stopDaemon();
    bool query(const QString &input);
    void stopQuery();

    [[nodiscard]] const QString &author() const;
    void                         setAuthor(const QString &author);

    [[nodiscard]] const QString &name() const;
    void                         setName(const QString &name);

    [[nodiscard]] const QString &description() const;
    void                         setDescription(const QString &description);

    [[nodiscard]] const QString &executable() const;
    void                         setExecutable(const QString &executable);

    [[nodiscard]] const QString &executor() const;
    void                         setExecutor(const QString &executor);

    void                             setPrefix(const QStringList &prefix);
    [[nodiscard]] const QStringList &prefix();

    [[nodiscard]] const QString &waitTitle() const;
    void                         setWaitTitle(const QString &waitTitle);

    [[nodiscard]] const QString &waitDescription() const;
    void                         setWaitDescription(const QString &waitDescription);

    [[nodiscard]] const QIcon &waitIcon() const;
    void                       setWaitIcon(const QIcon &waitIcon);
    void                       setWaitIconPath(const QString &waitIconPath);
    void                       setWaitIconData(const QString &waitIconData);

    [[nodiscard]] const QString &id() const;
    void                         setId(const QString &id);

    [[nodiscard]] bool daemon() const;
    void               setDaemon(bool daemon);

signals:
    void queried(DerflaActionList &);
public slots:
    void finished(int, QProcess::ExitStatus);

private:
    QString          author_;
    QString          name_;
    QString          description_;
    QString          executable_;
    QString          executor_; // php, python, ruby, perl, bash, etc.
    QString          waitTitle_;
    QString          waitDescription_;
    QString          id_;
    QStringList      prefix_;
    QIcon            waitIcon_;
    bool             daemon_ {false};
    QProcess        *subProcess_ {nullptr};
    DerflaActionList derflaActions_;

    [[nodiscard]] QProcessEnvironment getProcessEnvironment();
    void                              parseActionConfig(QJsonObject &eleObj, DerflaActionPtr &action);
};

using ExtensionPtr = QSharedPointer<Extension>;

#endif // Extension_H
