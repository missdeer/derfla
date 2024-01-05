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
    ~ExtensionManager() override;

    bool                                           loadAllFromLocal();
    void                                           query(const QString &input);
    [[nodiscard]] const std::vector<ExtensionPtr> &extensions() const;

signals:
    void emptyAction();
    void actionUpdated(DerflaActionList &dal);
public slots:
    void extensionQueried(DerflaActionList &dal);

private:
    std::vector<ExtensionPtr>        extensions_;
    QMultiMap<QString, ExtensionPtr> prefixExtensionMap_;
};

#endif // ExtensionMANAGER_H
