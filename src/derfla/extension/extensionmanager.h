#ifndef EXTENSIONMANAGER_H
#define EXTENSIONMANAGER_H

#include <QMultiMap>
#include <QObject>

#include "extension.h"

class ExtensionManager : public QObject
{
    Q_OBJECT
public:
    explicit ExtensionManager(QObject *parent = nullptr);
    ~ExtensionManager();

    bool                       loadAllFromLocal();
    void                       query(const QString &input);
    const QList<ExtensionPtr> &extensions() const;

signals:
    void emptyAction();
    void actionUpdated(DerflaActionList &dal);
public slots:
    void extensionQueried(DerflaActionList &dal);

private:
    QList<ExtensionPtr>              extensions_;
    QMultiMap<QString, ExtensionPtr> prefixExtensionMap_;
};

#endif // ExtensionMANAGER_H
