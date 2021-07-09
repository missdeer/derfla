#include "stdafx.h"
#include <private/qzipreader_p.h>
#include "luavm.h"
#include "skinmanager.h"

void SkinManager::makeSkinImages(int cutTop, int cutBottom)
{
    if (cutTop >= 0 && cutBottom > cutTop)
    {
        QPixmap topPartBackgroundImage = backgroundImage_.copy(0, 0, skinSize_.width(), cutTop);
        QPixmap cutPartBackgroundImage = backgroundImage_.copy(0, cutTop, skinSize_.width(), cutBottom - cutTop);
        QPixmap bottomPartBackgroundImage = backgroundImage_.copy(0, cutBottom, skinSize_.width(), skinSize_.height() - cutBottom);
        skinSize_.setHeight(skinSize_.height() - (cutBottom - cutTop));
        qDebug() << topPartBackgroundImage.size() << cutPartBackgroundImage.size() << bottomPartBackgroundImage.size() << skinSize_ << backgroundImage_.size();
        QPixmap t(skinSize_);
        t.fill(Qt::transparent);
        QPainter painter(&t);
        painter.drawPixmap(0, 0, skinSize_.width(), cutTop, topPartBackgroundImage);
        painter.drawPixmap(0, cutTop, skinSize_.width(), skinSize_.height()- cutTop, bottomPartBackgroundImage);
        backgroundImage_ = t.copy(0, 0, skinSize_.width(), skinSize_.height());
    }

    if (skinSize_.width() < widgetMinWidth_)
    {
        leftPartBackgroundImage_ = backgroundImage_.copy(0, 0,
                                                         skinSize_.width() / 2 -1, skinSize_.height());
        midPartBackgroundImage_ = backgroundImage_.copy(skinSize_.width() / 2 - 1, 0,
                                                        2, skinSize_.height()).scaled(widgetMinWidth_ - (skinSize_.width() - 2), skinSize_.height());
        rightPartBackgroundImage_ = backgroundImage_.copy(skinSize_.width() / 2 + 1, 0,
                                                          skinSize_.width() / 2 - 1, skinSize_.height());

        skinSize_.setWidth(widgetMinWidth_);
    }
}

bool SkinManager::applyDerflaSkin(const QString &skinPath)
{
    int cutTop = -1, cutBottom = -1;
    QFileInfo fi(skinPath);
    if ((fi.suffix().toLower() == "zip" && !loadDerflaSkinPackage(skinPath, cutTop, cutBottom))
            || (fi.suffix().toLower() == "derflaskin" && !loadDerflaSkinConfigurationFile(skinPath, cutTop, cutBottom)))
    {
        // load by skin package - *.zip, should be decompressed first
        // load by skin configuration file - *.derflaskin
        return false;
    }

    makeSkinImages(cutTop, cutBottom);

    return true;
}

bool SkinManager::loadDerflaSkinPackage(const QString &skinPath, int &cutTop, int &cutBottom)
{
    QZipReader zr(skinPath);

    auto fil = zr.fileInfoList();
    for (auto & fi : fil)
    {
        if (fi.filePath == "skin.derflaskin" || fi.filePath == QFileInfo(skinPath).completeBaseName() % ".derflaskin")
        {
            auto configuration = zr.fileData(fi.filePath);
            
            LuaVM vm;
            if (!vm.doScript(configuration))
            {
                qCritical() << "can't parse skin configuration file" << configuration;
                return false;
            }
            
            QString image = vm.getString("image");
            if (image.isEmpty())
            {
                qCritical() << "missing image element in skin configuration file" << configuration;
                return false;
            }

            auto bgImageData = zr.fileData(image);
            if (!backgroundImage_.loadFromData(bgImageData))
            {
                qCritical() << "can't load picture from " << image;
                return false;
            }
            skinSize_ = backgroundImage_.size();

            inputStyle_ = vm.getString("inputstyle");
            if (inputStyle_.isEmpty())
            {
                qCritical() << "missing inputstyle element in skin configuration file" << configuration;
                return false;
            }

            cutTop = vm.getInt("cuttop");
            cutBottom = vm.getInt("cutbottom");
            return true;
        }
    }

    qCritical() << "can't find configuration file in skin " << skinPath;
    return false;
}

bool SkinManager::loadDerflaSkinConfigurationFile(const QString &skinPath, int &cutTop, int &cutBottom)
{
    LuaVM vm;
    if (!vm.doFile(skinPath))
    {
        qCritical() << "can't parse skin configuration file" << skinPath;
        return false;
    }
    
    QString image = vm.getString("image");
    if (image.isEmpty())
    {
        qCritical() << "missing image element in skin configuration file" << skinPath;
        return false;
    }

    QFileInfo cfg(skinPath);

    QString bgImagePath = cfg.absolutePath() % image;
    if (!backgroundImage_.load(bgImagePath))
    {
        qCritical() << "can't load picture from " << bgImagePath;
        return false;
    }
    skinSize_ = backgroundImage_.size();
    
    inputStyle_ = vm.getString("inputstyle");
    if (inputStyle_.isEmpty())
    {
        qCritical() << "missing inputstyle element in skin configuration file" << skinPath;
        return false;
    }

    cutTop = vm.getInt("cuttop");
    cutBottom = vm.getInt("cutbottom");
    return true;
}

bool SkinManager::applySkin(const QString &skin)
{
    QMap<QString, std::function<bool(const QString&)>> m = {
    { "zip",        std::bind(&SkinManager::applyDerflaSkin,   this, std::placeholders::_1)},
    { "derflaskin", std::bind(&SkinManager::applyDerflaSkin,   this, std::placeholders::_1)},
};
    QFileInfo fi(skin);
    auto it = m.find(fi.suffix().toLower());
    if (m.end() != it)
    {
        auto f = it.value();
        return f(skin);
    }

    return false;
}

const QPixmap &SkinManager::backgroundImage() const
{
    return backgroundImage_;
}

const QPixmap &SkinManager::leftPartBackgroundImage() const
{
    return leftPartBackgroundImage_;
}

const QPixmap &SkinManager::midPartBackgroundImage() const
{
    return midPartBackgroundImage_;
}

const QPixmap &SkinManager::rightPartBackgroundImage() const
{
    return rightPartBackgroundImage_;
}

const QString &SkinManager::inputStyle() const
{
    return inputStyle_;
}

const QSize &SkinManager::skinSize() const
{
    return skinSize_;
}
