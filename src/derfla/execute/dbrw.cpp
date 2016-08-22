#include "stdafx.h"
#include "dbrw.h"

DBRW *DBRW::instance_ = nullptr;

DBRW* DBRW::instance()
{
    if (!instance_)
        instance_ = new DBRW;
    return instance_;
}

void DBRW::destroy()
{
    delete instance_;
    instance_ = nullptr;
}

bool DBRW::insertLFS(const QByteArray &icon, const QString &title, const QString &description, const QString &target, const QString &arguments, const QString workingDirectory, quint64 timestamp, quint64 lastModified, const QString &type)
{
    QSqlQuery query(db_);
    query.prepare("INSERT INTO lfs (icon, title, description, target, arguments, working_directory, timestamp, last_modified, type) "
        "VALUES (:icon, :title, :description, :target, :arguments, :working_directory, :timestamp, :last_modified, :type);");
    // save to database
    query.bindValue(":icon", icon);
    query.bindValue(":title", title);
    query.bindValue(":description", description);
    query.bindValue(":target", target);
    query.bindValue(":arguments", arguments);
    query.bindValue(":working_directory", workingDirectory);
    query.bindValue(":timestamp", timestamp);
    query.bindValue(":last_modified", lastModified);
    query.bindValue(":type", type);
    return query.exec();
}

DBRW::DBRW()
{
    dbPath_ = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/cache.db";
    if (!openDatabase())
    {
        qCritical() << "can't open cache database";
    }
}

DBRW::~DBRW()
{
    if (db_.isValid() && db_.isOpen())
    {
        db_.close();
    }
}

bool DBRW::createDatabase()
{
    db_ = QSqlDatabase::database(dbPath_, true);
    if (!db_.isValid()) {
        db_ = QSqlDatabase::addDatabase("QSQLITE", dbPath_);
    }

    db_.setDatabaseName(dbPath_);
    if (!db_.isOpen())
    {
        if (!db_.open())
        {
            return false;
        }
    }
    QSqlQuery query(db_);
    return query.exec("CREATE TABLE lfs(id INTEGER PRIMARY KEY AUTOINCREMENT,icon TEXT, title TEXT, description TEXT,target TEXT, arguments TEXT, working_directory TEXT,timestamp DATETIME,last_modified DATETIME, type TEXT);");
}

bool DBRW::openDatabase()
{
    if (!QFile::exists(dbPath_))
        return createDatabase();

    db_ = QSqlDatabase::database(dbPath_, true);
    if (!db_.isValid()) {
        db_ = QSqlDatabase::addDatabase("QSQLITE", dbPath_);
    }

    db_.setDatabaseName(dbPath_);

    if (db_.isOpen())
    {
        return true;
    }
    return db_.open();
}
