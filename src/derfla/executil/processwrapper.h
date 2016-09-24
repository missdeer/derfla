#ifndef PROCESSWRAPPER_H
#define PROCESSWRAPPER_H

#include <QObject>

class ProcessWrapper : public QProcess
{
    Q_OBJECT
public:
    explicit ProcessWrapper(QObject *parent = 0);
    explicit ProcessWrapper(const QUuid& uuid, QObject *parent = 0);
    ~ProcessWrapper();
    const QUuid& id() const;
signals:

public slots:

protected:
    QUuid uuid_;
};

typedef QSharedPointer<ProcessWrapper> ProcessPtr;

#endif // PROCESSWRAPPER_H
