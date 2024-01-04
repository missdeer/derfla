#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

class AlfredTheme;
class LuaVM;

class ThemeManager
{
public:
    ThemeManager();
    ~ThemeManager();
    AlfredTheme *loadThemeFromFile(const QString &theme);

private:
    AlfredTheme *m_theme = nullptr;
    LuaVM       *m_lua   = nullptr;
};

#endif // THEMEMANAGER_H
