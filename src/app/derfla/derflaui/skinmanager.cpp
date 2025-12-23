#include "stdafx.h"

#include <archive.h>
#include <archive_entry.h>

#include "skinmanager.h"
#include "luavm.h"

namespace
{

    // Helper function to read file data from a ZIP archive using libarchive
    QByteArray readFileFromArchive(const QString &archivePath, const QString &fileName)
    {
        QByteArray result;

        struct archive *a = archive_read_new();
        archive_read_support_format_zip(a);
        archive_read_support_filter_all(a);

        if (archive_read_open_filename(a, archivePath.toLocal8Bit().constData(), 10240) != ARCHIVE_OK)
        {
            archive_read_free(a);
            return result;
        }

        struct archive_entry *entry;
        while (archive_read_next_header(a, &entry) == ARCHIVE_OK)
        {
            QString currentFile = QString::fromUtf8(archive_entry_pathname(entry));
            if (currentFile == fileName)
            {
                la_int64_t size = archive_entry_size(entry);
                if (size > 0)
                {
                    result.resize(static_cast<int>(size));
                    archive_read_data(a, result.data(), static_cast<size_t>(size));
                }
                else
                {
                    // Size unknown, read in chunks
                    QByteArray buffer(8192, 0);
                    la_ssize_t readBytes;
                    while ((readBytes = archive_read_data(a, buffer.data(), buffer.size())) > 0)
                    {
                        result.append(buffer.constData(), static_cast<int>(readBytes));
                    }
                }
                break;
            }
            archive_read_data_skip(a);
        }

        archive_read_free(a);
        return result;
    }

    // Helper function to list all files in a ZIP archive using libarchive
    QStringList listFilesInArchive(const QString &archivePath)
    {
        QStringList files;

        struct archive *a = archive_read_new();
        archive_read_support_format_zip(a);
        archive_read_support_filter_all(a);

        if (archive_read_open_filename(a, archivePath.toLocal8Bit().constData(), 10240) != ARCHIVE_OK)
        {
            archive_read_free(a);
            return files;
        }

        struct archive_entry *entry;
        while (archive_read_next_header(a, &entry) == ARCHIVE_OK)
        {
            files.append(QString::fromUtf8(archive_entry_pathname(entry)));
            archive_read_data_skip(a);
        }

        archive_read_free(a);
        return files;
    }

} // anonymous namespace

void SkinManager::makeSkinImages(int cutTop, int cutBottom)
{
    if (cutTop >= 0 && cutBottom > cutTop)
    {
        QPixmap topPartBackgroundImage    = backgroundImage_.copy(0, 0, skinSize_.width(), cutTop);
        QPixmap cutPartBackgroundImage    = backgroundImage_.copy(0, cutTop, skinSize_.width(), cutBottom - cutTop);
        QPixmap bottomPartBackgroundImage = backgroundImage_.copy(0, cutBottom, skinSize_.width(), skinSize_.height() - cutBottom);
        skinSize_.setHeight(skinSize_.height() - (cutBottom - cutTop));
        qDebug() << topPartBackgroundImage.size() << cutPartBackgroundImage.size() << bottomPartBackgroundImage.size() << skinSize_
                 << backgroundImage_.size();
        QPixmap t(skinSize_);
        t.fill(Qt::transparent);
        QPainter painter(&t);
        painter.drawPixmap(0, 0, skinSize_.width(), cutTop, topPartBackgroundImage);
        painter.drawPixmap(0, cutTop, skinSize_.width(), skinSize_.height() - cutTop, bottomPartBackgroundImage);
        backgroundImage_ = t.copy(0, 0, skinSize_.width(), skinSize_.height());
    }

    if (skinSize_.width() < widgetMinWidth_)
    {
        leftPartBackgroundImage_ = backgroundImage_.copy(0, 0, skinSize_.width() / 2 - 1, skinSize_.height());
        midPartBackgroundImage_  = backgroundImage_.copy(skinSize_.width() / 2 - 1, 0, 2, skinSize_.height())
                                      .scaled(widgetMinWidth_ - (skinSize_.width() - 2), skinSize_.height());
        rightPartBackgroundImage_ = backgroundImage_.copy(skinSize_.width() / 2 + 1, 0, skinSize_.width() / 2 - 1, skinSize_.height());

        skinSize_.setWidth(widgetMinWidth_);
    }
}

bool SkinManager::applyDerflaSkin(const QString &skinPath)
{
    int       cutTop = -1, cutBottom = -1;
    QFileInfo fi(skinPath);
    if ((fi.suffix().toLower() == "zip" && !loadDerflaSkinPackage(skinPath, cutTop, cutBottom)) ||
        (fi.suffix().toLower() == "derflaskin" && !loadDerflaSkinConfigurationFile(skinPath, cutTop, cutBottom)))
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
    QStringList fileList = listFilesInArchive(skinPath);
    QString     configFile;

    for (const auto &file : fileList)
    {
        if (file == "skin.derflaskin" || file == QFileInfo(skinPath).completeBaseName() % ".derflaskin")
        {
            configFile = file;
            break;
        }
    }

    if (configFile.isEmpty())
    {
        qCritical() << "can't find configuration file in skin " << skinPath;
        return false;
    }

    QByteArray configuration = readFileFromArchive(skinPath, configFile);
    if (configuration.isEmpty())
    {
        qCritical() << "can't read configuration file from skin " << skinPath;
        return false;
    }

    LuaVM vm;
    if (!vm.doScript(configuration))
    {
        qCritical() << "can't parse skin configuration file" << configuration;
        return false;
    }

    QString image = vm.getQString("image");
    if (image.isEmpty())
    {
        qCritical() << "missing image element in skin configuration file" << configuration;
        return false;
    }

    QByteArray bgImageData = readFileFromArchive(skinPath, image);
    if (bgImageData.isEmpty() || !backgroundImage_.loadFromData(bgImageData))
    {
        qCritical() << "can't load picture from " << image;
        return false;
    }
    skinSize_ = backgroundImage_.size();

    inputStyle_ = vm.getQString("inputstyle");
    if (inputStyle_.isEmpty())
    {
        qCritical() << "missing inputstyle element in skin configuration file" << configuration;
        return false;
    }

    cutTop    = vm.getInt("cuttop");
    cutBottom = vm.getInt("cutbottom");
    return true;
}

bool SkinManager::loadDerflaSkinConfigurationFile(const QString &skinPath, int &cutTop, int &cutBottom)
{
    LuaVM vm;
    if (!vm.doFile(skinPath))
    {
        qCritical() << "can't parse skin configuration file" << skinPath;
        return false;
    }

    QString image = vm.getQString("image");
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

    inputStyle_ = vm.getQString("inputstyle");
    if (inputStyle_.isEmpty())
    {
        qCritical() << "missing inputstyle element in skin configuration file" << skinPath;
        return false;
    }

    cutTop    = vm.getInt("cuttop");
    cutBottom = vm.getInt("cutbottom");
    return true;
}

bool SkinManager::applySkin(const QString &skin)
{
    QMap<QString, std::function<bool(const QString &)>> m = {
        {"zip", std::bind(&SkinManager::applyDerflaSkin, this, std::placeholders::_1)},
        {"derflaskin", std::bind(&SkinManager::applyDerflaSkin, this, std::placeholders::_1)},
    };
    QFileInfo fi(skin);
    auto      it = m.find(fi.suffix().toLower());
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
