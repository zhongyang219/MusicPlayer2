#pragma once
#include <gdiplus.h>

class CGdiPlusTool
{
public:
    CGdiPlusTool();
    ~CGdiPlusTool();

    static Gdiplus::Color COLORREFToGdiplusColor(COLORREF color, BYTE alpha = 255);
};

