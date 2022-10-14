#include "stdafx.h"

#include <filesystem>
#include <random>
#include <sqlite3.h>

#include "Sqlite3DBManager.h"
#include "Sqlite3Constants.h"

Sqlite3DBManager Sqlite3DBManager::m_instance;

bool Sqlite3DBManager::createTablesAndIndexes()
{
    // no additional tables or indexes are required
    return true;
}

void Sqlite3DBManager::regenerateSavePoint()
{
    std::string                     chars("abcdefghijklmnopqrstuvwxyz"
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
    std::string sql = "SAVEPOINT " + m_savePoint + ";";
    if (m_sqlite.execDML(sql) != SQLITE_OK)
    {
        // CUBELOG_ERROR("Cannot set savepoint.");
        return false;
    }
    // CUBELOG_INFO_FMT("set savepoint %s", m_savePoint.c_str());
    return true;
}

bool Sqlite3DBManager::releaseSavePoint()
{
    std::string sql = "RELEASE " + m_savePoint + ";";
    if (m_sqlite.execDML(sql) != SQLITE_OK)
    {
        // CUBELOG_ERROR("Cannot release savepoint.");
        return false;
    }
    // CUBELOG_INFO_FMT("released savepoint %s", m_savePoint.c_str());
    return true;
}

Sqlite3DBManager &Sqlite3DBManager::instance()
{
    return m_instance;
}

bool Sqlite3DBManager::open(const std::string &dbPath)
{
    if (sqlite3_open_v2(dbPath.c_str(), &m_db, SQLITE_OPEN_READWRITE, nullptr) != SQLITE_OK)
    {
        // CUBELOG_ERROR_FMT("Cannot open the database '%s'.", dbPath.c_str());
        return false;
    }

    regenerateSavePoint();
    setSavePoint();

    // CUBELOG_INFO_FMT("The database '%s' is opened.", dbPath.c_str());
    return true;
}

bool Sqlite3DBManager::close()
{
    if (!m_db)
    {
        // CUBELOG_WARN("database is not opened");
        return true;
    }
    int rc = sqlite3_close_v2(m_db);
    // CUBELOG_DEBUG_FMT("Close db result: %d - %s", rc, (const char *)sqlite3_errmsg(m_db));
    while (rc == SQLITE_BUSY)
    {
        sqlite3_stmt *stmt = sqlite3_next_stmt(m_db, nullptr);
        if (stmt)
        {
            rc = sqlite3_finalize(stmt);
            if (rc == SQLITE_OK)
            {
                rc = sqlite3_close_v2(m_db);
                // CUBELOG_DEBUG_FMT("Secondary try closing db result: %d - %s", rc, (const char *)sqlite3_errmsg(m_db));
            }
        }
    }

    if (rc != SQLITE_OK)
    {
        // CUBELOG_ERROR_FMT("Closing db failed: %d - %s", rc, (const char *)sqlite3_errmsg(m_db));
        return false;
    }

    m_db = nullptr;
    // CUBELOG_DEBUG("Closed database");
    return true;
}

bool Sqlite3DBManager::save()
{
    releaseSavePoint();
    // always set a new save point
    regenerateSavePoint();
    return setSavePoint();
}

bool Sqlite3DBManager::create(const std::string &dbPath)
{
    if (sqlite3_open_v2(dbPath.c_str(), &m_db, SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE, nullptr) != SQLITE_OK)
    {
        // CUBELOG_ERROR_FMT("Cannot create the database '%s'", dbPath.c_str());
        return false;
    }
    // CUBELOG_INFO_FMT("The database is created at '%s'", dbPath.c_str());

    // CUBELOG_INFO("set sqlite options for performance issue");

    if (m_sqlite.execDML(SQL_STATEMENT_CACHE_SIZE) != SQLITE_OK)
    {
        // CUBELOG_ERROR("Cannot set cache size.");
        return false;
    }

    if (m_sqlite.execDML(SQL_STATEMENT_ENABLE_FOREIGN_KEYS) != SQLITE_OK)
    {
        // CUBELOG_ERROR("Cannot enable the foreign keys.");
        return false;
    }

    regenerateSavePoint();
    setSavePoint();

    return createTablesAndIndexes();
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
