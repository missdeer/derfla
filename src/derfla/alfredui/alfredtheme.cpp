#include "alfredtheme.h"

AlfredTheme::AlfredTheme(const QSize &dimensions,
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

const QSize &AlfredTheme::dimensions() const
{
    return mDimensions;
}

void AlfredTheme::setDimensions(const QSize &dimensions)
{
    mDimensions = dimensions;
}

const QString &AlfredTheme::groupBoxStylesheet() const
{
    return mGroupBoxStylesheet;
}

void AlfredTheme::setGroupBoxStylesheet(const QString &groupBoxStylesheet)
{
    mGroupBoxStylesheet = groupBoxStylesheet;
}

const QString & AlfredTheme::plainTextEditStylesheet() const
{
    return mPlainTextEditStylesheet;
}

void AlfredTheme::setPlainTextEditStylesheet(const QString &plainTextEditStylesheet)
{
    mPlainTextEditStylesheet = plainTextEditStylesheet;
}

const QString &AlfredTheme::listWidgetStylesheet() const
{
    return mListWidgetStylesheet;
}

void AlfredTheme::setListWidgetStylesheet(const QString &listWidgetStylesheet)
{
    mListWidgetStylesheet = listWidgetStylesheet;
}

qreal AlfredTheme::blurRadius() const
{
    return mBlurRadius;
}

void AlfredTheme::setBlurRadius(const qreal &blurRadius)
{
    mBlurRadius = blurRadius;
}

const QColor & AlfredTheme::shadowColor() const
{
    return mShadowColor;
}

void AlfredTheme::setShadowColor(const QColor &shadowColor)
{
    mShadowColor = shadowColor;
}

qreal AlfredTheme::shadowOffset() const
{
    return mShadowOffset;
}

void AlfredTheme::setShadowOffset(const qreal &shadowOffset)
{
    mShadowOffset = shadowOffset;
}

int AlfredTheme::fontSize() const
{
    return mFontSize;
}

void AlfredTheme::setFontSize(int fontSize)
{
    mFontSize = fontSize;
}

int AlfredTheme::beginHeight() const
{
    return mBeginHeight;
}

void AlfredTheme::setBeginHeight(int beginHeight)
{
    mBeginHeight = beginHeight;
}

int AlfredTheme::listWidgetY() const
{
    return mListWidgetY;
}

void AlfredTheme::setListWidgetY(int listWidgetY)
{
    mListWidgetY = listWidgetY;
}

