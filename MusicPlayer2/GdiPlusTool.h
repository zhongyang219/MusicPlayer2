#pragma once
#include <gdiplus.h>
#include "CommonData.h"

class CGdiPlusTool
{
public:
    CGdiPlusTool();
    ~CGdiPlusTool();

    static Gdiplus::Color COLORREFToGdiplusColor(COLORREF color, BYTE alpha = 255);
    static COLORREF GdiplusColorToCOLORREF(Gdiplus::Color color);
    static int ToGDIPluseFontStyle(const FontStyle& style);
    static void CreateRoundRectPath(Gdiplus::GraphicsPath& path, CRect rect, int radius);
    static CRect GdiplusRectToCRect(Gdiplus::Rect rect);
    static Gdiplus::Rect CRectToGdiplusRect(CRect rect);
};

