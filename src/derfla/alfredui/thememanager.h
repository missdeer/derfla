#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

class AlfredTheme;
class LuaWrapper;

class ThemeManager
{
public:
    ThemeManager();
    AlfredTheme *applyTheme(const QString& theme);
    ~ThemeManager();
private:
    AlfredTheme *m_theme = nullptr;
    LuaWrapper *m_lua = nullptr;
};

#endif // THEMEMANAGER_H
