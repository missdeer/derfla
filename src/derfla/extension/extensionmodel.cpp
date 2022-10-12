#include "stdafx.h"

#include "extensionmodel.h"
#include "derflaapp.h"
#include "extensionmanager.h"

ExtensionModel::ExtensionModel(QObject *parent) : QAbstractTableModel(parent) {}

QModelIndex ExtensionModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
    {
        return {};
    }

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
    {
        return {};
    }

    if (role != Qt::DisplayRole)
    {
        return {};
    }

    if (index.row() < 0 || index.row() >= derflaApp->extensionManager()->extensions().size())
    {
        return {};
    }

    auto extension = derflaApp->extensionManager()->extensions()[index.row()];
    switch (index.column())
    {
    case 0:
        return extension->id();
    case 1:
        return extension->name();
    case 2:
        return extension->author();
    case 3:
        return extension->description();
    case 4:
        return extension->daemon();
    case 5:
        return QFileInfo(extension->executable()).fileName();
    case 6:
        return QFileInfo(extension->executor()).fileName();
    }

    return {};
}

Qt::ItemFlags ExtensionModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    return QAbstractItemModel::flags(index);
}

QVariant ExtensionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        static QMap<int, QString> headerMap = {
            {0, tr("Id")},
            {1, tr("Name")},
            {2, tr("Author")},
            {3, tr("Description")},
            {4, tr("Daemon")},
            {5, tr("Executable")},
            {6, tr("Executor")},
        };
        auto iter = headerMap.find(section);
        if (iter != headerMap.end())
        {
            return {iter.value()};
        }
    }

    return {};
}
