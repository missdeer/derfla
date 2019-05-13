#ifndef THEME_H
#define THEME_H

#include <QSize>
#include <QString>
#include <QColor>

class Theme
{
public:
    Theme() = default;
    Theme(const QSize &dimensions, 
          const QString &groupBoxStylesheet, 
          const QString &plainTextEditStylesheet, 
          const QString &listWidgetStylesheet, 
          int fontSize, 
          qreal blurRadius, 
          const QColor &shadowColor, 
          qreal shadowOffset);
    ~Theme() = default;

    const QSize & dimensions() const;
    void setDimensions(const QSize &dimensions);

    const QString & groupBoxStylesheet() const;
    void setGroupBoxStylesheet(const QString &groupBoxStylesheet);

    const QString &plainTextEditStylesheet() const;
    void setPlainTextEditStylesheet(const QString &plainTextEditStylesheet);

    const QString & listWidgetStylesheet() const;
    void setListWidgetStylesheet(const QString &listWidgetStylesheet);

    qreal blurRadius() const;
    void setBlurRadius(const qreal &blurRadius);

    const QColor & shadowColor() const;
    void setShadowColor(const QColor &shadowColor);

    qreal shadowOffset() const;
    void setShadowOffset(const qreal &shadowOffset);

    int fontSize() const;
    void setFontSize(int value);

    int beginHeight() const;
    void setBeginHeight(int beginHeight);

    int listWidgetY() const;
    void setListWidgetY(int listWidgetY);

private:
    int mBeginHeight;
    int mListWidgetY;
    QSize mDimensions;
    QString mGroupBoxStylesheet;
    QString mPlainTextEditStylesheet;
    QString mListWidgetStylesheet;
    int mFontSize;
    qreal mBlurRadius;
    QColor mShadowColor;
    qreal mShadowOffset;
};

#endif // THEME_H
