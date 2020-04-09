#include "stdafx.h"
#include "GdiPlusTool.h"


CGdiPlusTool::CGdiPlusTool()
{
}


CGdiPlusTool::~CGdiPlusTool()
{
}

Gdiplus::Color CGdiPlusTool::COLORREFToGdiplusColor(COLORREF color, BYTE alpha /*= 255*/)
{
    Gdiplus::Color gdiplusColor;
    gdiplusColor.SetValue(Gdiplus::Color::MakeARGB(alpha, GetRValue(color), GetGValue(color), GetBValue(color)));
    return gdiplusColor;
}

int CGdiPlusTool::ToGDIPluseFontStyle(const FontStyle& style)
{
    int value = 0;
    if (style.bold)
        value |= Gdiplus::FontStyle::FontStyleBold;
    if (style.italic)
        value |= Gdiplus::FontStyle::FontStyleItalic;
    if (style.underline)
        value |= Gdiplus::FontStyle::FontStyleUnderline;
    if (style.strike_out)
        value |= Gdiplus::FontStyle::FontStyleStrikeout;
    return value;
}
