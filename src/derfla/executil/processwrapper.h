#ifndef PROCESSWRAPPER_H
#define PROCESSWRAPPER_H

#include <QObject>

class ProcessWrapper : public QObject
{
    Q_OBJECT
public:
    explicit ProcessWrapper(QObject *parent = 0);
    explicit ProcessWrapper(const QUuid& id, QObject *parent = 0);
    ~ProcessWrapper();
    const QUuid& id() const;

    void setWorkingDirectory(const QString& workingDirectory);
signals:

public slots:

private:
    QUuid id_;
    QProcess* process_;
};

typedef QSharedPointer<ProcessWrapper> ProcessPtr;

#endif // PROCESSWRAPPER_H
