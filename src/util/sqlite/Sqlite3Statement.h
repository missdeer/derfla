#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>

#include "util_global.h"

struct sqlite3;
struct sqlite3_stmt;

class Sqlite3Helper;

class UTILSHARED_EXPORT Sqlite3Statement
{
    friend class Sqlite3Helper;

public:
    Sqlite3Statement(sqlite3 *&db, sqlite3_stmt *pVM);

    void bind(int nParam, const char *szValue, int nLen = -1);
    void bind(int nParam, const std::string &szValue);
    void bind(int nParam, const std::string_view &szValue);
    void bind(int nParam, const int nValue);
    void bind(int nParam, const int64_t nValue);
    void bind(int nParam, const double dValue);
    void bind(int nParam, const unsigned char *blobValue, int nLen);
    void bindNull(int nParam);
    int  bindParameterIndex(const char *szParam);
    void bind(const char *szParam, const char *szValue, int nLen = -1);
    void bind(const char *szParam, const std::string &szValue);
    void bind(const char *szParam, const std::string_view &szValue);
    void bind(const char *szParam, const int nValue);
    void bind(const char *szParam, const int64_t nValue);
    void bind(const char *szParam, const double dwValue);
    void bind(const char *szParam, const unsigned char *blobValue, int nLen);
    void bindNull(const char *szParam);
    int  bindParameterIndex(const std::string &szParam);
    void bind(const std::string &szParam, const std::string &szValue);
    void bind(const std::string &szParam, const std::string_view &szValue);
    void bind(const std::string &szParam, const int nValue);
    void bind(const std::string &szParam, const int64_t nValue);
    void bind(const std::string &szParam, const double dwValue);
    void bind(const std::string &szParam, const unsigned char *blobValue, int nLen);
    void bindNull(const std::string &szParam);

    int execDML();
    int execQuery(bool &eof);
    int nextRow(bool &eof);
    int endQuery();

    int          getInt(int column);
    double       getDouble(int column);
    std::int64_t getInt64(int column);
    std::string  getString(int column);

    std::string getLastErrorString();

    int countRow();

    bool isValid();

private:
    sqlite3     *&m_db;
    sqlite3_stmt *m_pVM;

    bool isDatabaseOpened();
};

using Sqlite3StatementPtr = std::shared_ptr<Sqlite3Statement>;
