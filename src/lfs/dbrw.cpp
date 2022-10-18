#include "stdafx.h"

#include "dbrw.h"
#include "Sqlite3DBManager.h"

DBRW::DBRW(bool readOnly)
{
    dbPath_ = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QDir dir(dbPath_);
    if (!dir.exists())
    {
        dir.mkpath(dbPath_);
    }
    dbPath_ = QDir::toNativeSeparators(dir.absoluteFilePath("cache.db"));

    if (!openDatabase(readOnly))
    {
        qCritical() << "can't open cache database";
    }
}

DBRW::~DBRW()
{
    auto &dbMgr = Sqlite3DBManager::instance();
    if (dbMgr.isOpened())
    {
        dbMgr.close();
    }
}

QString DBRW::search(const QString &keyword, int countRequired)
{
    LocalFSItemList fsil;
    getLFSItems(fsil, keyword, countRequired);

    QJsonDocument doc = QJsonDocument::fromJson("[]");
    Q_ASSERT(doc.isArray());
    QJsonArray arr = doc.array();
    for (const auto &item : fsil)
    {
        QVariantMap varMap;
        varMap.insert("title", item->title());
        if (item->description() != item->title())
        {
            varMap.insert("description", item->description());
        }
        else
        {
            varMap.insert("description", item->target());
        }
        varMap.insert("target", item->target());
        varMap.insert("arguments", item->arguments());
        varMap.insert("workingDir", item->workingDirectory());
        varMap.insert("actionType", item->actionType());
        QIcon icon     = item->icon();
        auto  allSizes = icon.availableSizes();
        if (!allSizes.isEmpty())
        {
            QSize      size   = allSizes.at(0);
            auto       pixmap = icon.pixmap(size);
            QByteArray bytes;
            QBuffer    buffer(&bytes);
            buffer.open(QIODevice::WriteOnly);
            pixmap.save(&buffer, "PNG");
            buffer.close();
            varMap.insert("iconData", bytes.toBase64());
        }
        arr.append(QJsonObject::fromVariantMap(varMap));
    }
    doc.setArray(arr);
    return doc.toJson(QJsonDocument::Compact);
}

bool DBRW::getLFSItems(LocalFSItemList &fsil, const QString &keyword, int countRequired)
{
    auto &dbMgr = Sqlite3DBManager::instance();
    Q_ASSERT(dbMgr.isOpened());
    auto &engine = dbMgr.engine();

    QString sql = QString("SELECT icon, title, description, target, arguments, working_directory, type FROM lfs WHERE title LIKE '%%1%' LIMIT %2;")
                      .arg(keyword)
                      .arg(countRequired + 10);
    auto stmt = engine.compile(sql);
    if (!stmt || !stmt->isValid())
    {
        return false;
    }
    if (queryActions(fsil, countRequired, stmt))
    {
        return true;
    }

    sql = QString("SELECT icon, title, description, target, arguments, working_directory, type FROM lfs WHERE description LIKE '%%1%' LIMIT %2;")
              .arg(keyword)
              .arg(countRequired - fsil.length() + 10);
    stmt = engine.compile(sql);
    if (!stmt || !stmt->isValid())
    {
        return false;
    }
    if (queryActions(fsil, countRequired, stmt))
    {
        return true;
    }

    sql = QString("SELECT icon, title, description, target, arguments, working_directory, type FROM lfs WHERE target LIKE '%%1%' LIMIT %2;")
              .arg(keyword)
              .arg(countRequired - fsil.length() + 10);
    stmt = engine.compile(sql);
    if (!stmt || !stmt->isValid())
    {
        return false;
    }
    return queryActions(fsil, countRequired, stmt);
}

