#include "stdafx.h"
#include "luavm.h"


LuaVM::LuaVM(lua_State *L)
    : m_L(L)
{

}

void LuaVM::start()
{
    m_L = luaL_newstate();

    luaL_openlibs(m_L);

    loadBuiltinTemplate();

    setupBuiltinValues();
}

void LuaVM::shutdown()
{
    if (m_L)
    {
#if !defined(Q_OS_WIN) && !defined(__clang__)
        lua_close(m_L);
#endif
        m_L = nullptr;
    }    
}

bool LuaVM::doFile(const QString &file)
{
    if (!m_L)
        return false;

    int status = luaL_loadfile(m_L, file.toStdString().c_str());
    if (status)
    {
        qDebug() << "Couldn't load file:" << lua_tostring(m_L, -1);
        lua_pop(m_L, 1);
        return false;
    }

    status = lua_pcall(m_L, 0, 0, 0);
    if (status)
    {
        qDebug() << "Couldn't run file:" << lua_tostring(m_L, -1);
        lua_pop(m_L, 1);
        return false;
    }
    return true;
}

bool LuaVM::doScript(const QString &script)
{
    if (!m_L)
        return false;
    int status = luaL_loadbuffer(m_L, script.toStdString().c_str(), script.toStdString().size(), "theme");
    if (status)
    {
        qDebug() << "Couldn't load script:" << lua_tostring(m_L, -1);
        lua_pop(m_L, 1);
        return false;
    }

    status = lua_pcall(m_L, 0, 0, 0);
    if (status)
    {
        qDebug() << "Couldn't run script:" << lua_tostring(m_L, -1);
        lua_pop(m_L, 1);
        return false;
    }
    return true;
}

double LuaVM::getDouble(const QString &name)
{
    if (!m_L)
        return 0.0;

    lua_getglobal(m_L, name.toStdString().c_str());

    if (!lua_isnumber(m_L, -1))
    {
        qDebug() << name << " is expected to be a number";
        return 0.0;
    }

    double result = lua_tonumber(m_L, -1);
    lua_pop(m_L, 1);

    return result;
}

float LuaVM::getFloat(const QString &name)
{
    if (!m_L)
        return 0.0;

    lua_getglobal(m_L, name.toStdString().c_str());

    if (!lua_isnumber(m_L, -1))
    {
        qDebug() << name << " is expected to be a number";
        return 0.0;
    }

    float result = (float)lua_tonumber(m_L, -1);
    lua_pop(m_L, 1);

    return result;
}

int LuaVM::getInt(const QString &name)
{
    if (!m_L)
        return 0;

    lua_getglobal(m_L, name.toStdString().c_str());

    if (!lua_isinteger(m_L, -1))
    {
        qDebug() << name << " is expected to be a number";
        return 0;
    }

    int result = (int)lua_tointeger(m_L, -1);
    lua_pop(m_L, 1);

    return result;
}

long long LuaVM::getLongLong(const QString &name)
{
    if (!m_L)
        return 0;

    lua_getglobal(m_L, name.toStdString().c_str());

    if (!lua_isinteger(m_L, -1))
    {
        qDebug() << name << " is expected to be a number";
        return 0;
    }

    long long result = (long long)lua_tointeger(m_L, -1);
    lua_pop(m_L, 1);

    return result;
}

bool LuaVM::getBool(const QString &name)
{
    if (!m_L)
        return false;

    lua_getglobal(m_L, name.toStdString().c_str());

    if (!lua_isboolean(m_L, -1))
    {
        qDebug() << name << " is expected to be a boolean";
        return false;
    }

    int result = lua_toboolean(m_L, -1);
    lua_pop(m_L, 1);

    return !!result;
}

QString LuaVM::getString(const QString &name)
{
    if (!m_L)
        return "";

    lua_getglobal(m_L, name.toStdString().c_str());

    if (!lua_isstring(m_L, -1))
    {
        qDebug() << name << " is expected to be a string";
        return "";
    }

    size_t resultLen = 0;
    const char * result = lua_tolstring(m_L, -1, &resultLen);
    QString str = QString::fromLatin1(result, (int)resultLen);
    lua_pop(m_L, 1);

    return str;
}

