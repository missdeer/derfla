#include "stdafx.h"

#include "luavm.h"

LuaVM::LuaVM()
{
    start();
}

LuaVM::~LuaVM()
{
    shutdown();
}

void LuaVM::start()
{
    if (!m_L)
    {
        m_L = luaL_newstate();

        luaL_openlibs(m_L);

        loadBuiltinTemplate();

        setupBuiltinValues();
    }
}

void LuaVM::shutdown()
{
    if (m_L)
    {
        lua_close(m_L);
        m_L = nullptr;
    }
}

bool LuaVM::doFile(const QString &file)
{
    return doFile(file.toStdString().c_str());
}

bool LuaVM::doFile(const std::string &file)
{
    return doFile(file.c_str());
}

bool LuaVM::doFile(const char *file)
{
    if (!m_L)
    {
        qCritical() << "lua intepreter is not ready";
        return false;
    }

    qDebug() << "parsing " << file;
    int status = luaL_loadfile(m_L, file);
    if (status)
    {
        qCritical() << "Couldn't load file:" << lua_tostring(m_L, -1);
        lua_pop(m_L, 1);
        return false;
    }

    status = lua_pcall(m_L, 0, 0, 0);
    if (status)
    {
        qCritical() << "Couldn't run file:" << lua_tostring(m_L, -1);
        lua_pop(m_L, 1);
        return false;
    }
    return true;
}

bool LuaVM::doScript(const QString &script)
{
    return doScript(script.toStdString().c_str(), script.size());
}

bool LuaVM::doScript(const std::string &script)
{
    return doScript(script.c_str(), script.size());
}

bool LuaVM::doScript(const char *script, size_t len)
{
    if (!m_L)
    {
        qCritical() << "lua intepreter is not ready";
        return false;
    }

    qDebug() << "evaluating script:" << script;
    int status = luaL_loadbuffer(m_L, script, len, "theme");
    if (status)
    {
        qCritical() << "Couldn't load script:" << lua_tostring(m_L, -1);
        lua_pop(m_L, 1);
        return false;
    }

    status = lua_pcall(m_L, 0, 0, 0);
    if (status)
    {
        qCritical() << "Couldn't run script:" << lua_tostring(m_L, -1);
        lua_pop(m_L, 1);
        return false;
    }
    return true;
}

bool LuaVM::getQStringArray(const QString &name, QStringList &array)
{
    return getQStringArray(name.toStdString().c_str(), array);
}
bool LuaVM::getQStringArray(const std::string &name, QStringList &array)
{
    return getQStringArray(name.c_str(), array);
}
bool LuaVM::getQStringArray(const char *name, QStringList &array)
{
    if (!m_L)
    {
        qCritical() << "lua intepreter is not ready";
        return false;
    }

    lua_getglobal(m_L, name);

    if (!lua_istable(m_L, -1))
    {
        qCritical() << name << " is expected to be a table";
        return false;
    }

    size_t n = lua_rawlen(m_L, -1);

    for (size_t i = 1; i <= n; i++)
    {
        lua_rawgeti(m_L, -1, i);

        size_t      resultLen = 0;
        const char *result    = lua_tolstring(m_L, -1, &resultLen);
        QString     str       = QString::fromLatin1(result, static_cast<int>(resultLen));
        lua_pop(m_L, 1);
        array.append(str);
    }

    return true;
}

double LuaVM::getDouble(const QString &name)
{
    return getDouble(name.toStdString().c_str());
}
double LuaVM::getDouble(const std::string &name)
{
    return getDouble(name.c_str());
}
double LuaVM::getDouble(const char *name)
{
    if (!m_L)
    {
        qCritical() << "lua intepreter is not ready";
        return 0.0;
    }

    lua_getglobal(m_L, name);

    if (!lua_isnumber(m_L, -1))
    {
        qCritical() << name << " is expected to be a number";
        return 0.0;
    }

    double result = lua_tonumber(m_L, -1);
    lua_pop(m_L, 1);

    return result;
}

float LuaVM::getFloat(const QString &name)
{
    return getFloat(name.toStdString().c_str());
}
float LuaVM::getFloat(const std::string &name)
{
    return getFloat(name.c_str());
}
float LuaVM::getFloat(const char *name)
{
    if (!m_L)
    {
        qCritical() << "lua intepreter is not ready";
        return 0.0;
    }

    lua_getglobal(m_L, name);

    if (!lua_isnumber(m_L, -1))
    {
        qCritical() << name << " is expected to be a number";
        return 0.0;
    }

    auto result = (float)lua_tonumber(m_L, -1);
    lua_pop(m_L, 1);

    return result;
}