bool DBRW::insertLFS(const QByteArray &icon,
                     const QString    &title,
                     const QString    &description,
                     const QString    &target,
                     const QString    &arguments,
                     const QString    &workingDirectory,
                     const QString    &type)
{
    auto       &engine = Sqlite3DBManager::instance().engine();
    const char *sql    = "INSERT INTO lfs (icon, title, description, target, arguments, working_directory, last_modified, type) "
                         "SELECT icon, title, description, target, arguments, working_directory, last_modified, type "
                         "FROM ( SELECT :icon AS icon, :title AS title, :description AS description, :target AS target, :arguments AS arguments, "
                         ":working_directory AS working_directory, unixepoch(datetime('now', 'localtime')) AS last_modified, :type AS type ) t "
                         "WHERE NOT EXISTS (SELECT 1 FROM lfs WHERE lfs.title = t.title AND lfs.description = t.description AND lfs.target = t.target "
                         "AND lfs.arguments = t.arguments AND lfs.working_directory = t.working_directory AND lfs.type = t.type);";
    auto        stmt   = engine.compile(sql);
    if (!stmt || !stmt->isValid())
    {
        return false;
    }
    // save to database
    stmt->bind(":icon", icon);
    stmt->bind(":title", title);
    stmt->bind(":description", description);
    stmt->bind(":target", target);
    stmt->bind(":arguments", arguments);
    stmt->bind(":working_directory", workingDirectory);
    stmt->bind(":type", type);
    if (stmt->execDML() > 0)
    {
        Sqlite3DBManager::instance().save();
        return true;
    }
    return false;
}

bool DBRW::createDatabase()
{
    auto &dbMgr = Sqlite3DBManager::instance();
    if (!dbMgr.create(dbPath_))
    {
        return false;
    }
    Q_ASSERT(dbMgr.isOpened());
    auto &engine = dbMgr.engine();
    auto  result = engine.execDML(
        "CREATE TABLE lfs(id INTEGER PRIMARY KEY AUTOINCREMENT,icon BLOB, title TEXT, description TEXT,target TEXT, arguments TEXT, "
         "working_directory TEXT,timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,last_modified DATETIME DEFAULT CURRENT_TIMESTAMP, type TEXT);");
    if (Sqlite3Helper::isOk(result))
    {
        dbMgr.save();
        return true;
    }
    return false;
}

bool DBRW::openDatabase(bool readOnly)
{
    if (!readOnly && !QFile::exists(dbPath_))
    {
        return createDatabase();
    }

    return Sqlite3DBManager::instance().open(dbPath_, readOnly);
}

bool DBRW::queryActions(LocalFSItemList &fsil, int countRequired, Sqlite3StatementPtr &stmt)
{
    auto &engine = Sqlite3DBManager::instance().engine();

    bool eof  = false;
    int  nRet = 0;
    do
    {
        nRet = stmt->execQuery(eof);
        if (Sqlite3Helper::isQueryOk(nRet))
        {
            while (!eof)
            {
                LocalFSItemPtr item(new LocalFSItem);
                QPixmap        pixmap;
                pixmap.loadFromData(stmt->getBlob(0));
                item->setIcon(QIcon(pixmap));
                item->setTitle(stmt->getQString(1));
                item->setDescription(stmt->getQString(2));
                item->setTarget(stmt->getQString(3));
                item->setArguments(stmt->getQString(4));
                QString workingDirectory = stmt->getQString(5);
                if (workingDirectory.isEmpty())
                {
                    workingDirectory = QFileInfo(item->target()).absolutePath();
                }
                item->setWorkingDirectory(workingDirectory);
                item->setActionType(stmt->getQString(6));
                qDebug() << item->title() << item->description() << item->target() << item->arguments() << item->workingDirectory()
                         << item->actionType();
                auto iter = std::find_if(fsil.begin(), fsil.end(), [&item](const auto &d) {
                    return item->title() == d->title() && item->description() == d->description();
                });
                if (fsil.end() == iter)
                {
                    fsil.append(item);
                }
                stmt->nextRow(eof);
            }
        }
    } while (Sqlite3Helper::canQueryLoop(nRet));

    if (fsil.length() >= countRequired)
    {
        while (fsil.length() > countRequired)
        {
            fsil.removeLast();
        }
        return true;
    }

    return false;
}
bool DBRW::removeInvalidRecords()
{
    auto &engine = Sqlite3DBManager::instance().engine();
    auto  stmt   = engine.compile("DELETE FROM lfs WHERE file_not_exists(target);");
    if (!stmt->isValid())
    {
        qCritical() << "compiling sql failed";
        return false;
    }

    if (stmt->execDML() < 0)
    {
        qCritical() << "deleting records that target not exists failed";
        return false;
    }
    return true;
}
