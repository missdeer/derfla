#include "stdafx.h"
#include "luawrapper.h"
#include "alfredtheme.h"
#include "thememanager.h"

AlfredTheme *ThemeManager::applyTheme(const QString &theme)
{
    if (!QFile::exists(theme))
        return nullptr;

    LuaWrapper w;
    w.createLuaState();
    if (theme.startsWith(":/"))
    {
        QFile f(theme);
        if (!f.open(QIODevice::ReadOnly))
            return nullptr;
        QByteArray ba = f.readAll();
        f.close();

        if (!w.doScript(ba))
            return nullptr;
    } else {
        if (!w.doFile(theme))
            return nullptr;
    }

    if (!m_theme)
        m_theme = new AlfredTheme;
    m_theme->setBeginHeight(w.getInt("beginHeight"));
    m_theme->setListWidgetY(w.getInt("listWidgetY"));
    m_theme->setDimensions(QSize(w.getInt("dimensions", "width"),
                           w.getInt("dimensions", "height")));
    m_theme->setGroupBoxStylesheet(w.getString("groupBoxStylesheet"));
    m_theme->setPlainTextEditStylesheet(w.getString("plainTextEditStylesheet"));
    m_theme->setListWidgetStylesheet(w.getString("listWidgetStylesheet"));
    m_theme->setFontSize(w.getInt("fontSize"));
    m_theme->setBlurRadius(w.getDouble("blurRadius"));
    m_theme->setShadowColor(QColor(w.getInt("shadowColor", "r"),
                             w.getInt("shadowColor", "g"),
                             w.getInt("shadowColor", "b"),
                             w.getInt("shadowColor", "a")));
    m_theme->setShadowOffset(w.getDouble("shadowOffset"));
    
    return m_theme;
}

ThemeManager::~ThemeManager()
{
    if (m_theme)
        delete m_theme;
}