int LuaVM::getInt(const QString &name)
{
    return getInt(name.toStdString().c_str());
}
int LuaVM::getInt(const std::string &name)
{
    return getInt(name.c_str());
}
int LuaVM::getInt(const char *name)
{
    if (!m_L)
    {
        qCritical() << "lua intepreter is not ready";
        return 0;
    }

    lua_getglobal(m_L, name);

    if (!lua_isinteger(m_L, -1))
    {
        qCritical() << name << " is expected to be a number";
        return 0;
    }

    int result = (int)lua_tointeger(m_L, -1);
    lua_pop(m_L, 1);

    return result;
}

long long LuaVM::getLongLong(const QString &name)
{
    return getLongLong(name.toStdString().c_str());
}
long long LuaVM::getLongLong(const std::string &name)
{
    return getLongLong(name.c_str());
}
long long LuaVM::getLongLong(const char *name)
{
    if (!m_L)
    {
        qCritical() << "lua intepreter is not ready";
        return 0;
    }

    lua_getglobal(m_L, name);

    if (!lua_isinteger(m_L, -1))
    {
        qCritical() << name << " is expected to be a number";
        return 0;
    }

    auto result = (long long)lua_tointeger(m_L, -1);
    lua_pop(m_L, 1);

    return result;
}

bool LuaVM::getBool(const QString &name)
{
    return getBool(name.toStdString().c_str());
}
bool LuaVM::getBool(const std::string &name)
{
    return getBool(name.c_str());
}
bool LuaVM::getBool(const char *name)
{
    if (!m_L)
    {
        qCritical() << "lua intepreter is not ready";
        return false;
    }

    lua_getglobal(m_L, name);

    if (!lua_isboolean(m_L, -1))
    {
        qDebug() << name << " is expected to be a boolean";
        return false;
    }

    int result = lua_toboolean(m_L, -1);
    lua_pop(m_L, 1);

    return !!result;
}

QString LuaVM::getQString(const QString &name)
{
    return getQString(name.toStdString().c_str());
}

QString LuaVM::getQString(const std::string &name)
{
    return getQString(name.c_str());
}

QString LuaVM::getQString(const char *name)
{
    return QString::fromStdString(getString(name));
}

std::string LuaVM::getString(const std::string &name)
{
    return getString(name.c_str());
}
std::string LuaVM::getString(const char *name)
{
    if (!m_L)
    {
        qCritical() << "lua intepreter is not ready";
        return {};
    }

    lua_getglobal(m_L, name);

    if (!lua_isstring(m_L, -1))
    {
        qCritical() << name << " is expected to be a string";
        return {};
    }

    size_t      resultLen = 0;
    const char *result    = lua_tolstring(m_L, -1, &resultLen);
    std::string str       = std::string(result, static_cast<int>(resultLen));
    lua_pop(m_L, 1);

    return str;
}

double LuaVM::getDouble(const QString &table, const QString &name)
{
    return getDouble(table.toStdString().c_str(), name.toStdString().c_str());
}
double LuaVM::getDouble(const std::string &table, const std::string &name)
{
    return getDouble(table.c_str(), name.c_str());
}
double LuaVM::getDouble(const char *table, const char *name)
{
    if (!m_L)
    {
        qCritical() << "lua intepreter is not ready";
        return 0.0;
    }

    lua_getglobal(m_L, table);

    if (!lua_istable(m_L, -1))
    {
        qCritical() << table << " is expected to be a table";
        return 0.0;
    }

    lua_pushstring(m_L, name);
    lua_gettable(m_L, -2); /* get table[name] */

    if (!lua_isnumber(m_L, -1))
    {
        qCritical() << name << " is expected to be a number";
        return 0.0;
    }

    double result = lua_tonumber(m_L, -1);
    lua_pop(m_L, 1); // remove the result
    lua_pop(m_L, 1); // remove table

    return result;
}

