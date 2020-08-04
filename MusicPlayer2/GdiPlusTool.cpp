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

void CGdiPlusTool::CreateRoundRectPath(Gdiplus::GraphicsPath& path, CRect rect, int radius)
{
    int diam{ 2 * radius };
    path.AddArc(rect.left, rect.top, diam, diam, 180, 90); // 左上角圆弧
    path.AddLine(rect.left + radius, rect.top, rect.right - radius, rect.top); // 上边

    path.AddArc(rect.right - diam, rect.top, diam, diam, 270, 90); // 右上角圆弧
    path.AddLine(rect.right, rect.top + radius, rect.right, rect.bottom - radius);// 右边

    path.AddArc(rect.right - diam, rect.bottom - diam, diam, diam, 0, 90); // 右下角圆弧
    path.AddLine(rect.right - radius, rect.bottom, rect.left + radius, rect.bottom); // 下边

    path.AddArc(rect.left, rect.bottom - diam, diam, diam, 90, 90);
    path.AddLine(rect.left, rect.top + radius, rect.left, rect.bottom - radius);

}
