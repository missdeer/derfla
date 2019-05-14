#ifndef SKINMANAGER_H
#define SKINMANAGER_H

#include <QObject>
#include <QPixmap>

class SkinManager
{
public:
    SkinManager();

    bool applySkin(const QString& skin);

    const QPixmap &backgroundImage() const;

    const QPixmap &leftPartBackgroundImage() const;

    const QPixmap &midPartBackgroundImage() const;

    const QPixmap &rightPartBackgroundImage() const;

    const QString &inputStyle() const;

    const QSize &skinSize() const;

private:
    const int widgetMinWidth_ = 450;
    QPixmap backgroundImage_;
    QPixmap leftPartBackgroundImage_;
    QPixmap midPartBackgroundImage_;
    QPixmap rightPartBackgroundImage_;
    QString inputStyle_;
    QSize skinSize_;

    bool applySogouSkin(const QString& skinPath);
    bool loadSogouSkinPackage(const QString& skinPath, int &cutTop, int &cutBottom);
    bool applySogouMacSkin(const QString& skinPath);
    bool loadSogouMacSkinPackage(const QString& skinPath, int &cutTop, int &cutBottom);
    bool applyBaiduSkin(const QString& skinPath);
    bool loadBaiduSkinPackage(const QString& skinPath, int &cutTop, int &cutBottom);
    bool applyDerflaSkin(const QString& skinPath);
    bool loadDerflaSkinPackage(const QString& skinPath, int &cutTop, int &cutBottom);
    bool loadDerflaSkinConfigurationFile(const QString& skinPath, int &cutTop, int &cutBottom);
    void makeSkinImages(int cutTop, int cutBottom);
};

#endif // SKINMANAGER_H