double LuaVM::getDouble(const QString &t1, const QString &t2, const QString &name)
{
    return getDouble(t1.toStdString().c_str(), t2.toStdString().c_str(), name.toStdString().c_str());
}
double LuaVM::getDouble(const std::string &t1, const std::string &t2, const std::string &name)
{
    return getDouble(t1.c_str(), t2.c_str(), name.c_str());
}
double LuaVM::getDouble(const char *t1, const char *t2, const char *name)
{
    if (!m_L)
    {
        qCritical() << "lua intepreter is not ready";
        return 0.0;
    }

    lua_getglobal(m_L, t1);

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t1 << " is expected to be a table";
        return 0.0;
    }

    lua_pushstring(m_L, t2);
    lua_gettable(m_L, -2); /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t2 << " is expected to be a table";
        return 0.0;
    }

    lua_pushstring(m_L, name);
    lua_gettable(m_L, -2); /* get t2[name] */

    if (!lua_isnumber(m_L, -1))
    {
        qCritical() << name << " is expected to be a number";
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
    return getDouble(t1.toStdString().c_str(), t2.toStdString().c_str(), t3.toStdString().c_str(), name.toStdString().c_str());
}
double LuaVM::getDouble(const std::string &t1, const std::string &t2, const std::string &t3, const std::string &name)
{
    return getDouble(t1.c_str(), t2.c_str(), t3.c_str(), name.c_str());
}
double LuaVM::getDouble(const char *t1, const char *t2, const char *t3, const char *name)
{
    if (!m_L)
    {
        qCritical() << "lua intepreter is not ready";
        return 0.0;
    }

    lua_getglobal(m_L, t1);

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t1 << " is expected to be a table";
        return 0.0;
    }

    lua_pushstring(m_L, t2);
    lua_gettable(m_L, -2); /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t2 << " is expected to be a table";
        return 0.0;
    }

    lua_pushstring(m_L, t3);
    lua_gettable(m_L, -2); /* get t2[t3] */

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t3 << " is expected to be a table";
        return 0.0;
    }

    lua_pushstring(m_L, name);
    lua_gettable(m_L, -2); /* get t3[name] */

    if (!lua_isnumber(m_L, -1))
    {
        qCritical() << name << " is expected to be a number";
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
    return getFloat(table.toStdString().c_str(), name.toStdString().c_str());
}
float LuaVM::getFloat(const std::string &table, const std::string &name)
{
    return getFloat(table.c_str(), name.c_str());
}
float LuaVM::getFloat(const char *table, const char *name)
{
    if (!m_L)
    {
        qCritical() << "lua intepreter is not ready";
        return 0.0;
    }

    lua_getglobal(m_L, table);

    if (!lua_istable(m_L, -1))
    {
        qCritical() << table << " is expected to be a table";
        return 0.0;
    }

    lua_pushstring(m_L, name);
    lua_gettable(m_L, -2); /* get table[name] */

    if (!lua_isnumber(m_L, -1))
    {
        qCritical() << name << " is expected to be a number";
        return 0.0;
    }

    auto result = (float)lua_tonumber(m_L, -1);
    lua_pop(m_L, 1); // remove the result
    lua_pop(m_L, 1); // remove table

    return result;
}

float LuaVM::getFloat(const QString &t1, const QString &t2, const QString &name)
{
    return getFloat(t1.toStdString().c_str(), t2.toStdString().c_str(), name.toStdString().c_str());
}
float LuaVM::getFloat(const std::string &t1, const std::string &t2, const std::string &name)
{
    return getFloat(t1.c_str(), t2.c_str(), name.c_str());
}
float LuaVM::getFloat(const char *t1, const char *t2, const char *name)
{
    if (!m_L)
    {
        qCritical() << "lua intepreter is not ready";
        return 0.0;
    }

    lua_getglobal(m_L, t1);

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t1 << " is expected to be a table";
        return 0.0;
    }

    lua_pushstring(m_L, t2);
    lua_gettable(m_L, -2); /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t2 << " is expected to be a table";
        return 0.0;
    }

    lua_pushstring(m_L, name);
    lua_gettable(m_L, -2); /* get t2[name] */

    if (!lua_isnumber(m_L, -1))
    {
        qDebug() << name << " is expected to be a number";
        return 0.0;
    }

    auto result = (float)lua_tonumber(m_L, -1);
    lua_pop(m_L, 1); // remove the result
    lua_pop(m_L, 1); // remove t2
    lua_pop(m_L, 1); // remove t1

    return result;
}

float LuaVM::getFloat(const QString &t1, const QString &t2, const QString &t3, const QString &name)
{
    return getFloat(t1.toStdString().c_str(), t2.toStdString().c_str(), t3.toStdString().c_str(), name.toStdString().c_str());
}
float LuaVM::getFloat(const std::string &t1, const std::string &t2, const std::string &t3, const std::string &name)
{
    return getFloat(t1.c_str(), t2.c_str(), t3.c_str(), name.c_str());
}
float LuaVM::getFloat(const char *t1, const char *t2, const char *t3, const char *name)
{
    if (!m_L)
    {
        qCritical() << "lua intepreter is not ready";
        return 0.0;
    }

    lua_getglobal(m_L, t1);

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t1 << " is expected to be a table";
        return 0.0;
    }

    lua_pushstring(m_L, t2);
    lua_gettable(m_L, -2); /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t2 << " is expected to be a table";
        return 0.0;
    }

    lua_pushstring(m_L, t3);
    lua_gettable(m_L, -2); /* get t2[t3] */

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t3 << " is expected to be a table";
        return 0.0;
    }

    lua_pushstring(m_L, name);
    lua_gettable(m_L, -2); /* get t3[name] */

    if (!lua_isnumber(m_L, -1))
    {
        qCritical() << name << " is expected to be a number";
        return 0.0;
    }

    auto result = (float)lua_tonumber(m_L, -1);
    lua_pop(m_L, 1); // remove the result
    lua_pop(m_L, 1); // remove t3
    lua_pop(m_L, 1); // remove t2
    lua_pop(m_L, 1); // remove t1

    return result;
}

