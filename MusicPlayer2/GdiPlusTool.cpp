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
    return Gdiplus::Color(alpha, GetRValue(color), GetGValue(color), GetBValue(color));
}

COLORREF CGdiPlusTool::GdiplusColorToCOLORREF(Gdiplus::Color color)
{
    return RGB(color.GetR(), color.GetG(), color.GetB());
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

void CGdiPlusTool::CreateRoundRectPath(Gdiplus::GraphicsPath& path, CRect rect, int r)
{
    int L{ rect.left }, T{ rect.top }, R{ rect.right }, B{ rect.bottom }, d{ 2 * r };
    path.AddArc(L, T, d, d, 180, 90);         // 左上角圆弧
    path.AddLine(L + r, T, R - r, T);         // 上边

    path.AddArc(R - d, T, d, d, 270, 90);     // 右上角圆弧
    path.AddLine(R, T + r, R, B - r);         // 右边

    path.AddArc(R - d, B - d, d, d, 0, 90);   // 右下角圆弧
    path.AddLine(R - r, B, L + r, B);         // 下边

    path.AddArc(L, B - d, d, d, 90, 90);
    path.AddLine(L, T + r, L, B - r);

}

CRect CGdiPlusTool::GdiplusRectToCRect(Gdiplus::Rect rect)
{
    return CRect(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
}

Gdiplus::Rect CGdiPlusTool::CRectToGdiplusRect(CRect rect)
{
    return Gdiplus::Rect(rect.left, rect.top, rect.Width(), rect.Height());
}
