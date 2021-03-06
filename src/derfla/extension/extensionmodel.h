#ifndef EXTENSIONMODEL_H
#define EXTENSIONMODEL_H

#include <QAbstractTableModel>
#include "extension.h"

class ExtensionModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ExtensionModel(QObject* parent);

    virtual QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual int columnCount(const QModelIndex & parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

};

#endif // EXTENSIONMODEL_H