int LuaVM::getInt(const QString &table, const QString &name)
{
    return getInt(table.toStdString().c_str(), name.toStdString().c_str());
}
int LuaVM::getInt(const std::string &table, const std::string &name)
{
    return getInt(table.c_str(), name.c_str());
}
int LuaVM::getInt(const char *table, const char *name)
{
    if (!m_L)
    {
        qCritical() << "lua intepreter is not ready";
        return 0;
    }

    lua_getglobal(m_L, table);

    if (!lua_istable(m_L, -1))
    {
        qCritical() << table << " is expected to be a table";
        return 0;
    }

    lua_pushstring(m_L, name);
    lua_gettable(m_L, -2); /* get table[name] */

    if (!lua_isinteger(m_L, -1))
    {
        qCritical() << name << " is expected to be an integer";
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
    {
        qCritical() << "lua intepreter is not ready";
        return 0;
    }

    lua_getglobal(m_L, t1.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t1 << " is expected to be a table";
        return 0;
    }

    lua_pushstring(m_L, t2.toStdString().c_str());
    lua_gettable(m_L, -2); /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t2 << " is expected to be a table";
        return 0;
    }

    lua_pushstring(m_L, name.toStdString().c_str());
    lua_gettable(m_L, -2); /* get t2[name] */

    if (!lua_isinteger(m_L, -1))
    {
        qCritical() << name << " is expected to be an integer";
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
    {
        qCritical() << "lua intepreter is not ready";
        return 0;
    }

    lua_getglobal(m_L, t1.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t1 << " is expected to be a table";
        return 0;
    }

    lua_pushstring(m_L, t2.toStdString().c_str());
    lua_gettable(m_L, -2); /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t2 << " is expected to be a table";
        return 0;
    }

    lua_pushstring(m_L, t3.toStdString().c_str());
    lua_gettable(m_L, -2); /* get t2[t3] */

    if (!lua_istable(m_L, -1))
    {
        qDebug() << t3 << " is expected to be a table";
        return 0;
    }

    lua_pushstring(m_L, name.toStdString().c_str());
    lua_gettable(m_L, -2); /* get t3[name] */

    if (!lua_isinteger(m_L, -1))
    {
        qCritical() << name << " is expected to be an integer";
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
    {
        qCritical() << "lua intepreter is not ready";
        return 0;
    }

    lua_getglobal(m_L, table.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qCritical() << table << " is expected to be a table";
        return 0;
    }

    lua_pushstring(m_L, name.toStdString().c_str());
    lua_gettable(m_L, -2); /* get table[name] */

    if (!lua_isinteger(m_L, -1))
    {
        qCritical() << name << " is expected to be an integer";
        return 0;
    }

    auto result = (long long)lua_tointeger(m_L, -1);
    lua_pop(m_L, 1); // remove the result
    lua_pop(m_L, 1); // remove table

    return result;
}

long long LuaVM::getLongLong(const QString &t1, const QString &t2, const QString &name)
{
    if (!m_L)
    {
        qCritical() << "lua intepreter is not ready";
        return 0;
    }

    lua_getglobal(m_L, t1.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t1 << " is expected to be a table";
        return 0;
    }

    lua_pushstring(m_L, t2.toStdString().c_str());
    lua_gettable(m_L, -2); /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t2 << " is expected to be a table";
        return 0;
    }

    lua_pushstring(m_L, name.toStdString().c_str());
    lua_gettable(m_L, -2); /* get t2[name] */

    if (!lua_isinteger(m_L, -1))
    {
        qCritical() << name << " is expected to be an integer";
        return 0;
    }

    auto result = (long long)lua_tointeger(m_L, -1);
    lua_pop(m_L, 1); // remove the result
    lua_pop(m_L, 1); // remove t2
    lua_pop(m_L, 1); // remove t1

    return result;
}

