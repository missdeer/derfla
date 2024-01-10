#include "stdafx.h"

#include "thememanager.h"
#include "alfredtheme.h"
#include "luavm.h"

ThemeManager::ThemeManager() : m_lua(new LuaVM) {}

AlfredTheme *ThemeManager::loadThemeFromFile(const QString &themeFilePath)
{
    if (!QFile::exists(themeFilePath))
    {
        return nullptr;
    }

    Q_ASSERT(m_lua);
    if (themeFilePath.startsWith(":/"))
    {
        QFile file(themeFilePath);
        if (!file.open(QIODevice::ReadOnly))
        {
            return nullptr;
        }
        QByteArray script = file.readAll();
        file.close();

        if (!m_lua->doScript(script))
        {
            return nullptr;
        }
    }
    else
    {
        if (!m_lua->doFile(themeFilePath))
        {
            return nullptr;
        }
    }

    if (!m_theme)
    {
        m_theme = new AlfredTheme;
    }
    m_theme->setBeginHeight(m_lua->getInt("beginHeight"));
    m_theme->setListWidgetY(m_lua->getInt("listWidgetY"));
    m_theme->setDimensions(QSize(m_lua->getInt("dimensions", "width"), m_lua->getInt("dimensions", "height")));
    m_theme->setGroupBoxStylesheet(m_lua->getQString("groupBoxStylesheet"));
    m_theme->setPlainTextEditStylesheet(m_lua->getQString("plainTextEditStylesheet"));
    m_theme->setListWidgetStylesheet(m_lua->getQString("listWidgetStylesheet"));
    m_theme->setFontSize(m_lua->getInt("fontSize"));
    m_theme->setBlurRadius(m_lua->getDouble("blurRadius"));
    m_theme->setShadowColor(QColor(
        m_lua->getInt("shadowColor", "r"), m_lua->getInt("shadowColor", "g"), m_lua->getInt("shadowColor", "b"), m_lua->getInt("shadowColor", "a")));
    m_theme->setShadowOffset(m_lua->getDouble("shadowOffset"));

    return m_theme;
}

ThemeManager::~ThemeManager()
{
    delete m_theme;
    delete m_lua;
}
