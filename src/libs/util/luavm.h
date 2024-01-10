#ifndef LUAWRAPPER_H
#define LUAWRAPPER_H

#include <lua.hpp>

#include <QStringList>

#include "util_global.h"

class UTILSHARED_EXPORT LuaVM
{
public:
    LuaVM();
    ~LuaVM();
    LuaVM(const LuaVM &)            = delete;
    LuaVM &operator=(const LuaVM &) = delete;
    LuaVM(LuaVM &&)                 = delete;
    LuaVM &operator=(LuaVM &&)      = delete;

    void start();
    void shutdown();
    bool doFile(const QString &file);
    bool doScript(const QString &script);

    bool getQStringArray(const QString &name, QStringList &array);

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

    QString getQString(const QString &name);
    QString getQString(const QString &table, const QString &name);
    QString getQString(const QString &t1, const QString &t2, const QString &name);
    QString getQString(const QString &t1, const QString &t2, const QString &t3, const QString &name);

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

    bool doFile(const std::string &file);
    bool doScript(const std::string &script);

    bool getQStringArray(const std::string &name, QStringList &array);

    double getDouble(const std::string &name);
    double getDouble(const std::string &table, const std::string &name);
    double getDouble(const std::string &t1, const std::string &t2, const std::string &name);
    double getDouble(const std::string &t1, const std::string &t2, const std::string &t3, const std::string &name);

    float getFloat(const std::string &name);
    float getFloat(const std::string &table, const std::string &name);
    float getFloat(const std::string &t1, const std::string &t2, const std::string &name);
    float getFloat(const std::string &t1, const std::string &t2, const std::string &t3, const std::string &name);

    int getInt(const std::string &name);
    int getInt(const std::string &table, const std::string &name);
    int getInt(const std::string &t1, const std::string &t2, const std::string &name);
    int getInt(const std::string &t1, const std::string &t2, const std::string &t3, const std::string &name);

    long long getLongLong(const std::string &name);
    long long getLongLong(const std::string &table, const std::string &name);
    long long getLongLong(const std::string &t1, const std::string &t2, const std::string &name);
    long long getLongLong(const std::string &t1, const std::string &t2, const std::string &t3, const std::string &name);

    bool getBool(const std::string &name);
    bool getBool(const std::string &table, const std::string &name);
    bool getBool(const std::string &t1, const std::string &t2, const std::string &name);
    bool getBool(const std::string &t1, const std::string &t2, const std::string &t3, const std::string &name);

    QString getQString(const std::string &name);
    QString getQString(const std::string &table, const std::string &name);
    QString getQString(const std::string &t1, const std::string &t2, const std::string &name);
    QString getQString(const std::string &t1, const std::string &t2, const std::string &t3, const std::string &name);

    std::string getString(const std::string &name);
    std::string getString(const std::string &table, const std::string &name);
    std::string getString(const std::string &t1, const std::string &t2, const std::string &name);
    std::string getString(const std::string &t1, const std::string &t2, const std::string &t3, const std::string &name);

    bool set(const std::string &name, double value);
    bool set(const std::string &t1, const std::string &name, double value);
    bool set(const std::string &t1, const std::string &t2, const std::string &name, double value);
    bool set(const std::string &t1, const std::string &t2, const std::string &t3, const std::string &name, double value);

    bool set(const std::string &name, float value);
    bool set(const std::string &t1, const std::string &name, float value);
    bool set(const std::string &t1, const std::string &t2, const std::string &name, float value);
    bool set(const std::string &t1, const std::string &t2, const std::string &t3, const std::string &name, float value);

    bool set(const std::string &name, int value);
    bool set(const std::string &t1, const std::string &name, int value);
    bool set(const std::string &t1, const std::string &t2, const std::string &name, int value);
    bool set(const std::string &t1, const std::string &t2, const std::string &t3, const std::string &name, int value);

    bool set(const std::string &name, long long value);
    bool set(const std::string &t1, const std::string &name, long long value);
    bool set(const std::string &t1, const std::string &t2, const std::string &name, long long value);
    bool set(const std::string &t1, const std::string &t2, const std::string &t3, const std::string &name, long long value);