long long LuaVM::getLongLong(const QString &t1, const QString &t2, const QString &t3, const QString &name)
{
    if (!m_L)
    {
        qCritical() << "lua intepreter is not ready";
        return 0;
    }

    lua_getglobal(m_L, t1.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t1 << " is expected to be a table";
        return 0;
    }

    lua_pushstring(m_L, t2.toStdString().c_str());
    lua_gettable(m_L, -2); /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t2 << " is expected to be a table";
        return 0;
    }

    lua_pushstring(m_L, t3.toStdString().c_str());
    lua_gettable(m_L, -2); /* get t2[t3] */

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t3 << " is expected to be a table";
        return 0;
    }

    lua_pushstring(m_L, name.toStdString().c_str());
    lua_gettable(m_L, -2); /* get t3[name] */

    if (!lua_isinteger(m_L, -1))
    {
        qCritical() << name << " is expected to be an integer";
        return 0;
    }

    auto result = (long long)lua_tointeger(m_L, -1);
    lua_pop(m_L, 1); // remove the result
    lua_pop(m_L, 1); // remove t3
    lua_pop(m_L, 1); // remove t2
    lua_pop(m_L, 1); // remove t1

    return result;
}

bool LuaVM::getBool(const QString &table, const QString &name)
{
    if (!m_L)
    {
        qCritical() << "lua intepreter is not ready";
        return false;
    }

    lua_getglobal(m_L, table.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qCritical() << table << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, name.toStdString().c_str());
    lua_gettable(m_L, -2); /* get table[name] */

    if (!lua_isboolean(m_L, -1))
    {
        qCritical() << name << " is expected to be a boolean";
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
    {
        qCritical() << "lua intepreter is not ready";
        return false;
    }

    lua_getglobal(m_L, t1.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t1 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, t2.toStdString().c_str());
    lua_gettable(m_L, -2); /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t2 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, name.toStdString().c_str());
    lua_gettable(m_L, -2); /* get t2[name] */

    if (!lua_isboolean(m_L, -1))
    {
        qCritical() << name << " is expected to be a boolean";
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
    {
        qCritical() << "lua intepreter is not ready";
        return false;
    }

    lua_getglobal(m_L, t1.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t1 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, t2.toStdString().c_str());
    lua_gettable(m_L, -2); /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t2 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, t3.toStdString().c_str());
    lua_gettable(m_L, -2); /* get t2[t3] */

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t3 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, name.toStdString().c_str());
    lua_gettable(m_L, -2); /* get t3[name] */

    if (!lua_isboolean(m_L, -1))
    {
        qCritical() << name << " is expected to be a boolean";
        return false;
    }

    int result = lua_toboolean(m_L, -1);
    lua_pop(m_L, 1); // remove the result
    lua_pop(m_L, 1); // remove t2
    lua_pop(m_L, 1); // remove t1

    return !!result;
}

QString LuaVM::getQString(const QString &table, const QString &name)
{
    return getQString(table.toStdString().c_str(), name.toStdString().c_str());
}
QString LuaVM::getQString(const std::string &table, const std::string &name)
{
    return getQString(table.c_str(), name.c_str());
}
QString LuaVM::getQString(const char *table, const char *name)
{
    return QString::fromStdString(getString(table, name));
}
std::string LuaVM::getString(const std::string &table, const std::string &name)
{
    return getString(table.c_str(), name.c_str());
}

std::string LuaVM::getString(const char *table, const char *name)
{
    if (!m_L)
    {
        qCritical() << "lua intepreter is not ready";
        return {};
    }

    lua_getglobal(m_L, table);

    if (!lua_istable(m_L, -1))
    {
        qCritical() << table << " is expected to be a table";
        return {};
    }

    lua_pushstring(m_L, name);
    lua_gettable(m_L, -2); /* get table[name] */

    if (!lua_isstring(m_L, -1))
    {
        qCritical() << name << " is expected to be a string";
        return {};
    }

    size_t      resultLen = 0;
    const char *result    = lua_tolstring(m_L, -1, &resultLen);
    std::string str       = std::string(result, static_cast<int>(resultLen));
    lua_pop(m_L, 1); // remove the result
    lua_pop(m_L, 1); // remove table

    return str;
}

QString LuaVM::getQString(const QString &t1, const QString &t2, const QString &name)
{
    return getQString(t1.toStdString().c_str(), t2.toStdString().c_str(), name.toStdString().c_str());
}
QString LuaVM::getQString(const std::string &t1, const std::string &t2, const std::string &name)
{
    return getQString(t1.c_str(), t2.c_str(), name.c_str());
}
QString LuaVM::getQString(const char *t1, const char *t2, const char *name)
{
    return QString::fromStdString(getString(t1, t2, name));
}
std::string LuaVM::getString(const std::string &t1, const std::string &t2, const std::string &name)
{
    return getString(t1.c_str(), t2.c_str(), name.c_str());
}
std::string LuaVM::getString(const char *t1, const char *t2, const char *name)
{
    if (!m_L)
    {
        qCritical() << "lua intepreter is not ready";
        return {};
    }

    lua_getglobal(m_L, t1);

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t1 << " is expected to be a table";
        return {};
    }

    lua_pushstring(m_L, t2);
    lua_gettable(m_L, -2); /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t2 << " is expected to be a table";
        return {};
    }

    lua_pushstring(m_L, name);
    lua_gettable(m_L, -2); /* get t2[name] */

    if (!lua_isstring(m_L, -1))
    {
        qCritical() << name << " is expected to be a string";
        return {};
    }

    size_t      resultLen = 0;
    const char *result    = lua_tolstring(m_L, -1, &resultLen);
    auto        str       = std::string(result, static_cast<int>(resultLen));
    lua_pop(m_L, 1); // remove the result
    lua_pop(m_L, 1); // remove t2
    lua_pop(m_L, 1); // remove t1

    return str;
}

