#include "stdafx.h"

#include <filesystem>
#include <random>
#include <sqlite3.h>

#include "Sqlite3DBManager.h"
#include "Sqlite3Constants.h"

Sqlite3DBManager Sqlite3DBManager::m_instance;

void Sqlite3DBManager::regenerateSavePoint()
{
    static const std::string        chars("abcdefghijklmnopqrstuvwxyz"
                                          "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                          "1234567890");
    std::random_device              rng;
    std::uniform_int_distribution<> index_dist(0, chars.size() - 1);
    std::string                     res;
    const int                       length = 16;
    for (int i = 0; i < length; i++)
    {
        res.append(1, chars[index_dist(rng)]);
    }
    m_savePoint = res;
}

void Sqlite3DBManager::clearSavePoint()
{
    m_savePoint.clear();
}

bool Sqlite3DBManager::setSavePoint()
{
    std::string sql = "SAVEPOINT '" + m_savePoint + "';";
    if (m_sqlite.execDML(sql) != SQLITE_OK)
    {
        // CUBELOG_ERROR("Cannot set savepoint.");
        qCritical() << "Cannot set savepoint.";
        return false;
    }
    // CUBELOG_INFO_FMT("set savepoint %s", m_savePoint.c_str());
    qDebug() << "set savepoint" << QString::fromStdString(m_savePoint);
    return true;
}

bool Sqlite3DBManager::releaseSavePoint()
{
    std::string sql = "RELEASE '" + m_savePoint + "';";
    if (m_sqlite.execDML(sql) != SQLITE_OK)
    {
        // CUBELOG_ERROR("Cannot release savepoint.");
        qCritical() << "Cannot release savepoint.";
        return false;
    }
    // CUBELOG_INFO_FMT("released savepoint %s", m_savePoint.c_str());
    qDebug() << "released savepoint" << QString::fromStdString(m_savePoint);
    return true;
}

Sqlite3DBManager &Sqlite3DBManager::instance()
{
    return m_instance;
}

bool Sqlite3DBManager::open(const QString &dbPath, bool readOnly)
{
    return open(dbPath.toUtf8().constData(), readOnly);
}

bool Sqlite3DBManager::open(const std::string &dbPath, bool readOnly)
{
    return open(dbPath.c_str(), readOnly);
}

bool Sqlite3DBManager::open(const char *dbPath, bool readOnly)
{
    if (sqlite3_open_v2(dbPath, &m_db, (readOnly ? SQLITE_OPEN_READONLY : SQLITE_OPEN_READWRITE), nullptr) != SQLITE_OK)
    {
        // CUBELOG_ERROR_FMT("Cannot open the database '%s'.", dbPath.c_str());
        qCritical() << "Cannot open the database" << QString::fromUtf8(dbPath);
        return false;
    }

    Q_ASSERT(m_db);

    m_sqlite.registerCustomFunctions();

    if (!readOnly)
    {
        regenerateSavePoint();
        setSavePoint();
    }

    // CUBELOG_INFO_FMT("The database '%s' is opened.", dbPath.c_str());
    qDebug() << "The database" << QString::fromStdString(dbPath) << "is opened";
    return true;
}

bool Sqlite3DBManager::close()
{
    if (!m_db)
    {
        // CUBELOG_WARN("database is not opened");
        qWarning() << "database is not opened";
        return true;
    }
    int result = sqlite3_close_v2(m_db);
    while (result == SQLITE_BUSY)
    {
        // CUBELOG_DEBUG_FMT("Close db result: %d - %s", rc, (const char *)sqlite3_errmsg(m_db));
        qDebug() << "Close db result: " << result << (const char *)sqlite3_errmsg(m_db);
        sqlite3_stmt *stmt = sqlite3_next_stmt(m_db, nullptr);
        if (stmt)
        {
            result = sqlite3_finalize(stmt);
            if (result == SQLITE_OK)
            {
                result = sqlite3_close_v2(m_db);
                // CUBELOG_DEBUG_FMT("Secondary try closing db result: %d - %s", rc, (const char *)sqlite3_errmsg(m_db));
                qDebug() << "Secondary try closing db result: " << result << (const char *)sqlite3_errmsg(m_db);
            }
        }
    }

    if (result != SQLITE_OK)
    {
        // CUBELOG_ERROR_FMT("Closing db failed: %d - %s", rc, (const char *)sqlite3_errmsg(m_db));
        qCritical() << "Closing db failed:" << result << (const char *)sqlite3_errmsg(m_db);
        return false;
    }

    m_db = nullptr;
    // CUBELOG_DEBUG("Closed database");
    qDebug() << "database closed";
    return true;
}

bool Sqlite3DBManager::save()
{
    releaseSavePoint();
    // always set a new save point
    regenerateSavePoint();
    return setSavePoint();
}

bool Sqlite3DBManager::create(const QString &dbPath)
{
    return create(dbPath.toUtf8().constData());
}

bool Sqlite3DBManager::create(const std::string &dbPath)
{
    return create(dbPath.c_str());
}

bool Sqlite3DBManager::create(const char *dbPath)
{
    if (sqlite3_open_v2(dbPath, &m_db, SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE, nullptr) != SQLITE_OK)
    {
        // CUBELOG_ERROR_FMT("Cannot create the database '%s'", dbPath.c_str());
        qCritical() << "Cannot create the database" << QString::fromUtf8(dbPath);
        return false;
    }

    Q_ASSERT(m_db);

    // CUBELOG_INFO_FMT("The database is created at '%s'", dbPath.c_str());
    qInfo() << "The database is created at" << QString::fromStdString(dbPath);

    // CUBELOG_INFO("set sqlite options for performance issue");
    qDebug() << "set sqlite options for performance issue";

    if (m_sqlite.execDML(SQL_STATEMENT_CACHE_SIZE) != SQLITE_OK)
    {
        // CUBELOG_ERROR("Cannot set cache size.");
        qCritical() << "Cannot set cache size.";
        return false;
    }

    if (m_sqlite.execDML(SQL_STATEMENT_ENABLE_FOREIGN_KEYS) != SQLITE_OK)
    {
        // CUBELOG_ERROR("Cannot enable the foreign keys.");
        qCritical() << "Cannot enable the foreign keys.";
        return false;
    }

    m_sqlite.registerCustomFunctions();

    regenerateSavePoint();
    setSavePoint();

    return true;
}

void Sqlite3DBManager::lock()
{
    m_mutex.lock();
}

void Sqlite3DBManager::unlock()
{
    m_mutex.unlock();
}

Sqlite3Helper &Sqlite3DBManager::engine()
{
    return m_sqlite;
}

Sqlite3DBManager::~Sqlite3DBManager()
{
    close();
}
bool Sqlite3DBManager::isOpened() const
{
    return m_db != nullptr;
}
