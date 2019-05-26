#include "stdafx.h"
#include "derflaapp.h"
#include "extensionmanager.h"
#include "extensionmodel.h"

ExtensionModel::ExtensionModel( QObject *parent)
    : QAbstractTableModel(parent)
{

}

QModelIndex ExtensionModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
            return QModelIndex();

    return createIndex(row, column);
}

int ExtensionModel::rowCount(const QModelIndex &) const
{
    return derflaApp->extensionManager()->extensions().size();
}

int ExtensionModel::columnCount(const QModelIndex &) const
{
    return 7;
}

QVariant ExtensionModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    if (index.row() < 0 || index.row() >= derflaApp->extensionManager()->extensions().size())
        return QVariant();

    auto e = derflaApp->extensionManager()->extensions()[index.row()];
    switch(index.column())
    {
    case 0:
        return e->id();
    case 1:
        return e->name();
    case 2:
        return e->author();
    case 3:
        return e->description();
    case 4:
        return e->daemon();
    case 5:
        return QFileInfo(e->executable()).fileName();
    case 6:
        return QFileInfo(e->executor()).fileName();
    }

    return QVariant();
}

Qt::ItemFlags ExtensionModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
}

QVariant ExtensionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        QMap<int, QString> m = {
            {0, tr("Id")},
            {1, tr("Name")},
            {2, tr("Author")},
            {3, tr("Description")},
            {4, tr("Daemon")},
            {5, tr("Executable")},
            {6, tr("Executor")},
        };
        auto it = m.find(section);
        if (it != m.end())
            return QVariant(it.value());
    }

    return QVariant();
}
