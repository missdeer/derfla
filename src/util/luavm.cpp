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
    lua_pop(m_L, 1); // remove name
    lua_pop(m_L, 1); // remove table

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
    lua_pop(m_L, 1); // remove name
    lua_pop(m_L, 1); // remove table

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
        qDebug() << name << " is expected to be a number";
        return 0;
    }

    int result = (int)lua_tointeger(m_L, -1);
    lua_pop(m_L, 1); // remove the result
    lua_pop(m_L, 1); // remove name
    lua_pop(m_L, 1); // remove table

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
        qDebug() << name << " is expected to be a number";
        return 0;
    }

    long long result = (long long)lua_tointeger(m_L, -1);
    lua_pop(m_L, 1); // remove the result
    lua_pop(m_L, 1); // remove name
    lua_pop(m_L, 1); // remove table

    return result;
}

bool LuaVM::getBool(const QString &table, const QString &name)
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
    lua_pop(m_L, 1); // remove name
    lua_pop(m_L, 1); // remove table

    return str;
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

}
