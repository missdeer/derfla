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
    bool query(const QString& input);
    bool installExtension(const QString& extensionFile);
signals:
    void actionUpdated(DerflaActionList& dal);
public slots:
    void extensionQueried(DerflaActionList& dal);
private:
    QList<ExtensionPtr> extensions_;
};

#endif // ExtensionMANAGER_H
