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
