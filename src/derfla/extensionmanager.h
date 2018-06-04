#ifndef EXTENSIONMANAGER_H
#define EXTENSIONMANAGER_H

#include <QObject>
#include "extension.h"

class ExtensionManager : public QObject
{
    Q_OBJECT
public:
    explicit ExtensionManager(QObject *parent = nullptr);

    bool loadAllFromCache();
    bool getActions(DerflaActionList& dal, const QString& prefix, int countRequired = 50);
    bool installExtension(const QString& extensionFile);
signals:

public slots:

private:
    QList<ExtensionPtr> extensions_;
};

#endif // ExtensionMANAGER_H
