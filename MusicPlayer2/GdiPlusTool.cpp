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
