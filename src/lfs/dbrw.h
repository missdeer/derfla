#ifndef DBRW_H
#define DBRW_H

#include <functional>

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>

#include "localfsitem.h"

class DBRW : public QObject
{
    Q_OBJECT
public:
    DBRW();
    ~DBRW();

    QString search(const QString &keyword, int countRequired = 50);
    bool    removeOldRecords(qint64 timestamp);
    bool    insertLFS(const QByteArray &icon,
                      const QString    &title,
                      const QString    &description,
                      const QString    &target,
                      const QString    &arguments,
                      const QString     workingDirectory,
                      qint64            timestamp,
                      qint64            lastModified,
                      const QString    &type);
signals:

public slots:

private:
    QString      dbPath_;
    QSqlDatabase db_;
    bool         createDatabase();
    bool         openDatabase();
    bool         getLFSItems(LocalFSItemList &fsil, const QString &keyword, int countRequired = 50);
    bool         queryActions(LocalFSItemList &fsil, const QString &keyword, int countRequired, QSqlQuery &q);
};

typedef std::function<bool(
    const QByteArray &, const QString &, const QString &, const QString &, const QString &, const QString &, qint64, qint64, const QString &)>
    LFSInserter;

#endif // DBRW_H
