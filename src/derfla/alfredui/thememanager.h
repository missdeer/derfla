#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

class AlfredTheme;
class LuaVM;

class ThemeManager
{
public:
    ThemeManager();
    AlfredTheme *applyTheme(const QString& theme);
    ~ThemeManager();
private:
    AlfredTheme *m_theme = nullptr;
    LuaVM *m_lua = nullptr;
};

#endif // THEMEMANAGER_H
