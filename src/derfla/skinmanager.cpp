#include "stdafx.h"
#include <private/qzipreader_p.h>
#include "skinmanager.h"

SkinManager::SkinManager()
{

}

bool SkinManager::applyDerflaSkin(const QString &skin)
{
    QString s;
    if (!QFileInfo::exists(skin))
    {
        if (applySkin(QString(":/skins/%1.derflaskin").arg(skin)))
            return true;
        // load by skin name
        s = QApplication::applicationDirPath();
        const QString skinPath = QString("/skins/%1.xml").arg(skin);
#if defined(Q_OS_MAC)
        QDir d(s);
        d.cdUp();
        d.cd("Resources");
        s = d.absolutePath() + skinPath;
#else
        s += skinPath;
#endif
        if (!QFile::exists(s))
        {
            // load by skin package - *.derflaskin, should be decompressed first
            int index = s.lastIndexOf(".xml");
            Q_ASSERT(index > 0);
            s.remove(index, 4);
            s.append(".derflaskin");
            if (!loadDerflaSkinPackage(s, s))
            {
                return false;
            }
        }
    }
    else
    {
        QFileInfo fi(skin);
        if (fi.suffix() == "xml")
        {
            // load by skin configuration file - *.xml
            s = skin;
        }
        else
        {
            // load by skin package - *.derflaskin, should be decompressed first
            if (!loadDerflaSkinPackage(skin, s))
            {
                return false;
            }
        }
    }

    QString imagePath;
    int cutTop = -1, cutBottom = -1;
    if (!loadDerflaSkinConfiguration(s, imagePath, inputStyle_, cutTop, cutBottom))
    {
        return false;
    }

    if (!backgroundImage_.load(imagePath))
    {
        qCritical() << "can't load picture from " << imagePath;
        return false;
    }

    size_ = backgroundImage_.size();

    if (cutTop >= 0 && cutBottom > cutTop)
    {
        QPixmap topPartBackgroundImage = backgroundImage_.copy(0, 0, size_.width(), cutTop);
        QPixmap cutPartBackgroundImage = backgroundImage_.copy(0, cutTop, size_.width(), cutBottom - cutTop);
        QPixmap bottomPartBackgroundImage = backgroundImage_.copy(0, cutBottom, size_.width(), size_.height() - cutBottom);
        size_.setHeight(size_.height() - (cutBottom - cutTop));
        qDebug() << topPartBackgroundImage.size() << cutPartBackgroundImage.size() << bottomPartBackgroundImage.size() << size_ << backgroundImage_.size();
        QPixmap t(size_);
        t.fill(Qt::transparent);
        QPainter painter(&t);
        painter.drawPixmap(0, 0, size_.width(), cutTop, topPartBackgroundImage);
        painter.drawPixmap(0, cutTop, size_.width(), size_.height()- cutTop, bottomPartBackgroundImage);
        backgroundImage_ = t.copy(0, 0, size_.width(), size_.height());
    }

    if (size_.width() < widgetMinWidth_)
    {
        leftPartBackgroundImage_ = backgroundImage_.copy(0, 0,
                                                         size_.width() / 2 -1, size_.height());
        midPartBackgroundImage_ = backgroundImage_.copy(size_.width() / 2 - 1, 0,
                                                        2, size_.height()).scaled(widgetMinWidth_ - (size_.width() - 2), size_.height());
        rightPartBackgroundImage_ = backgroundImage_.copy(size_.width() / 2 + 1, 0,
                                                          size_.width() / 2 - 1, size_.height());

        size_.setWidth(widgetMinWidth_);
    }

    return true;
}

bool SkinManager::loadDerflaSkinConfiguration(const QString &configurationPath, QString &bgImagePath, QString &inputStyle, int &cutTop, int &cutBottom)
{
    QDomDocument doc;
    QFile file(configurationPath);
    if (!file.open(QIODevice::ReadOnly))
    {
        qCritical() << "can't open skin configuration file" << configurationPath;
        return false;
    }

    if (!doc.setContent(&file))
    {
        qCritical() << "can't parse skin configuration file" << configurationPath;
        file.close();
        return false;
    }
    file.close();

    QDomElement docElem = doc.documentElement();
    QDomElement imageElem = docElem.firstChildElement("image");
    if (imageElem.isNull())
    {
        qCritical() << "missing image element in skin configuration file" << configurationPath;
        return false;
    }

    QFileInfo cfg(configurationPath);

    bgImagePath = QString("%1/%2").arg(cfg.absolutePath()).arg(imageElem.text());

    QDomElement issElem = docElem.firstChildElement("inputstyle");
    if (issElem.isNull())
    {
        qCritical() << "missing inputstyle element in skin configuration file" << configurationPath;
        return false;
    }
    inputStyle = issElem.text();

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

bool SkinManager::loadDerflaSkinPackage(const QString &skinPath, QString &configurationPath)
{
    QString dirName = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) % "/SkinTmp";
    QDir dir(dirName);
    dir.removeRecursively();
    dir.mkpath(dirName);
    QZipReader zr(skinPath);
    bool res = zr.extractAll(dirName);
    if (!res)
    {
        qCritical() << "extracting" << skinPath << "to" << dirName << "failed";
        return false;
    }
    configurationPath = dirName % "/skin.xml";
    if (QFile::exists(configurationPath))
        return true;

    configurationPath = dirName % "/" % QFileInfo(skinPath).completeBaseName() % ".xml";
    if (QFile::exists(configurationPath))
        return true;

    configurationPath.clear();
    qCritical() << "can't find configuration file in skin package" << skinPath;
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

const QSize &SkinManager::size() const
{
    return size_;
}

bool SkinManager::applySogouSkin(const QString &skin)
{
    return false;
}

bool SkinManager::applySogouMacSkin(const QString &skin)
{
    return false;
}

bool SkinManager::applyBaiduSkin(const QString &skin)
{
    return false;
}

bool SkinManager::applySkin(const QString &skin)
{
    QFileInfo fi(skin);
    if (fi.suffix().toLower() == "xml" || fi.suffix().toLower() == "derflaskin")
        return applyDerflaSkin(skin);

    if (fi.suffix().toLower() == "bps")
        return applyBaiduSkin(skin);

    if (fi.suffix().toLower() == "ssf")
        return applySogouSkin(skin);

    if (fi.suffix().toLower() == "mssf")
        return applySogouMacSkin(skin);

    return false;
}
