#ifndef LUAWRAPPER_H
#define LUAWRAPPER_H

#include "util_global.h"
#include <lua.hpp>

class UTILSHARED_EXPORT LuaWrapper
{
public:
    LuaWrapper() = default;
    explicit LuaWrapper(lua_State *L);
    ~LuaWrapper();

    void attachLuaState(lua_State *L);
    void createLuaState();
    bool doFile(const QString &file);
    bool doScript(const QString &script);

    double getDouble(const QString &name);
    float getFloat(const QString &name);
    int getInt(const QString &name);
    long long getLongLong(const QString &name);
    bool getBool(const QString &name);
    QString getString(const QString &name);

    double getDouble(const QString &table, const QString &name);
    float getFloat(const QString &table, const QString &name);
    int getInt(const QString &table, const QString &name);
    long long getLongLong(const QString &table, const QString &name);
    bool getBool(const QString &table, const QString &name);
    QString getString(const QString &table, const QString &name);

private:
    lua_State *m_L = nullptr;
};

#endif // LUAWRAPPER_H