    bool set(const std::string &name, bool value);
    bool set(const std::string &t1, const std::string &name, bool value);
    bool set(const std::string &t1, const std::string &t2, const std::string &name, bool value);
    bool set(const std::string &t1, const std::string &t2, const std::string &t3, const std::string &name, bool value);

    bool set(const std::string &name, const char *value);
    bool set(const std::string &t1, const std::string &name, const char *value);
    bool set(const std::string &t1, const std::string &t2, const std::string &name, const char *value);
    bool set(const std::string &t1, const std::string &t2, const std::string &t3, const std::string &name, const char *value);

    bool doFile(const char *file);
    bool doScript(const char *script, size_t len);

    bool getQStringArray(const char *name, QStringList &array);

    double getDouble(const char *name);
    double getDouble(const char *table, const char *name);
    double getDouble(const char *t1, const char *t2, const char *name);
    double getDouble(const char *t1, const char *t2, const char *t3, const char *name);

    float getFloat(const char *name);
    float getFloat(const char *table, const char *name);
    float getFloat(const char *t1, const char *t2, const char *name);
    float getFloat(const char *t1, const char *t2, const char *t3, const char *name);

    int getInt(const char *name);
    int getInt(const char *table, const char *name);
    int getInt(const char *t1, const char *t2, const char *name);
    int getInt(const char *t1, const char *t2, const char *t3, const char *name);

    long long getLongLong(const char *name);
    long long getLongLong(const char *table, const char *name);
    long long getLongLong(const char *t1, const char *t2, const char *name);
    long long getLongLong(const char *t1, const char *t2, const char *t3, const char *name);

    bool getBool(const char *name);
    bool getBool(const char *table, const char *name);
    bool getBool(const char *t1, const char *t2, const char *name);
    bool getBool(const char *t1, const char *t2, const char *t3, const char *name);

    QString getQString(const char *name);
    QString getQString(const char *table, const char *name);
    QString getQString(const char *t1, const char *t2, const char *name);
    QString getQString(const char *t1, const char *t2, const char *t3, const char *name);

    std::string getString(const char *name);
    std::string getString(const char *table, const char *name);
    std::string getString(const char *t1, const char *t2, const char *name);
    std::string getString(const char *t1, const char *t2, const char *t3, const char *name);

    bool set(const char *name, double value);
    bool set(const char *t1, const char *name, double value);
    bool set(const char *t1, const char *t2, const char *name, double value);
    bool set(const char *t1, const char *t2, const char *t3, const char *name, double value);

    bool set(const char *name, float value);
    bool set(const char *t1, const char *name, float value);
    bool set(const char *t1, const char *t2, const char *name, float value);
    bool set(const char *t1, const char *t2, const char *t3, const char *name, float value);

    bool set(const char *name, int value);
    bool set(const char *t1, const char *name, int value);
    bool set(const char *t1, const char *t2, const char *name, int value);
    bool set(const char *t1, const char *t2, const char *t3, const char *name, int value);

    bool set(const char *name, long long value);
    bool set(const char *t1, const char *name, long long value);
    bool set(const char *t1, const char *t2, const char *name, long long value);
    bool set(const char *t1, const char *t2, const char *t3, const char *name, long long value);

    bool set(const char *name, bool value);
    bool set(const char *t1, const char *name, bool value);
    bool set(const char *t1, const char *t2, const char *name, bool value);
    bool set(const char *t1, const char *t2, const char *t3, const char *name, bool value);

    bool set(const char *name, const char *value);
    bool set(const char *t1, const char *name, const char *value);
    bool set(const char *t1, const char *t2, const char *name, const char *value);
    bool set(const char *t1, const char *t2, const char *t3, const char *name, const char *value);

private:
    lua_State *m_L = nullptr;

    void loadBuiltinTemplate();
    void setupBuiltinValues();
};

#endif // LUAWRAPPER_H
