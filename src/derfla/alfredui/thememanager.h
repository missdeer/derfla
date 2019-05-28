#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

class AlfredTheme;

class ThemeManager
{
public:
    AlfredTheme *applyTheme(const QString& theme);
    ~ThemeManager();
private:
    AlfredTheme *m_theme = nullptr;
};

#endif // THEMEMANAGER_H
