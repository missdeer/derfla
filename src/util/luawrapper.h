#ifndef LUAWRAPPER_H
#define LUAWRAPPER_H

#include "util_global.h"
#include <lua.hpp>

class UTILSHARED_EXPORT LuaWrapper
{
public:
    LuaWrapper() = default;
    explicit LuaWrapper(lua_State * L);
    ~LuaWrapper();

    void attachLuaState(lua_State * L);
    void createLuaState();
    bool doFile(const QString& file);
    bool doScript(const QString& script);
private:
    lua_State *m_L = nullptr;
};

#endif // LUAWRAPPER_H
