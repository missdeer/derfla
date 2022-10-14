#pragma once

#include <cstdint>
#include <mutex>

#include "Sqlite3Helper.h"
#include "util_global.h"

struct sqlite3;

class UTILSHARED_EXPORT Sqlite3DBManager
{
public:
    static Sqlite3DBManager &instance();

    bool open(const std::string &dbPath);
    bool close();
    bool save();
    bool create(const std::string &dbPath);
    void lock();
    void unlock();

    Sqlite3Helper &engine();

private:
    Sqlite3DBManager() : m_sqlite(m_db) {}
    ~Sqlite3DBManager();

    static Sqlite3DBManager m_instance;

    sqlite3      *m_db {nullptr};
    Sqlite3Helper m_sqlite;
    std::mutex    m_mutex;
    std::string   m_savePoint;
    bool          createTablesAndIndexes();
    void          regenerateSavePoint();
    void          clearSavePoint();
    bool          setSavePoint();
    bool          releaseSavePoint();
};