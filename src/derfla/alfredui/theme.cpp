#include "theme.h"

Theme::Theme(const QSize &dimensions, 
             const QString &groupBoxStylesheet, 
             const QString &plainTextEditStylesheet, 
             const QString &listWidgetStylesheet,
             int fontSize, 
             qreal blurRadius, 
             const QColor &shadowColor, 
             qreal shadowOffset) 
    : mDimensions(dimensions)
    , mGroupBoxStylesheet(groupBoxStylesheet)
    , mPlainTextEditStylesheet(plainTextEditStylesheet)
    , mListWidgetStylesheet(listWidgetStylesheet)
    , mFontSize(fontSize)
    , mBlurRadius(blurRadius)
    , mShadowColor(shadowColor)
    , mShadowOffset(shadowOffset)
{

}

const QSize &Theme::dimensions() const
{
    return mDimensions;
}

void Theme::setDimensions(const QSize &dimensions)
{
    mDimensions = dimensions;
}

const QString &Theme::groupBoxStylesheet() const
{
    return mGroupBoxStylesheet;
}

void Theme::setGroupBoxStylesheet(const QString &groupBoxStylesheet)
{
    mGroupBoxStylesheet = groupBoxStylesheet;
}

const QString & Theme::plainTextEditStylesheet() const
{
    return mPlainTextEditStylesheet;
}

void Theme::setPlainTextEditStylesheet(const QString &plainTextEditStylesheet)
{
    mPlainTextEditStylesheet = plainTextEditStylesheet;
}

const QString &Theme::listWidgetStylesheet() const
{
    return mListWidgetStylesheet;
}

void Theme::setListWidgetStylesheet(const QString &listWidgetStylesheet)
{
    mListWidgetStylesheet = listWidgetStylesheet;
}

qreal Theme::blurRadius() const
{
    return mBlurRadius;
}

void Theme::setBlurRadius(const qreal &blurRadius)
{
    mBlurRadius = blurRadius;
}

const QColor & Theme::shadowColor() const
{
    return mShadowColor;
}

void Theme::setShadowColor(const QColor &shadowColor)
{
    mShadowColor = shadowColor;
}

qreal Theme::shadowOffset() const
{
    return mShadowOffset;
}

void Theme::setShadowOffset(const qreal &shadowOffset)
{
    mShadowOffset = shadowOffset;
}

int Theme::fontSize() const
{
    return mFontSize;
}

void Theme::setFontSize(int fontSize)
{
    mFontSize = fontSize;
}

int Theme::beginHeight() const
{
    return mBeginHeight;
}

void Theme::setBeginHeight(int beginHeight)
{
    mBeginHeight = beginHeight;
}

int Theme::listWidgetY() const
{
    return mListWidgetY;
}

void Theme::setListWidgetY(int listWidgetY)
{
    mListWidgetY = listWidgetY;
}

