#include "stdafx.h"
#include <private/qzipreader_p.h>
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
    if ((fi.suffix().toLower() == "derflaskin" && !loadDerflaSkinPackage(skinPath, cutTop, cutBottom))
            || (fi.suffix().toLower() == "xml" && !loadDerflaSkinConfigurationFile(skinPath, cutTop, cutBottom)))
    {
        // load by skin package - *.derflaskin, should be decompressed first
        // load by skin configuration file - *.xml
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
        if (fi.filePath == "skin.xml" || fi.filePath == QFileInfo(skinPath).completeBaseName() % ".xml")
        {
            auto configuration = zr.fileData(fi.filePath);

            QDomDocument doc;
            if (!doc.setContent(configuration))
            {
                qCritical() << "can't parse skin configuration file" << configuration;
                return false;
            }

            QDomElement docElem = doc.documentElement();
            QDomElement imageElem = docElem.firstChildElement("image");
            if (imageElem.isNull())
            {
                qCritical() << "missing image element in skin configuration file" << configuration;
                return false;
            }

            QFileInfo cfg(configuration);

            auto bgImageData = zr.fileData(imageElem.text());
            if (!backgroundImage_.loadFromData(bgImageData))
            {
                qCritical() << "can't load picture from " << imageElem.text();
                return false;
            }
            skinSize_ = backgroundImage_.size();

            QDomElement issElem = docElem.firstChildElement("inputstyle");
            if (issElem.isNull())
            {
                qCritical() << "missing inputstyle element in skin configuration file" << configuration;
                return false;
            }
            inputStyle_ = issElem.text();

            QDomElement cutTopElem = docElem.firstChildElement("cuttop");
            if (!cutTopElem.isNull())
            {
                cutTop = cutTopElem.text().toInt();
            }
            QDomElement cutBottomElem = docElem.firstChildElement("cutbottom");
            if (!cutBottomElem.isNull())
            {
                cutBottom = cutBottomElem.text().toInt();
            }
            return true;
        }
    }

    qCritical() << "can't find configuration file in skin " << skinPath;
    return false;
}

bool SkinManager::loadDerflaSkinConfigurationFile(const QString &skinPath, int &cutTop, int &cutBottom)
{
    QFile f(skinPath);
    if (!f.open(QIODevice::ReadOnly))
    {
        qCritical() << "can't open skin configuration file" << skinPath;
        return false;
    }
    QDomDocument doc;
    if (!doc.setContent(&f))
    {
        qCritical() << "can't parse skin configuration file" << skinPath;
        f.close();
        return false;
    }
    f.close();

    QDomElement docElem = doc.documentElement();
    QDomElement imageElem = docElem.firstChildElement("image");
    if (imageElem.isNull())
    {
        qCritical() << "missing image element in skin configuration file" << skinPath;
        return false;
    }

    QFileInfo cfg(skinPath);

    auto bgImagePath  = QString("%1/%2").arg(cfg.absolutePath()).arg(imageElem.text());
    if (!backgroundImage_.load(bgImagePath))
    {
        qCritical() << "can't load picture from " << bgImagePath;
        return false;
    }
    skinSize_ = backgroundImage_.size();

    QDomElement issElem = docElem.firstChildElement("inputstyle");
    if (issElem.isNull())
    {
        qCritical() << "missing inputstyle element in skin configuration file" << skinPath;
        return false;
    }
    inputStyle_ = issElem.text();

    QDomElement cutTopElem = docElem.firstChildElement("cuttop");
    if (!cutTopElem.isNull())
    {
        cutTop = cutTopElem.text().toInt();
    }
    QDomElement cutBottomElem = docElem.firstChildElement("cutbottom");
    if (!cutBottomElem.isNull())
    {
        cutBottom = cutBottomElem.text().toInt();
    }
    return true;
}

bool SkinManager::applySogouSkin(const QString &skinPath)
{
    int cutTop = -1, cutBottom = -1;
    QFileInfo fi(skinPath);
    if (fi.suffix().toLower() == "ssf" && !loadSogouSkinPackage(skinPath, cutTop, cutBottom))
    {
        // load by skin package - *.ssf, should be decompressed first
        return false;
    }

    makeSkinImages(cutTop, cutBottom);
    return false;
}

bool SkinManager::loadSogouSkinPackage(const QString &skinPath, int &cutTop, int &cutBottom)
{
    QZipReader zr(skinPath);

    auto fil = zr.fileInfoList();
    for (auto & fi : fil)
    {
        if (fi.filePath == "skin.ini")
        {
            return true;
        }
    }
    return false;
}

bool SkinManager::applySogouMacSkin(const QString &skinPath)
{
    int cutTop = -1, cutBottom = -1;
    QFileInfo fi(skinPath);
    if (fi.suffix().toLower() == "mssf" && !loadSogouMacSkinPackage(skinPath, cutTop, cutBottom))
    {
        // load by skin package - *.mssf, should be decompressed first
        return false;
    }

    makeSkinImages(cutTop, cutBottom);
    return false;
}

bool SkinManager::loadSogouMacSkinPackage(const QString &skinPath, int &cutTop, int &cutBottom)
{
    QZipReader zr(skinPath);

    auto skinData = zr.fileData("skin");
    QBuffer buffer(&skinData);
    QZipReader zr2(&buffer);

    auto fil = zr2.fileInfoList();
    for (auto & fi : fil)
    {
        if (fi.filePath == "skin.plist")
        {
            return true;
        }
    }

    return false;
}

bool SkinManager::applyBaiduSkin(const QString &skinPath)
{
    int cutTop = -1, cutBottom = -1;
    QFileInfo fi(skinPath);
    if (fi.suffix().toLower() == "bps" && !loadBaiduSkinPackage(skinPath, cutTop, cutBottom))
    {
        // load by skin package - *.bps, should be decompressed first
        return false;
    }

    makeSkinImages(cutTop, cutBottom);
    return false;
}

bool SkinManager::loadBaiduSkinPackage(const QString &skinPath, int &cutTop, int &cutBottom)
{
    QZipReader zr(skinPath);

    auto fil = zr.fileInfoList();
    for (auto & fi : fil)
    {
        if (fi.filePath == "Candidate.ini")
        {
            return true;
        }
    }
    return false;
}

bool SkinManager::applySkin(const QString &skin)
{
    QMap<QString, std::function<bool(const QString&)>> m = {
    { "xml",        std::bind(&SkinManager::applyDerflaSkin,   this, std::placeholders::_1)},
    { "derflaskin", std::bind(&SkinManager::applyDerflaSkin,   this, std::placeholders::_1)},
    { "bps",        std::bind(&SkinManager::applyBaiduSkin,    this, std::placeholders::_1)},
    { "ssf",        std::bind(&SkinManager::applySogouSkin,    this, std::placeholders::_1)},
    { "mssf",       std::bind(&SkinManager::applySogouMacSkin, this, std::placeholders::_1)},
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