QString LuaVM::getQString(const QString &t1, const QString &t2, const QString &t3, const QString &name)
{
    return getQString(t1.toStdString().c_str(), t2.toStdString().c_str(), t3.toStdString().c_str(), name.toStdString().c_str());
}
QString LuaVM::getQString(const std::string &t1, const std::string &t2, const std::string &t3, const std::string &name)
{
    return getQString(t1.c_str(), t2.c_str(), t3.c_str(), name.c_str());
}
QString LuaVM::getQString(const char *t1, const char *t2, const char *t3, const char *name)
{
    return QString::fromStdString(getString(t1, t2, t3, name));
}
std::string LuaVM::getString(const std::string &t1, const std::string &t2, const std::string &t3, const std::string &name)
{
    return getString(t1.c_str(), t2.c_str(), t3.c_str(), name.c_str());
}
std::string LuaVM::getString(const char *t1, const char *t2, const char *t3, const char *name)
{
    if (!m_L)
    {
        qCritical() << "lua intepreter is not ready";
        return {};
    }

    lua_getglobal(m_L, t1);

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t1 << " is expected to be a table";
        return {};
    }

    lua_pushstring(m_L, t2);
    lua_gettable(m_L, -2); /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t2 << " is expected to be a table";
        return {};
    }

    lua_pushstring(m_L, t3);
    lua_gettable(m_L, -2); /* get t2[t3] */

    if (!lua_isstring(m_L, -1))
    {
        qCritical() << t3 << " is expected to be a string";
        return {};
    }

    lua_pushstring(m_L, name);
    lua_gettable(m_L, -2); /* get t3[name] */

    if (!lua_isstring(m_L, -1))
    {
        qCritical() << name << " is expected to be a string";
        return {};
    }

    size_t      resultLen = 0;
    const char *result    = lua_tolstring(m_L, -1, &resultLen);
    auto        str       = std::string(result, static_cast<int>(resultLen));
    lua_pop(m_L, 1); // remove the result
    lua_pop(m_L, 1); // remove t3
    lua_pop(m_L, 1); // remove t2
    lua_pop(m_L, 1); // remove t1

    return str;
}

bool LuaVM::set(const QString &name, double value)
{
    if (!m_L)
    {
        qCritical() << "lua intepreter is not ready";
        return false;
    }

    lua_getglobal(m_L, name.toStdString().c_str());

    if (!lua_isnumber(m_L, -1))
    {
        qCritical() << name << " is expected to be a number";
        return false;
    }

    lua_pushnumber(m_L, value);
    lua_setglobal(m_L, name.toStdString().c_str());
    return true;
}

bool LuaVM::set(const QString &t1, const QString &name, double value)
{
    if (!m_L)
    {
        qCritical() << "lua intepreter is not ready";
        return false;
    }

    lua_getglobal(m_L, t1.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t1 << " is expected to be a table";
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
    {
        qCritical() << "lua intepreter is not ready";
        return false;
    }

    lua_getglobal(m_L, t1.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t1 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, t2.toStdString().c_str());
    lua_gettable(m_L, -2); /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t2 << " is expected to be a table";
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
    {
        qCritical() << "lua intepreter is not ready";
        return false;
    }

    lua_getglobal(m_L, t1.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t1 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, t2.toStdString().c_str());
    lua_gettable(m_L, -2); /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t2 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, t3.toStdString().c_str());
    lua_gettable(m_L, -2); /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t3 << " is expected to be a table";
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
    return set(name, static_cast<double>(value));
}

bool LuaVM::set(const QString &t1, const QString &name, float value)
{
    return set(t1, name, static_cast<double>(value));
}

bool LuaVM::set(const QString &t1, const QString &t2, const QString &name, float value)
{
    return set(t1, t2, name, static_cast<double>(value));
}

bool LuaVM::set(const QString &t1, const QString &t2, const QString &t3, const QString &name, float value)
{
    return set(t1, t2, t3, name, static_cast<double>(value));
}

