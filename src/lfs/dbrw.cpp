#include "stdafx.h"
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include "dbrw.h"

using namespace rapidjson;

DBRW::DBRW()
{
    dbPath_ = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QDir d(dbPath_);
    if (!d.exists())
    {
        d.mkpath(dbPath_);
    }
    dbPath_.append("/cache.db");

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

QString DBRW::search(const QString &keyword, int countRequired)
{
    QString res;
	LocalFSItemList fsil; 
    getLFSItems(fsil, keyword, countRequired);

    if (!fsil.empty())
    {
        Document d;
        d.Parse("[]");
        Q_ASSERT(d.IsArray());

        Document::AllocatorType& a = d.GetAllocator();
        for (auto item : fsil)
        {
            Value o(kObjectType);
            o.AddMember(Value("title", a), Value(StringRef(item->title().toStdString().c_str()), a), a);
            o.AddMember(Value("description", a), Value(StringRef(item->description().toStdString().c_str()), a), a);
            o.AddMember(Value("target", a), Value(StringRef(item->target().toStdString().c_str()), a), a);
            o.AddMember(Value("arguments", a), Value(StringRef(item->arguments().toStdString().c_str()), a), a);
            o.AddMember(Value("workingDir", a), Value(StringRef(item->workingDirectory().toStdString().c_str()), a), a);
            o.AddMember(Value("actionType", a), Value(StringRef(item->actionType().toStdString().c_str()), a), a);
            QIcon icon = item->icon();
            auto allSizes = icon.availableSizes();
            if (!allSizes.isEmpty())
            {
                QSize size = allSizes.at(0);
                auto pixmap = icon.pixmap(size);
                QByteArray bytes;
                QBuffer buffer(&bytes);
                buffer.open(QIODevice::WriteOnly);
                pixmap.save(&buffer, "PNG");
                buffer.close();
                o.AddMember(Value("iconData", a), Value(QString(bytes.toBase64()).toStdString().c_str(), a), a);
            }
            d.PushBack(o, a);
        }

        StringBuffer buffer;
        Writer<StringBuffer, Document::EncodingType, ASCII<>> writer(buffer);
        d.Accept(writer);

        res = QString(buffer.GetString());
    }

    return res;
}

bool DBRW::getLFSItems(LocalFSItemList &fsil, const QString& keyword, int countRequired)
{
    QSqlDatabase db = QSqlDatabase::database(dbPath_, false);
    Q_ASSERT(db.isValid());
    Q_ASSERT(db.isOpen());

    QSqlQuery q(db);
	QString sql = QString("SELECT * FROM lfs WHERE title LIKE '%'||?||'%' LIMIT %1;").arg(countRequired + 10);
	if (!q.prepare(sql))
		return false;
    if (queryActions(fsil, keyword, countRequired, q))
        return true;

    sql = QString("SELECT * FROM lfs WHERE description LIKE '%'||?||'%' LIMIT %1;").arg(countRequired - fsil.length() + 10);
	if (!q.prepare(sql))
		return false; 
    if (queryActions(fsil, keyword, countRequired, q))
        return true;

    sql = QString("SELECT * FROM lfs WHERE target LIKE '%'||?||'%' LIMIT %1;").arg(countRequired - fsil.length() + 10);
	if (!q.prepare(sql))
		return false;
    if (queryActions(fsil, keyword, countRequired, q))
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
    QString sql = "INSERT INTO lfs (icon, title, allocatorescription, target, arguments, working_directory, timestamp, last_modified, type) "
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
    return query.exec("CREATE TABLE lfs(id INTEGER PRIMARY KEY AUTOINCREMENT,icon BLOB, title TEXT, allocatorescription TEXT,target TEXT, arguments TEXT, working_directory TEXT,timestamp DATETIME,last_modified DATETIME, type TEXT);");
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

bool DBRW::queryActions(LocalFSItemList &fsil, const QString &keyword, int countRequired, QSqlQuery &q)
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
            LocalFSItemPtr item(new LocalFSItem);
            QPixmap pixmap;
            pixmap.loadFromData(q.value(iconIndex).toByteArray());
            item->setIcon(QIcon(pixmap));
            item->setArguments(q.value(argumentsIndex).toString());
            item->setTarget(q.value(targetIndex).toString());
            QString workingDirectory = q.value(workingDirectoryIndex).toString();
            if (workingDirectory.isEmpty())
                workingDirectory = QFileInfo(item->target()).absolutePath();
            item->setWorkingDirectory(workingDirectory);
            item->setTitle(q.value(titleIndex).toString());
            item->setDescription(q.value(descriptionIndex).toString());
            item->setActionType(q.value(typeIndex).toString());
            auto it = std::find_if(fsil.begin(), fsil.end(), [item](LocalFSItemPtr d) {
                    return item->title() == d->title()
                    && item->description() == d->description();}
                    );
            if (fsil.end() == it)
                fsil.append(item);
        }
        q.clear();
        q.finish();

        if (fsil.length() >= countRequired)
        {
            while(fsil.length() > countRequired)
                fsil.removeLast();
            return true;
        }
    }
    return false;
}
