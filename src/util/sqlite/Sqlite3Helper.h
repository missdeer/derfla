#pragma once

#include <cstdint>
#include <map>
#include <string>

#include "Sqlite3Statement.h"
#include "util_global.h"

struct sqlite3;
struct sqlite3_stmt;

class UTILSHARED_EXPORT Sqlite3Helper
{
public:
    Sqlite3Helper(sqlite3 *&db);

    Sqlite3StatementPtr compile(const char *szSQL);
    Sqlite3StatementPtr compile(const std::string &sql);
    int                 execDML(const char *szSQL);
    int                 execDML(const std::string &sql);
    bool                isQueryOk(int result);
    bool                canQueryLoop(int result);

    int countRow(const char *szSQL);
    int countRow(const std::string &sql);

    bool isDatabaseOpened();

    int checkExists(const std::string &field, const std::string &name);

    bool createTablesAndIndexes(std::map<std::string, const char *> &tablesMap, std::map<std::string, const char *> &indexesMap);

    bool beginTransaction();
    bool endTransaction();
    bool rollbackTransaction();

    bool vacuum();

    std::int64_t lastInsertRowId();

private:
    sqlite3 *&m_db;
};