#ifndef LUAWRAPPER_H
#define LUAWRAPPER_H

#include "util_global.h"
#include <lua.hpp>
#include <QStringList>

class UTILSHARED_EXPORT LuaVM
{
public:
    LuaVM() = default;
    explicit LuaVM(lua_State *L);
    ~LuaVM() = default;

    void start();
    void shutdown();
    bool doFile(const QString &file);
    bool doScript(const QString &script);
    
    bool getStringArray(const QString &name, QStringList& array);

    double getDouble(const QString &name);
    double getDouble(const QString &table, const QString &name);
    double getDouble(const QString &t1, const QString &t2, const QString &name);
    double getDouble(const QString &t1, const QString &t2, const QString &t3, const QString &name);

    float getFloat(const QString &name);
    float getFloat(const QString &table, const QString &name);
    float getFloat(const QString &t1, const QString &t2, const QString &name);
    float getFloat(const QString &t1, const QString &t2, const QString &t3, const QString &name);

    int getInt(const QString &name);
    int getInt(const QString &table, const QString &name);
    int getInt(const QString &t1, const QString &t2, const QString &name);
    int getInt(const QString &t1, const QString &t2, const QString &t3, const QString &name);

    long long getLongLong(const QString &name);
    long long getLongLong(const QString &table, const QString &name);
    long long getLongLong(const QString &t1, const QString &t2, const QString &name);
    long long getLongLong(const QString &t1, const QString &t2, const QString &t3, const QString &name);

    bool getBool(const QString &name);
    bool getBool(const QString &table, const QString &name);
    bool getBool(const QString &t1, const QString &t2, const QString &name);
    bool getBool(const QString &t1, const QString &t2, const QString &t3, const QString &name);

    QString getString(const QString &name);
    QString getString(const QString &table, const QString &name);
    QString getString(const QString &t1, const QString &t2, const QString &name);
    QString getString(const QString &t1, const QString &t2, const QString &t3, const QString &name);


    bool set(const QString &name, double value);
    bool set(const QString &t1, const QString &name, double value);
    bool set(const QString &t1, const QString &t2, const QString &name, double value);
    bool set(const QString &t1, const QString &t2, const QString &t3, const QString &name, double value);

    bool set(const QString &name, float value);
    bool set(const QString &t1, const QString &name, float value);
    bool set(const QString &t1, const QString &t2, const QString &name, float value);
    bool set(const QString &t1, const QString &t2, const QString &t3, const QString &name, float value);

    bool set(const QString &name, int value);
    bool set(const QString &t1, const QString &name, int value);
    bool set(const QString &t1, const QString &t2, const QString &name, int value);
    bool set(const QString &t1, const QString &t2, const QString &t3, const QString &name, int value);

    bool set(const QString &name, long long value);
    bool set(const QString &t1, const QString &name, long long value);
    bool set(const QString &t1, const QString &t2, const QString &name, long long value);
    bool set(const QString &t1, const QString &t2, const QString &t3, const QString &name, long long value);

    bool set(const QString &name, bool value);
    bool set(const QString &t1, const QString &name, bool value);
    bool set(const QString &t1, const QString &t2, const QString &name, bool value);
    bool set(const QString &t1, const QString &t2, const QString &t3, const QString &name, bool value);

    bool set(const QString &name, const char *value);
    bool set(const QString &t1, const QString &name, const char *value);
    bool set(const QString &t1, const QString &t2, const QString &name, const char *value);
    bool set(const QString &t1, const QString &t2, const QString &t3, const QString &name, const char *value);
private:
    lua_State *m_L = nullptr;

    void loadBuiltinTemplate();
    void setupBuiltinValues();
};

#endif // LUAWRAPPER_H
