#include "stdafx.h"
#include "luawrapper.h"

LuaWrapper::LuaWrapper(lua_State *L)
    : m_L(L)
{

}

LuaWrapper::~LuaWrapper()
{
    if (m_L)
    {
        lua_close(m_L);
        m_L = nullptr;
    }
}

void LuaWrapper::attachLuaState(lua_State *L)
{
    m_L = L;
}

void LuaWrapper::createLuaState()
{
    m_L = luaL_newstate();

    luaL_openlibs(m_L);
}

bool LuaWrapper::doFile(const QString &file)
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

bool LuaWrapper::doScript(const QString &script)
{
    if (!m_L)
        return false;
    int status = luaL_loadbuffer(m_L, script.toStdString().c_str(), script.toStdString().size(), nullptr);
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

double LuaWrapper::getDouble(const QString &name)
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

float LuaWrapper::getFloat(const QString &name)
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

int LuaWrapper::getInt(const QString &name)
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

long long LuaWrapper::getLongLong(const QString &name)
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

bool LuaWrapper::getBool(const QString &name)
{
    if (!m_L)
        return 0.0;

    lua_getglobal(m_L, name.toStdString().c_str());

    if (!lua_isboolean(m_L, -1))
    {
        qDebug() << name << " is expected to be a boolean";
        return 0.0;
    }

    int result = lua_toboolean(m_L, -1);
    lua_pop(m_L, 1);

    return !!result;
}

QString LuaWrapper::getString(const QString &name)
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

double LuaWrapper::getDouble(const QString &table, const QString &name)
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
    lua_pop(m_L, 1); // remove name
    lua_pop(m_L, 1); // remove table

    return result;
}

float LuaWrapper::getFloat(const QString &table, const QString &name)
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
    lua_pop(m_L, 1); // remove name
    lua_pop(m_L, 1); // remove table

    return result;
}

int LuaWrapper::getInt(const QString &table, const QString &name)
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
        qDebug() << name << " is expected to be a number";
        return 0;
    }

    int result = (int)lua_tointeger(m_L, -1);
    lua_pop(m_L, 1); // remove the result
    lua_pop(m_L, 1); // remove name
    lua_pop(m_L, 1); // remove table

    return result;
}

long long LuaWrapper::getLongLong(const QString &table, const QString &name)
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
        qDebug() << name << " is expected to be a number";
        return 0;
    }

    long long result = (long long)lua_tointeger(m_L, -1);
    lua_pop(m_L, 1); // remove the result
    lua_pop(m_L, 1); // remove name
    lua_pop(m_L, 1); // remove table

    return result;
}

bool LuaWrapper::getBool(const QString &table, const QString &name)
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

    if (!lua_isboolean(m_L, -1))
    {
        qDebug() << name << " is expected to be a boolean";
        return 0.0;
    }

    int result = lua_toboolean(m_L, -1);
    lua_pop(m_L, 1); // remove the result
    lua_pop(m_L, 1); // remove name
    lua_pop(m_L, 1); // remove table

    return result;
}

QString LuaWrapper::getString(const QString &table, const QString &name)
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
    lua_pop(m_L, 1); // remove name
    lua_pop(m_L, 1); // remove table

    return str;
}