double LuaVM::getDouble(const QString &table, const QString &name)
{
    if (!m_L)
        return 0.0;

    lua_getglobal(m_L, table.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qDebug() << table << " is expected to be a table";
        return 0.0;
    }

    lua_pushstring(m_L, name.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get table[name] */

    if (!lua_isnumber(m_L, -1))
    {
        qDebug() << name << " is expected to be a number";
        return 0.0;
    }

    double result = lua_tonumber(m_L, -1);
    lua_pop(m_L, 1); // remove the result
    lua_pop(m_L, 1); // remove table

    return result;
}

double LuaVM::getDouble(const QString &t1, const QString &t2, const QString &name)
{
    if (!m_L)
        return 0.0;

    lua_getglobal(m_L, t1.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t1 << " is expected to be a table";
        return 0.0;
    }

    lua_pushstring(m_L, t2.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t2 << " is expected to be a table";
        return 0.0;
    }

    lua_pushstring(m_L, name.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t2[name] */

    if (!lua_isnumber(m_L, -1))
    {
        qDebug() << name << " is expected to be a number";
        return 0.0;
    }

    double result = lua_tonumber(m_L, -1);
    lua_pop(m_L, 1); // remove the result
    lua_pop(m_L, 1); // remove t2
    lua_pop(m_L, 1); // remove t1

    return result;
}

double LuaVM::getDouble(const QString &t1, const QString &t2, const QString &t3, const QString &name)
{
    if (!m_L)
        return 0.0;

    lua_getglobal(m_L, t1.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t1 << " is expected to be a table";
        return 0.0;
    }

    lua_pushstring(m_L, t2.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t2 << " is expected to be a table";
        return 0.0;
    }

    lua_pushstring(m_L, t3.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t2[t3] */

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t3 << " is expected to be a table";
        return 0.0;
    }

    lua_pushstring(m_L, name.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t3[name] */

    if (!lua_isnumber(m_L, -1))
    {
        qDebug() << name << " is expected to be a number";
        return 0.0;
    }

    double result = lua_tonumber(m_L, -1);
    lua_pop(m_L, 1); // remove the result
    lua_pop(m_L, 1); // remove t3
    lua_pop(m_L, 1); // remove t2
    lua_pop(m_L, 1); // remove t1

    return result;
}

float LuaVM::getFloat(const QString &table, const QString &name)
{
    if (!m_L)
        return 0.0;

    lua_getglobal(m_L, table.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qDebug() << table << " is expected to be a table";
        return 0.0;
    }

    lua_pushstring(m_L, name.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get table[name] */

    if (!lua_isnumber(m_L, -1))
    {
        qDebug() << name << " is expected to be a number";
        return 0.0;
    }

    float result = (float)lua_tonumber(m_L, -1);
    lua_pop(m_L, 1); // remove the result
    lua_pop(m_L, 1); // remove table

    return result;
}

float LuaVM::getFloat(const QString &t1, const QString &t2, const QString &name)
{
    if (!m_L)
        return 0.0;

    lua_getglobal(m_L, t1.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t1 << " is expected to be a table";
        return 0.0;
    }

    lua_pushstring(m_L, t2.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t2 << " is expected to be a table";
        return 0.0;
    }

    lua_pushstring(m_L, name.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t2[name] */

    if (!lua_isnumber(m_L, -1))
    {
        qDebug() << name << " is expected to be a number";
        return 0.0;
    }

    float result = (float)lua_tonumber(m_L, -1);
    lua_pop(m_L, 1); // remove the result
    lua_pop(m_L, 1); // remove t2
    lua_pop(m_L, 1); // remove t1

    return result;
}

float LuaVM::getFloat(const QString &t1, const QString &t2, const QString &t3, const QString &name)
{
    if (!m_L)
        return 0.0;

    lua_getglobal(m_L, t1.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t1 << " is expected to be a table";
        return 0.0;
    }

    lua_pushstring(m_L, t2.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t2 << " is expected to be a table";
        return 0.0;
    }

    lua_pushstring(m_L, t3.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t2[t3] */

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t3 << " is expected to be a table";
        return 0.0;
    }

    lua_pushstring(m_L, name.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t3[name] */

    if (!lua_isnumber(m_L, -1))
    {
        qDebug() << name << " is expected to be a number";
        return 0.0;
    }

    float result = (float)lua_tonumber(m_L, -1);
    lua_pop(m_L, 1); // remove the result
    lua_pop(m_L, 1); // remove t3
    lua_pop(m_L, 1); // remove t2
    lua_pop(m_L, 1); // remove t1

    return result;
}

int LuaVM::getInt(const QString &table, const QString &name)
{
    if (!m_L)
        return 0;

    lua_getglobal(m_L, table.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qDebug() << table << " is expected to be a table";
        return 0;
    }

    lua_pushstring(m_L, name.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get table[name] */

    if (!lua_isinteger(m_L, -1))
    {
        qDebug() << name << " is expected to be an integer";
        return 0;
    }

    int result = (int)lua_tointeger(m_L, -1);
    lua_pop(m_L, 1); // remove the result
    lua_pop(m_L, 1); // remove table

    return result;
}

int LuaVM::getInt(const QString &t1, const QString &t2, const QString &name)
{
    if (!m_L)
        return 0;

    lua_getglobal(m_L, t1.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t1 << " is expected to be a table";
        return 0;
    }

    lua_pushstring(m_L, t2.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t2 << " is expected to be a table";
        return 0;
    }

    lua_pushstring(m_L, name.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t2[name] */

    if (!lua_isinteger(m_L, -1))
    {
        qDebug() << name << " is expected to be an integer";
        return 0;
    }

    int result = (int)lua_tointeger(m_L, -1);
    lua_pop(m_L, 1); // remove the result
    lua_pop(m_L, 1); // remove t2
    lua_pop(m_L, 1); // remove t1

    return result;
}

int LuaVM::getInt(const QString &t1, const QString &t2, const QString &t3, const QString &name)
{
    if (!m_L)
        return 0;

    lua_getglobal(m_L, t1.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t1 << " is expected to be a table";
        return 0;
    }

    lua_pushstring(m_L, t2.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t2 << " is expected to be a table";
        return 0;
    }

    lua_pushstring(m_L, t3.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t2[t3] */

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t3 << " is expected to be a table";
        return 0;
    }

    lua_pushstring(m_L, name.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t3[name] */

    if (!lua_isinteger(m_L, -1))
    {
        qDebug() << name << " is expected to be an integer";
        return 0;
    }

    int result = (int)lua_tointeger(m_L, -1);
    lua_pop(m_L, 1); // remove the result
    lua_pop(m_L, 1); // remove t3
    lua_pop(m_L, 1); // remove t2
    lua_pop(m_L, 1); // remove t1

    return result;
}

long long LuaVM::getLongLong(const QString &table, const QString &name)
{
    if (!m_L)
        return 0;

    lua_getglobal(m_L, table.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qDebug() << table << " is expected to be a table";
        return 0;
    }

    lua_pushstring(m_L, name.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get table[name] */

    if (!lua_isinteger(m_L, -1))
    {
        qDebug() << name << " is expected to be an integer";
        return 0;
    }

    long long result = (long long)lua_tointeger(m_L, -1);
    lua_pop(m_L, 1); // remove the result
    lua_pop(m_L, 1); // remove table

    return result;
}

long long LuaVM::getLongLong(const QString &t1, const QString &t2, const QString &name)
{
    if (!m_L)
        return 0;

    lua_getglobal(m_L, t1.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t1 << " is expected to be a table";
        return 0;
    }

    lua_pushstring(m_L, t2.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t2 << " is expected to be a table";
        return 0;
    }

    lua_pushstring(m_L, name.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t2[name] */

    if (!lua_isinteger(m_L, -1))
    {
        qDebug() << name << " is expected to be an integer";
        return 0;
    }

    long long result = (long long)lua_tointeger(m_L, -1);
    lua_pop(m_L, 1); // remove the result
    lua_pop(m_L, 1); // remove t2
    lua_pop(m_L, 1); // remove t1

    return result;
}

long long LuaVM::getLongLong(const QString &t1, const QString &t2, const QString &t3, const QString &name)
{
    if (!m_L)
        return 0;

    lua_getglobal(m_L, t1.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t1 << " is expected to be a table";
        return 0;
    }

    lua_pushstring(m_L, t2.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t2 << " is expected to be a table";
        return 0;
    }

    lua_pushstring(m_L, t3.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t2[t3] */

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t3 << " is expected to be a table";
        return 0;
    }

    lua_pushstring(m_L, name.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t3[name] */

    if (!lua_isinteger(m_L, -1))
    {
        qDebug() << name << " is expected to be an integer";
        return 0;
    }

    long long result = (long long)lua_tointeger(m_L, -1);
    lua_pop(m_L, 1); // remove the result
    lua_pop(m_L, 1); // remove t3
    lua_pop(m_L, 1); // remove t2
    lua_pop(m_L, 1); // remove t1

    return result;
}

bool LuaVM::getBool(const QString &table, const QString &name)
{
    if (!m_L)
        return false;

    lua_getglobal(m_L, table.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qDebug() << table << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, name.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get table[name] */

    if (!lua_isboolean(m_L, -1))
    {
        qDebug() << name << " is expected to be a boolean";
        return false;
    }

    int result = lua_toboolean(m_L, -1);
    lua_pop(m_L, 1); // remove the result
    lua_pop(m_L, 1); // remove table

    return !!result;
}

bool LuaVM::getBool(const QString &t1, const QString &t2, const QString &name)
{
    if (!m_L)
        return false;

    lua_getglobal(m_L, t1.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t1 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, t2.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t2 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, name.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t2[name] */

    if (!lua_isboolean(m_L, -1))
    {
        qDebug() << name << " is expected to be a boolean";
        return false;
    }

    int result = lua_toboolean(m_L, -1);
    lua_pop(m_L, 1); // remove the result
    lua_pop(m_L, 1); // remove t2
    lua_pop(m_L, 1); // remove t1

    return !!result;
}

bool LuaVM::getBool(const QString &t1, const QString &t2, const QString &t3, const QString &name)
{
    if (!m_L)
        return false;

    lua_getglobal(m_L, t1.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t1 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, t2.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t2 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, t3.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t2[t3] */

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t3 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, name.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t3[name] */

    if (!lua_isboolean(m_L, -1))
    {
        qDebug() << name << " is expected to be a boolean";
        return false;
    }

    int result = lua_toboolean(m_L, -1);
    lua_pop(m_L, 1); // remove the result
    lua_pop(m_L, 1); // remove t2
    lua_pop(m_L, 1); // remove t1

    return !!result;
}

QString LuaVM::getString(const QString &table, const QString &name)
{
    if (!m_L)
        return "";

    lua_getglobal(m_L, table.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qDebug() << table << " is expected to be a table";
        return "";
    }

    lua_pushstring(m_L, name.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get table[name] */

    if (!lua_isstring(m_L, -1))
    {
        qDebug() << name << " is expected to be a string";
        return "";
    }

    size_t resultLen = 0;
    const char * result = lua_tolstring(m_L, -1, &resultLen);
    QString str = QString::fromLatin1(result, (int)resultLen);
    lua_pop(m_L, 1); // remove the result
    lua_pop(m_L, 1); // remove table

    return str;
}

QString LuaVM::getString(const QString &t1, const QString &t2, const QString &name)
{
    if (!m_L)
        return "";

    lua_getglobal(m_L, t1.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t1 << " is expected to be a table";
        return "";
    }

    lua_pushstring(m_L, t2.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t2 << " is expected to be a table";
        return "";
    }

    lua_pushstring(m_L, name.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t2[name] */

    if (!lua_isstring(m_L, -1))
    {
        qDebug() << name << " is expected to be a string";
        return "";
    }

    size_t resultLen = 0;
    const char * result = lua_tolstring(m_L, -1, &resultLen);
    QString str = QString::fromLatin1(result, (int)resultLen);
    lua_pop(m_L, 1); // remove the result
    lua_pop(m_L, 1); // remove t2
    lua_pop(m_L, 1); // remove t1

    return str;
}

QString LuaVM::getString(const QString &t1, const QString &t2, const QString &t3, const QString &name)
{
    if (!m_L)
        return "";

    lua_getglobal(m_L, t1.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t1 << " is expected to be a table";
        return "";
    }

    lua_pushstring(m_L, t2.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t2 << " is expected to be a table";
        return "";
    }

    lua_pushstring(m_L, t3.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t2[t3] */

    if (!lua_isstring(m_L, -1))
    {
        qDebug() << t3 << " is expected to be a string";
        return "";
    }

    lua_pushstring(m_L, name.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t3[name] */

    if (!lua_isstring(m_L, -1))
    {
        qDebug() << name << " is expected to be a string";
        return "";
    }

    size_t resultLen = 0;
    const char * result = lua_tolstring(m_L, -1, &resultLen);
    QString str = QString::fromLatin1(result, (int)resultLen);
    lua_pop(m_L, 1); // remove the result
    lua_pop(m_L, 1); // remove t3
    lua_pop(m_L, 1); // remove t2
    lua_pop(m_L, 1); // remove t1

    return str;
}

bool LuaVM::set(const QString &name, double value)
{
    if (!m_L)
        return false;

    lua_getglobal(m_L, name.toStdString().c_str());

    if (!lua_isnumber(m_L, -1))
    {
        qDebug() << name << " is expected to be a number";
        return false;
    }

    lua_pushnumber(m_L, value);
    lua_setglobal(m_L, name.toStdString().c_str());
    return true;
}

bool LuaVM::set(const QString &t1, const QString &name, double value)
{
    if (!m_L)
        return false;

    lua_getglobal(m_L, t1.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t1 << " is expected to be a table";
        return false;
    }

    lua_pushnumber(m_L, value);
    lua_setfield(m_L, -2, name.toStdString().c_str());

    lua_pop(m_L, 1); // remove t1
    return true;
}

bool LuaVM::set(const QString &t1, const QString &t2, const QString &name, double value)
{
    if (!m_L)
        return false;

    lua_getglobal(m_L, t1.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t1 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, t2.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t2 << " is expected to be a table";
        return false;
    }

    lua_pushnumber(m_L, value);
    lua_setfield(m_L, -2, name.toStdString().c_str());

    lua_pop(m_L, 1); // remove t2
    lua_pop(m_L, 1); // remove t1
    return true;
}

bool LuaVM::set(const QString &t1, const QString &t2, const QString &t3, const QString &name, double value)
{
    if (!m_L)
        return false;

    lua_getglobal(m_L, t1.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t1 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, t2.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t2 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, t3.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t3 << " is expected to be a table";
        return false;
    }

    lua_pushnumber(m_L, value);
    lua_setfield(m_L, -2, name.toStdString().c_str());

    lua_pop(m_L, 1); // remove t3
    lua_pop(m_L, 1); // remove t2
    lua_pop(m_L, 1); // remove t1
    return true;
}

bool LuaVM::set(const QString &name, float value)
{
    return set(name, (double)value);
}

bool LuaVM::set(const QString &t1, const QString &name, float value)
{
    return set(t1, name, (double)value);
}

bool LuaVM::set(const QString &t1, const QString &t2, const QString &name, float value)
{
    return set(t1, t2, name, (double)value);
}

bool LuaVM::set(const QString &t1, const QString &t2, const QString &t3, const QString &name, float value)
{
    return set(t1, t2, t3, name, (double)value);
}

bool LuaVM::set(const QString &name, int value)
{
    if (!m_L)
        return false;

    lua_getglobal(m_L, name.toStdString().c_str());

    if (!lua_isinteger(m_L, -1))
    {
        qDebug() << name << " is expected to be an integer";
        return false;
    }

    lua_pushinteger(m_L, value);
    lua_setglobal(m_L, name.toStdString().c_str());
    return true;
}

bool LuaVM::set(const QString &t1, const QString &name, int value)
{
    if (!m_L)
        return false;

    lua_getglobal(m_L, t1.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t1 << " is expected to be a table";
        return false;
    }

    lua_pushinteger(m_L, value);
    lua_setfield(m_L, -2, name.toStdString().c_str());

    lua_pop(m_L, 1); // remove t1
    return true;
}

bool LuaVM::set(const QString &t1, const QString &t2, const QString &name, int value)
{
    if (!m_L)
        return false;

    lua_getglobal(m_L, t1.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t1 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, t2.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t2 << " is expected to be a table";
        return false;
    }

    lua_pushinteger(m_L, value);
    lua_setfield(m_L, -2, name.toStdString().c_str());

    lua_pop(m_L, 1); // remove t2
    lua_pop(m_L, 1); // remove t1
    return true;
}

bool LuaVM::set(const QString &t1, const QString &t2, const QString &t3, const QString &name, int value)
{
    if (!m_L)
        return false;

    lua_getglobal(m_L, t1.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t1 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, t2.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t2 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, t3.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t3 << " is expected to be a table";
        return false;
    }

    lua_pushinteger(m_L, value);
    lua_setfield(m_L, -2, name.toStdString().c_str());

    lua_pop(m_L, 1); // remove t3
    lua_pop(m_L, 1); // remove t2
    lua_pop(m_L, 1); // remove t1
    return true;
}

bool LuaVM::set(const QString &name, long long value)
{
    if (!m_L)
        return false;

    lua_getglobal(m_L, name.toStdString().c_str());

    if (!lua_isinteger(m_L, -1))
    {
        qDebug() << name << " is expected to be an integer";
        return false;
    }

    lua_pushinteger(m_L, value);
    lua_setglobal(m_L, name.toStdString().c_str());
    return true;
}

bool LuaVM::set(const QString &t1, const QString &name, long long value)
{
    if (!m_L)
        return false;

    lua_getglobal(m_L, t1.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t1 << " is expected to be a table";
        return false;
    }

    lua_pushinteger(m_L, value);
    lua_setfield(m_L, -2, name.toStdString().c_str());

    lua_pop(m_L, 1); // remove t1
    return true;
}

bool LuaVM::set(const QString &t1, const QString &t2, const QString &name, long long value)
{
    if (!m_L)
        return false;

    lua_getglobal(m_L, t1.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t1 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, t2.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t2 << " is expected to be a table";
        return false;
    }

    lua_pushinteger(m_L, value);
    lua_setfield(m_L, -2, name.toStdString().c_str());

    lua_pop(m_L, 1); // remove t2
    lua_pop(m_L, 1); // remove t1
    return true;
}

bool LuaVM::set(const QString &t1, const QString &t2, const QString &t3, const QString &name, long long value)
{
    if (!m_L)
        return false;

    lua_getglobal(m_L, t1.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t1 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, t2.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t2 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, t3.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t3 << " is expected to be a table";
        return false;
    }

    lua_pushinteger(m_L, value);
    lua_setfield(m_L, -2, name.toStdString().c_str());

    lua_pop(m_L, 1); // remove t3
    lua_pop(m_L, 1); // remove t2
    lua_pop(m_L, 1); // remove t1
    return true;
}

bool LuaVM::set(const QString &name, bool value)
{
    if (!m_L)
        return false;

    lua_getglobal(m_L, name.toStdString().c_str());

    if (!lua_isboolean(m_L, -1))
    {
        qDebug() << name << " is expected to be a boolean";
        return false;
    }

    lua_pushboolean(m_L, value);
    lua_setglobal(m_L, name.toStdString().c_str());
    return true;
}

bool LuaVM::set(const QString &t1, const QString &name, bool value)
{
    if (!m_L)
        return false;

    lua_getglobal(m_L, t1.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t1 << " is expected to be a table";
        return false;
    }

    lua_pushboolean(m_L, value);
    lua_setfield(m_L, -2, name.toStdString().c_str());

    lua_pop(m_L, 1); // remove t1
    return true;
}

bool LuaVM::set(const QString &t1, const QString &t2, const QString &name, bool value)
{
    if (!m_L)
        return false;

    lua_getglobal(m_L, t1.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t1 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, t2.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t2 << " is expected to be a table";
        return false;
    }

    lua_pushboolean(m_L, value);
    lua_setfield(m_L, -2, name.toStdString().c_str());

    lua_pop(m_L, 1); // remove t2
    lua_pop(m_L, 1); // remove t1
    return true;
}

bool LuaVM::set(const QString &t1, const QString &t2, const QString &t3, const QString &name, bool value)
{    if (!m_L)
        return false;

    lua_getglobal(m_L, t1.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t1 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, t2.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t2 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, t3.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t3 << " is expected to be a table";
        return false;
    }

    lua_pushboolean(m_L, value);
    lua_setfield(m_L, -2, name.toStdString().c_str());

    lua_pop(m_L, 1); // remove t3
    lua_pop(m_L, 1); // remove t2
    lua_pop(m_L, 1); // remove t1
    return true;

}

bool LuaVM::set(const QString &name, const char *value)
{
    if (!m_L)
        return false;

    lua_getglobal(m_L, name.toStdString().c_str());

    if (!lua_isstring(m_L, -1))
    {
        qDebug() << name << " is expected to be a string";
        return false;
    }

    lua_pushstring(m_L, value);
    lua_setglobal(m_L, name.toStdString().c_str());
    return true;
}

bool LuaVM::set(const QString &t1, const QString &name, const char *value)
{
    if (!m_L)
        return false;

    lua_getglobal(m_L, t1.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t1 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, value);
    lua_setfield(m_L, -2, name.toStdString().c_str());

    lua_pop(m_L, 1); // remove t1
    return true;
}

bool LuaVM::set(const QString &t1, const QString &t2, const QString &name, const char *value)
{
    if (!m_L)
        return false;

    lua_getglobal(m_L, t1.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t1 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, t2.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t2 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, value);
    lua_setfield(m_L, -2, name.toStdString().c_str());

    lua_pop(m_L, 1); // remove t2
    lua_pop(m_L, 1); // remove t1
    return true;
}

bool LuaVM::set(const QString &t1, const QString &t2, const QString &t3, const QString &name, const char *value)
{
    if (!m_L)
        return false;

    lua_getglobal(m_L, t1.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t1 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, t2.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t2 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, t3.toStdString().c_str());
    lua_gettable(m_L, -2);  /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t3 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, value);
    lua_setfield(m_L, -2, name.toStdString().c_str());

    lua_pop(m_L, 1); // remove t3
    lua_pop(m_L, 1); // remove t2
    lua_pop(m_L, 1); // remove t1
    return true;
}

void LuaVM::loadBuiltinTemplate()
{
    QFile f(":/lua/builtin.lua");
    if (f.open(QIODevice::ReadOnly))
    {
        QByteArray ba = f.readAll();
        f.close();

        doScript(ba);
    }
}

void LuaVM::setupBuiltinValues()
{
#if defined(Q_OS_MAC)
    set("derfla", "os", "name", "macos");
#elif defined(Q_OS_WIN)
    set("derfla", "os", "name", "windows");
#elif defined(Q_OS_LINUX)
    set("derfla", "os", "name", "linux");
#else
    set("derfla", "os", "name", "other");
#endif
}
