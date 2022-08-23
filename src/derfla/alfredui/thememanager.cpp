#include "stdafx.h"

#include "thememanager.h"
#include "alfredtheme.h"
#include "luavm.h"

ThemeManager::ThemeManager() : m_lua(new LuaVM) {}

AlfredTheme *ThemeManager::applyTheme(const QString &theme)
{
    if (!QFile::exists(theme))
        return nullptr;

    Q_ASSERT(m_lua);
    if (theme.startsWith(":/"))
    {
        QFile f(theme);
        if (!f.open(QIODevice::ReadOnly))
            return nullptr;
        QByteArray ba = f.readAll();
        f.close();

        if (!m_lua->doScript(ba))
            return nullptr;
    }
    else
    {
        if (!m_lua->doFile(theme))
            return nullptr;
    }

    if (!m_theme)
        m_theme = new AlfredTheme;
    m_theme->setBeginHeight(m_lua->getInt("beginHeight"));
    m_theme->setListWidgetY(m_lua->getInt("listWidgetY"));
    m_theme->setDimensions(QSize(m_lua->getInt("dimensions", "width"), m_lua->getInt("dimensions", "height")));
    m_theme->setGroupBoxStylesheet(m_lua->getString("groupBoxStylesheet"));
    m_theme->setPlainTextEditStylesheet(m_lua->getString("plainTextEditStylesheet"));
    m_theme->setListWidgetStylesheet(m_lua->getString("listWidgetStylesheet"));
    m_theme->setFontSize(m_lua->getInt("fontSize"));
    m_theme->setBlurRadius(m_lua->getDouble("blurRadius"));
    m_theme->setShadowColor(QColor(
        m_lua->getInt("shadowColor", "r"), m_lua->getInt("shadowColor", "g"), m_lua->getInt("shadowColor", "b"), m_lua->getInt("shadowColor", "a")));
    m_theme->setShadowOffset(m_lua->getDouble("shadowOffset"));

    return m_theme;
}

ThemeManager::~ThemeManager()
{
    if (m_theme)
        delete m_theme;
    if (m_lua)
        delete m_lua;
}
