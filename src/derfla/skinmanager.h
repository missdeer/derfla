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

    const QSize &size() const;

private:
    const int widgetMinWidth_ = 450;
    QPixmap backgroundImage_;
    QPixmap leftPartBackgroundImage_;
    QPixmap midPartBackgroundImage_;
    QPixmap rightPartBackgroundImage_;
    QString inputStyle_;
    QSize size_;

    bool applySogouSkin(const QString& skin);
    bool applySogouMacSkin(const QString& skin);
    bool applyBaiduSkin(const QString& skin);
    bool applyDerflaSkin(const QString& skin);
    bool loadDerflaSkinConfiguration(const QString& configurationPath, QString& bgImagePath, QString& inputStyle, int& cutTop, int& cutBottom);
    bool loadDerflaSkinPackage(const QString& skinPath, QString& configurationPath);
};

#endif // SKINMANAGER_H
