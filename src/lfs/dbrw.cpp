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

bool DBRW::getLFSActions(DerflaActionList &dal, const QString& keyword, int countRequired)
{
    QSqlDatabase db = QSqlDatabase::database(dbPath_, false);
    Q_ASSERT(db.isValid());
    Q_ASSERT(db.isOpen());

    QSqlQuery q(db);
	QString sql = QString("SELECT * FROM lfs WHERE title LIKE '%'||?||'%' LIMIT %1;").arg(countRequired + 10);
	if (!q.prepare(sql))
		return false;
    if (queryActions(dal, keyword, countRequired, q))
        return true;

	sql = QString("SELECT * FROM lfs WHERE description LIKE '%'||?||'%' LIMIT %1;").arg(countRequired - dal.length() + 10);
	if (!q.prepare(sql))
		return false; 
	if (queryActions(dal, keyword, countRequired, q))
        return true;

	sql = QString("SELECT * FROM lfs WHERE target LIKE '%'||?||'%' LIMIT %1;").arg(countRequired - dal.length() + 10);
	if (!q.prepare(sql))
		return false;
    if (queryActions(dal, keyword, countRequired, q))
        return true;

    return false;
}

bool DBRW::removeOldRecords(qint64 timestamp)
{
    QSqlQuery query(db_);
	if (!query.prepare("DELETE FROM lfs WHERE timestamp < :timestamp;"))
		return false;
    query.bindValue(":timestamp", timestamp);
    return query.exec();
}

bool DBRW::insertLFS(const QByteArray &icon, const QString &title, const QString &description, const QString &target, const QString &arguments, const QString workingDirectory, qint64 timestamp, qint64 lastModified, const QString &type)
{
    Q_ASSERT(db_.isValid());
    Q_ASSERT(db_.isOpen());
    QSqlQuery query(db_);
	QString sql = "INSERT INTO lfs (icon, title, description, target, arguments, working_directory, timestamp, last_modified, type) "
		"VALUES (:icon, :title, :description, :target, :arguments, :working_directory, :timestamp, :last_modified, :type);";
	if (!query.prepare(sql))
		return false;
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
    dbPath_ = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QDir d(dbPath_);
    if (!d.exists())
    {
        d.mkpath(dbPath_);
    }
    dbPath_.append("/cache.db");

    firstLaunch_ = !QFile::exists(dbPath_);

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
    return query.exec("CREATE TABLE lfs(id INTEGER PRIMARY KEY AUTOINCREMENT,icon BLOB, title TEXT, description TEXT,target TEXT, arguments TEXT, working_directory TEXT,timestamp DATETIME,last_modified DATETIME, type TEXT);");
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

bool DBRW::queryActions(DerflaActionList &dal, const QString &keyword, int countRequired, QSqlQuery &q)
{
    q.addBindValue(keyword);
    if (q.exec())
    {
        int iconIndex = q.record().indexOf("icon");
        int titleIndex = q.record().indexOf("title");
        int descriptionIndex = q.record().indexOf("description");
        int targetIndex = q.record().indexOf("target");
        int argumentsIndex = q.record().indexOf("arguments");
        int workingDirectoryIndex = q.record().indexOf("working_directory");
        int typeIndex = q.record().indexOf("type");
        while (q.next())
        {
            DerflaActionPtr da(new DerflaAction);
            QPixmap pixmap;
            pixmap.loadFromData(q.value(iconIndex).toByteArray());
            da->setIcon(QIcon(pixmap));
            da->setArguments(q.value(argumentsIndex).toString());
            da->setTarget(q.value(targetIndex).toString());
            QString workingDirectory = q.value(workingDirectoryIndex).toString();
            if (workingDirectory.isEmpty())
                workingDirectory = QFileInfo(da->target()).absolutePath();
            da->setWorkingDirectory(workingDirectory);
            da->setTitle(q.value(titleIndex).toString());
            da->setDescription(q.value(descriptionIndex).toString());
            da->setActionType(q.value(typeIndex).toString() == "c" ? DAT_CONSOLE : DAT_GUI);
            auto it = std::find_if(dal.begin(), dal.end(), [da](DerflaActionPtr d) {
                    return da->title() == d->title()
                    && da->description() == d->description();}
                    );
            if (dal.end() == it)
                dal.append(da);
        }
        q.clear();
        q.finish();

        if (dal.length() >= countRequired)
        {
            while(dal.length() > countRequired)
                dal.removeLast();
            return true;
        }
    }
    return false;
}