bool LuaVM::set(const QString &name, int value)
{
    if (!m_L)
    {
        qCritical() << "lua intepreter is not ready";
        return false;
    }

    lua_getglobal(m_L, name.toStdString().c_str());

    if (!lua_isinteger(m_L, -1))
    {
        qCritical() << name << " is expected to be an integer";
        return false;
    }

    lua_pushinteger(m_L, value);
    lua_setglobal(m_L, name.toStdString().c_str());
    return true;
}

bool LuaVM::set(const QString &t1, const QString &name, int value)
{
    if (!m_L)
    {
        qCritical() << "lua intepreter is not ready";
        return false;
    }

    lua_getglobal(m_L, t1.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t1 << " is expected to be a table";
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
    {
        qCritical() << "lua intepreter is not ready";
        return false;
    }

    lua_getglobal(m_L, t1.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t1 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, t2.toStdString().c_str());
    lua_gettable(m_L, -2); /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t2 << " is expected to be a table";
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
    {
        qCritical() << "lua intepreter is not ready";
        return false;
    }

    lua_getglobal(m_L, t1.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t1 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, t2.toStdString().c_str());
    lua_gettable(m_L, -2); /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t2 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, t3.toStdString().c_str());
    lua_gettable(m_L, -2); /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t3 << " is expected to be a table";
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
    {
        qCritical() << "lua intepreter is not ready";
        return false;
    }

    lua_getglobal(m_L, name.toStdString().c_str());

    if (!lua_isinteger(m_L, -1))
    {
        qCritical() << name << " is expected to be an integer";
        return false;
    }

    lua_pushinteger(m_L, value);
    lua_setglobal(m_L, name.toStdString().c_str());
    return true;
}

bool LuaVM::set(const QString &t1, const QString &name, long long value)
{
    if (!m_L)
    {
        qCritical() << "lua intepreter is not ready";
        return false;
    }

    lua_getglobal(m_L, t1.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t1 << " is expected to be a table";
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
    {
        qCritical() << "lua intepreter is not ready";
        return false;
    }

    lua_getglobal(m_L, t1.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t1 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, t2.toStdString().c_str());
    lua_gettable(m_L, -2); /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t2 << " is expected to be a table";
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
    {
        qCritical() << "lua intepreter is not ready";
        return false;
    }

    lua_getglobal(m_L, t1.toStdString().c_str());

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t1 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, t2.toStdString().c_str());
    lua_gettable(m_L, -2); /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t2 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, t3.toStdString().c_str());
    lua_gettable(m_L, -2); /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t3 << " is expected to be a table";
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
    return set(name.toStdString().c_str(), value);
}
bool LuaVM::set(const std::string &name, bool value)
{
    return set(name.c_str(), value);
}
bool LuaVM::set(const char *name, bool value)
{
    if (!m_L)
    {
        qCritical() << "lua intepreter is not ready";
        return false;
    }

    lua_getglobal(m_L, name);

    if (!lua_isboolean(m_L, -1))
    {
        qCritical() << name << " is expected to be a boolean";
        return false;
    }

    lua_pushboolean(m_L, value);
    lua_setglobal(m_L, name);
    return true;
}

bool LuaVM::set(const QString &t1, const QString &name, bool value)
{
    return set(t1.toStdString().c_str(), name.toStdString().c_str(), value);
}
bool LuaVM::set(const std::string &t1, const std::string &name, bool value)
{
    return set(t1.c_str(), name.c_str(), value);
}
bool LuaVM::set(const char *t1, const char *name, bool value)
{
    if (!m_L)
    {
        qCritical() << "lua intepreter is not ready";
        return false;
    }

    lua_getglobal(m_L, t1);

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t1 << " is expected to be a table";
        return false;
    }

    lua_pushboolean(m_L, value);
    lua_setfield(m_L, -2, name);

    lua_pop(m_L, 1); // remove t1
    return true;
}

bool LuaVM::set(const QString &t1, const QString &t2, const QString &name, bool value)
{
    return set(t1.toStdString().c_str(), t2.toStdString().c_str(), name.toStdString().c_str(), value);
}
bool LuaVM::set(const std::string &t1, const std::string &t2, const std::string &name, bool value)
{
    return set(t1.c_str(), t2.c_str(), name.c_str(), value);
}
bool LuaVM::set(const char *t1, const char *t2, const char *name, bool value)
{
    if (!m_L)
    {
        qCritical() << "lua intepreter is not ready";
        return false;
    }

    lua_getglobal(m_L, t1);

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t1 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, t2);
    lua_gettable(m_L, -2); /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t2 << " is expected to be a table";
        return false;
    }

    lua_pushboolean(m_L, value);
    lua_setfield(m_L, -2, name);

    lua_pop(m_L, 1); // remove t2
    lua_pop(m_L, 1); // remove t1
    return true;
}

