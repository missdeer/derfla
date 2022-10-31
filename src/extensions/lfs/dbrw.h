#ifndef DBRW_H
#define DBRW_H

#include <functional>

#include <QObject>

#include "Sqlite3Statement.h"
#include "localfsitem.h"

class DBRW : public QObject
{
    Q_OBJECT
public:
    DBRW();
    ~DBRW() override;

    QString search(const QString &keyword, int countRequired = 50);
    bool    removeInvalidRecords();
    bool    insertLFS(const QByteArray &icon,
                      const QString    &title,
                      const QString    &description,
                      const QString    &target,
                      const QString    &arguments,
                      const QString    &workingDirectory,
                      const QString    &type);
    void    save();
signals:

public slots:

private:
    QString dbPath_;
    bool    createDatabase();
    bool    openDatabase();
    bool    getLFSItems(LocalFSItemList &fsil, const QString &keyword, int countRequired = 50);
    bool    queryActions(LocalFSItemList &fsil, int countRequired, Sqlite3StatementPtr &query);
};

using LFSInserter =
    std::function<bool(const QByteArray &, const QString &, const QString &, const QString &, const QString &, const QString &, const QString &)>;

#endif // DBRW_H
