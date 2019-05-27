#include "stdafx.h"
#include <lua.hpp>
#include "scopedguard.h"
#include "thememanager.h"

bool ThemeManager::applyTheme(const QString &theme)
{
    if (!QFile::exists(theme))
        return false;
    
    auto *L = luaL_newstate();
    ScopedGuard sg([L](){lua_close(L);});
    
    luaL_openlibs(L);
    
    int status = luaL_loadfile(L, theme.toStdString().c_str());
    if (status) 
    {
        qDebug() << "Couldn't load file:" << lua_tostring(L, -1);
        return false;
    }
    
    lua_pcall(L, 0, 0, 0);
    
    lua_getglobal(L, "beginHeight");
    
    if (!lua_isnumber(L, -1))
    {
        qDebug() << "beginHeight is expected to be a number";
        return false;
    }
    
    int beginHeight = (int)lua_tonumber(L, -1);
    lua_pop(L, 1);
    
    return true;
}