bool LuaVM::set(const QString &t1, const QString &t2, const QString &t3, const QString &name, bool value)
{
    return set(t1.toStdString().c_str(), t2.toStdString().c_str(), t3.toStdString().c_str(), name.toStdString().c_str(), value);
}
bool LuaVM::set(const std::string &t1, const std::string &t2, const std::string &t3, const std::string &name, bool value)
{
    return set(t1.c_str(), t2.c_str(), t3.c_str(), name.c_str(), value);
}
bool LuaVM::set(const char *t1, const char *t2, const char *t3, const char *name, bool value)
{
    if (!m_L)
    {
        qCritical() << "lua intepreter is not ready";
        return false;
    }

    lua_getglobal(m_L, t1);

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t1 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, t2);
    lua_gettable(m_L, -2); /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t2 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, t3);
    lua_gettable(m_L, -2); /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t3 << " is expected to be a table";
        return false;
    }

    lua_pushboolean(m_L, value);
    lua_setfield(m_L, -2, name);

    lua_pop(m_L, 1); // remove t3
    lua_pop(m_L, 1); // remove t2
    lua_pop(m_L, 1); // remove t1
    return true;
}

bool LuaVM::set(const QString &name, const char *value)
{
    return set(name.toStdString().c_str(), value);
}
bool LuaVM::set(const std::string &name, const char *value)
{
    return set(name.c_str(), value);
}
bool LuaVM::set(const char *name, const char *value)
{
    if (!m_L)
    {
        qCritical() << "lua intepreter is not ready";
        return false;
    }

    lua_getglobal(m_L, name);

    if (!lua_isstring(m_L, -1))
    {
        qCritical() << name << " is expected to be a string";
        return false;
    }

    lua_pushstring(m_L, value);
    lua_setglobal(m_L, name);
    return true;
}

bool LuaVM::set(const QString &t1, const QString &name, const char *value)
{
    return set(t1.toStdString().c_str(), name.toStdString().c_str(), value);
}
bool LuaVM::set(const std::string &t1, const std::string &name, const char *value)
{
    return set(t1.c_str(), name.c_str(), value);
}
bool LuaVM::set(const char *t1, const char *name, const char *value)
{
    if (!m_L)
    {
        qCritical() << "lua intepreter is not ready";
        return false;
    }

    lua_getglobal(m_L, t1);

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t1 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, value);
    lua_setfield(m_L, -2, name);

    lua_pop(m_L, 1); // remove t1
    return true;
}

bool LuaVM::set(const QString &t1, const QString &t2, const QString &name, const char *value)
{
    return set(t1.toStdString().c_str(), t2.toStdString().c_str(), name.toStdString().c_str(), value);
}
bool LuaVM::set(const std::string &t1, const std::string &t2, const std::string &name, const char *value)
{
    return set(t1.c_str(), t2.c_str(), name.c_str(), value);
}
bool LuaVM::set(const char *t1, const char *t2, const char *name, const char *value)
{
    if (!m_L)
    {
        qCritical() << "lua intepreter is not ready";
        return false;
    }

    lua_getglobal(m_L, t1);

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t1 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, t2);
    lua_gettable(m_L, -2); /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t2 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, value);
    lua_setfield(m_L, -2, name);

    lua_pop(m_L, 1); // remove t2
    lua_pop(m_L, 1); // remove t1
    return true;
}

bool LuaVM::set(const QString &t1, const QString &t2, const QString &t3, const QString &name, const char *value)
{
    return set(t1.toStdString().c_str(), t2.toStdString().c_str(), t3.toStdString().c_str(), name.toStdString().c_str(), value);
}
bool LuaVM::set(const std::string &t1, const std::string &t2, const std::string &t3, const std::string &name, const char *value)
{
    return set(t1.c_str(), t2.c_str(), t3.c_str(), name.c_str(), value);
}
bool LuaVM::set(const char *t1, const char *t2, const char *t3, const char *name, const char *value)
{
    if (!m_L)
    {
        qCritical() << "lua intepreter is not ready";
        return false;
    }

    lua_getglobal(m_L, t1);

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t1 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, t2);
    lua_gettable(m_L, -2); /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t2 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, t3);
    lua_gettable(m_L, -2); /* get t1[t2] */

    if (!lua_istable(m_L, -1))
    {
        qCritical() << t3 << " is expected to be a table";
        return false;
    }

    lua_pushstring(m_L, value);
    lua_setfield(m_L, -2, name);

    lua_pop(m_L, 1); // remove t3
    lua_pop(m_L, 1); // remove t2
    lua_pop(m_L, 1); // remove t1
    return true;
}

void LuaVM::loadBuiltinTemplate()
{
    QFile file(QStringLiteral(":/lua/builtin.lua"));
    if (file.open(QIODevice::ReadOnly))
    {
        QByteArray content = file.readAll();
        file.close();

        doScript(content.constData(), content.length());
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
