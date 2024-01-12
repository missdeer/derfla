#include "stdafx.h"

#include <chrono>
#include <vector>
#include <sqlite3.h>

#include <QFile>
#include <QtCore>

#include "Sqlite3Helper.h"
#include "Sqlite3Constants.h"

Sqlite3Helper::Sqlite3Helper(sqlite3 *&db) : m_db(db) {}

Sqlite3StatementPtr Sqlite3Helper::compile(const char *szSQL)
{
    if (!m_db)
    {
        qCritical() << szSQL << "null db pointer";
        return nullptr;
    }

    sqlite3_stmt *pVM = nullptr;
    int           res = sqlite3_prepare(m_db, szSQL, -1, &pVM, nullptr);
    if (res != SQLITE_OK)
    {
        const char *szError = (const char *)sqlite3_errmsg(m_db);
        qCritical() << "prepare SQL statement" << szSQL << "failed:" << res << "-" << szError;
        return nullptr;
    }

    return std::make_shared<Sqlite3Statement>(m_db, pVM);
}

Sqlite3StatementPtr Sqlite3Helper::compile(const std::string &sql)
{
    return compile(sql.c_str());
}

Sqlite3StatementPtr Sqlite3Helper::compile(const QString &sql)
{
    return compile(sql.toStdString().c_str());
}

int Sqlite3Helper::execDML(const char *szSQL)
{
    int nRet = SQLITE_OK;

    do
    {
        auto stmt = compile(szSQL);
        if (!stmt || !stmt->isValid())
        {
            qCritical() << szSQL << "null pVM, quit";
            return SQLITE_ERROR;
        }

        nRet = sqlite3_step(stmt->m_pVM);

        if (nRet == SQLITE_ERROR)
        {
            qCritical() << "step SQL statement " << szSQL << "failed:" << nRet << "-" << (const char *)sqlite3_errmsg(m_db);
            sqlite3_finalize(stmt->m_pVM);
            break;
        }
        nRet = sqlite3_finalize(stmt->m_pVM);
    } while (nRet == SQLITE_SCHEMA);

    return nRet;
}

int Sqlite3Helper::execDML(const std::string &sql)
{
    return execDML(sql.c_str());
}

int Sqlite3Helper::execDML(const QString &sql)
{
    return execDML(sql.toStdString().c_str());
}

bool Sqlite3Helper::isQueryOk(int result)
{
    return (result == SQLITE_DONE || result == SQLITE_ROW);
}

bool Sqlite3Helper::isOk(int result)
{
    return result == SQLITE_OK;
}

bool Sqlite3Helper::canQueryLoop(int result)
{
    return result == SQLITE_SCHEMA;
}

int Sqlite3Helper::countRow(const char *szSQL)
{
    int  count = 0;
    int  nRet  = 0;
    bool eof   = false;
    do
    {
        auto stmt = compile(szSQL);
        nRet      = stmt->execQuery(eof);
        if (nRet == SQLITE_DONE || nRet == SQLITE_ROW)
        {
            while (!eof)
            {
                count = stmt->getInt(0);
                stmt->nextRow(eof);
            }
            break;
        }
    } while (nRet == SQLITE_SCHEMA);
    return count;
}

int Sqlite3Helper::countRow(const std::string &sql)
{
    return countRow(sql.c_str());
}

int Sqlite3Helper::countRow(const QString &sql)
{
    return countRow(sql.toStdString().c_str());
}

bool Sqlite3Helper::isDatabaseOpened()
{
    return m_db != nullptr;
}

int Sqlite3Helper::checkExists(const std::string &field, const std::string &name)
{
    bool eof  = false;
    int  nRet = 0;
    do
    {
        auto stmt = compile(SQL_STATEMENT_SELECT_SQLITE_MASTER);
        if (!stmt || !stmt->isValid())
        {
            return -1;
        }
        stmt->bind(1, field.c_str());
        stmt->bind(2, name.c_str());
        nRet = stmt->execQuery(eof);
        if (nRet == SQLITE_DONE || nRet == SQLITE_ROW)
        {
            int size = 0;
            while (!eof)
            {
                size = 1;
                if (!stmt->nextRow(eof))
                    return -2;
            }

            if (size > 0)
            {
                qDebug() << "found expected" << QString::fromStdString(field) << ":" << QString::fromStdString(name);
                return size;
            }

            qDebug() << "not found expected" << QString::fromStdString(field) << ":" << QString::fromStdString(name);
            return 0;
        }
    } while (nRet == SQLITE_SCHEMA);

    return 0;
}

int Sqlite3Helper::checkExists(const QString &field, const QString &name)
{
    return checkExists(field.toStdString(), name.toStdString());
}

bool Sqlite3Helper::createTablesAndIndexes(std::map<std::string, const char *> &tablesMap, std::map<std::string, const char *> &indexesMap)
{
    for (auto &[name, sql] : tablesMap)
    {
        int res = checkExists("table", name);
        if (res < 0)
        {
            // do repair database file
            return false;
        }

        if (!res)
        {
            if (execDML(sql) != SQLITE_OK)
            {
                const char *szError = sqlite3_errmsg(m_db);
                return false;
            }
        }
    }

    for (auto &[name, sql] : indexesMap)
    {
        int res = checkExists("index", name);
        if (res < 0)
        {
            // do repair database file
            return false;
        }

        if (!res)
        {
            if (execDML(sql) != SQLITE_OK)
            {
                const char *szError = sqlite3_errmsg(m_db);
                return false;
            }
        }
    }

    return true;
}

bool Sqlite3Helper::beginTransaction()
{
    return execDML(SQL_STATEMENT_BEGIN_TRANSACTION) == SQLITE_OK;
}

bool Sqlite3Helper::endTransaction()
{
    return execDML(SQL_STATEMENT_COMMIT_TRANSACTION) == SQLITE_OK;
}

bool Sqlite3Helper::rollbackTransaction()
{
    return execDML(SQL_STATEMENT_ROLLBACK_TRANSACTION) == SQLITE_OK;
}

bool Sqlite3Helper::vacuum()
{
    return execDML(SQL_STATEMENT_VACUUM) == SQLITE_OK;
}

std::int64_t Sqlite3Helper::lastInsertRowId()
{
    if (m_db)
    {
        return sqlite3_last_insert_rowid(m_db);
    }
    return -1;
}
void Sqlite3Helper::fileNotExists(sqlite3_context *ctx, int, sqlite3_value **argv)
{
    QString filePath(reinterpret_cast<const char *>(sqlite3_value_text(argv[0])));
    bool    notExists = !QFile::exists(filePath);

    if (notExists)
    {
        sqlite3_result_int(ctx, 1);
    }
    else
    {
        sqlite3_result_int(ctx, 0);
    }
}
void Sqlite3Helper::registerCustomFunctions()
{
    sqlite3_create_function_v2(m_db, "file_not_exists", 1, SQLITE_UTF8 | SQLITE_DETERMINISTIC, this, &Sqlite3Helper::fileNotExists, NULL, NULL, NULL);
}
