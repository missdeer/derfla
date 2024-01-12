#pragma once

#include <cstdint>
#include <map>
#include <string>

#include "Sqlite3Statement.h"
#include "util_global.h"

struct sqlite3;
struct sqlite3_stmt;
struct sqlite3_context;
struct sqlite3_value;

class UTILSHARED_EXPORT Sqlite3Helper
{
public:
    explicit Sqlite3Helper(sqlite3 *&db);

    Sqlite3StatementPtr compile(const char *szSQL);
    Sqlite3StatementPtr compile(const std::string &sql);
    Sqlite3StatementPtr compile(const QString &sql);
    int                 execDML(const char *szSQL);
    int                 execDML(const std::string &sql);
    int                 execDML(const QString &sql);
    static bool         isQueryOk(int result);
    static bool         isOk(int result);
    static bool         canQueryLoop(int result);

    int countRow(const char *szSQL);
    int countRow(const std::string &sql);
    int countRow(const QString &sql);

    bool isDatabaseOpened();

    int checkExists(const std::string &field, const std::string &name);
    int checkExists(const QString &field, const QString &name);

    bool createTablesAndIndexes(std::map<std::string, const char *> &tablesMap, std::map<std::string, const char *> &indexesMap);

    bool beginTransaction();
    bool endTransaction();
    bool rollbackTransaction();

    bool vacuum();

    std::int64_t lastInsertRowId();

    void registerCustomFunctions();

private:
    sqlite3   *&m_db;
    static void fileNotExists(sqlite3_context *ctx, int, sqlite3_value **argv);
};