#include "stdafx.h"
#include "ColorConvert.h"
#include "Common.h"

static ColorTable CreateGrayColor()
{
    ColorTable gray_color;
    gray_color.original_color = GRAY(240);
    CColorConvert::ConvertColor(gray_color);
    return gray_color;
}

ColorTable CColorConvert::m_gray_color = CreateGrayColor();

CColorConvert::CColorConvert()
{
}


CColorConvert::~CColorConvert()
{
}

void CColorConvert::RGBtoHSL(const COLOR_RGB * rgb, COLOR_HSL * hsl)
{
	float h = 0, s = 0, l = 0;
	// normalizes red-green-blue values
	float r = rgb->red / 255.0f;
	float g = rgb->green / 255.0f;
	float b = rgb->blue / 255.0f;
	float maxVal = CCommon::Max3(r, g, b);
	float minVal = CCommon::Min3(r, g, b);

	// hue
	if (maxVal == minVal)
	{
		h = 0; // undefined
	}
	else if (maxVal == r && g >= b)
	{
		h = 60.0f*(g - b) / (maxVal - minVal);
	}
	else if (maxVal == r && g<b)
	{
		h = 60.0f*(g - b) / (maxVal - minVal) + 360.0f;
	}
	else if (maxVal == g)
	{
		h = 60.0f*(b - r) / (maxVal - minVal) + 120.0f;
	}
	else if (maxVal == b)
	{
		h = 60.0f*(r - g) / (maxVal - minVal) + 240.0f;
	}

	// luminance
	l = (maxVal + minVal) / 2.0f;

	// saturation
	if (l == 0 || maxVal == minVal)
	{
		s = 0;
	}
	else if (0<l && l <= 0.5f)
	{
		s = (maxVal - minVal) / (maxVal + minVal);
	}
	else if (l>0.5f)
	{
		s = (maxVal - minVal) / (2 - (maxVal + minVal)); //(maxVal-minVal > 0)?
	}

	hsl->hue = (h>360) ? 360 : ((h<0) ? 0 : h);
	hsl->saturation = ((s>1) ? 1 : ((s<0) ? 0 : s)) * 100;
	hsl->luminance = ((l>1) ? 1 : ((l<0) ? 0 : l)) * 100;
}

void CColorConvert::HSLtoRGB(const COLOR_HSL * hsl, COLOR_RGB * rgb)
{
	float h = hsl->hue;                  // h must be [0, 360]
	float s = hsl->saturation / 100.f; // s must be [0, 1]
	float l = hsl->luminance / 100.f;      // l must be [0, 1]
	float R, G, B;
	if (hsl->saturation == 0)
	{
		// achromatic color (gray scale)
		R = G = B = l*255.0f;
	}
	else
	{
		float q = (l<0.5f) ? (l * (1.0f + s)) : (l + s - (l*s));
		float p = (2.0f * l) - q;
		float Hk = h / 360.0f;
		float T[3];
		T[0] = Hk + 0.3333333f; // Tr   0.3333333f=1.0/3.0
		T[1] = Hk;              // Tb
		T[2] = Hk - 0.3333333f; // Tg
		for (int i = 0; i<3; i++)
		{
			if (T[i] < 0) T[i] += 1.0f;
			if (T[i] > 1) T[i] -= 1.0f;
			if ((T[i] * 6) < 1)
			{
				T[i] = p + ((q - p)*6.0f*T[i]);
			}
			else if ((T[i] * 2.0f) < 1) //(1.0/6.0)<=T[i] && T[i]<0.5
			{
				T[i] = q;
			}
			else if ((T[i] * 3.0f) < 2) // 0.5<=T[i] && T[i]<(2.0/3.0)
			{
				T[i] = p + (q - p) * ((2.0f / 3.0f) - T[i]) * 6.0f;
			}
			else T[i] = p;
		}
		R = T[0] * 255.0f;
		G = T[1] * 255.0f;
		B = T[2] * 255.0f;
	}

	rgb->red = (int)((R>255) ? 255 : ((R<0) ? 0 : R));
	rgb->green = (int)((G>255) ? 255 : ((G<0) ? 0 : G));
	rgb->blue = (int)((B>255) ? 255 : ((B<0) ? 0 : B));
}

// 在Windows系统下，HSL分量的范围是[0，240].参考“画笔”程序，可以看到RGB（红|绿|蓝）
// 和HSL（色调|饱和度|亮度）的联系。
// 下面的代码，把COLOR_HSL的分量值变为Windows的HSL分量，取值在[0，240]之间，需要：
// 下面为COLOR_HSL到Windows的HSL的转换：
// win_H = 240 * hsl.hue / 360.f;
// win_S = 240 * hsl.saturation / 100.f;
// win_L = 240 * hsl.luminance / 100.f;


void CColorConvert::ConvertColor(ColorTable & color_table)
{
	COLOR_RGB color_rgb;
	color_rgb.red = GetRValue(color_table.original_color);
	color_rgb.green = GetGValue(color_table.original_color);
	color_rgb.blue = GetBValue(color_table.original_color);

	COLOR_HSL color_hsl;
	RGBtoHSL(&color_rgb, &color_hsl);		//将颜色从RGB模式转换成HSL模式

	float luminance = color_hsl.luminance;		//保存原来的亮度
	color_hsl.luminance = luminance * 0.2f + 45;		//dark0
	HSLtoRGB(&color_hsl, &color_rgb);
	color_table.dark0 = RGB(color_rgb.red, color_rgb.green, color_rgb.blue);

	color_hsl.luminance = luminance * 0.2f + 35;		//dark1
	HSLtoRGB(&color_hsl, &color_rgb);
	color_table.dark1 = RGB(color_rgb.red, color_rgb.green, color_rgb.blue);

	color_hsl.luminance = luminance * 0.2f + 28;		//dark1_5
	HSLtoRGB(&color_hsl, &color_rgb);
	color_table.dark1_5 = RGB(color_rgb.red, color_rgb.green, color_rgb.blue);

	color_hsl.luminance = luminance * 0.3f + 20;		//dark2
	HSLtoRGB(&color_hsl, &color_rgb);
	color_table.dark2 = RGB(color_rgb.red, color_rgb.green, color_rgb.blue);

	color_hsl.luminance = luminance * 0.25f + 15;		//dark2_5
	HSLtoRGB(&color_hsl, &color_rgb);
	color_table.dark2_5 = RGB(color_rgb.red, color_rgb.green, color_rgb.blue);

	color_hsl.luminance = luminance * 0.2f + 10;		//dark3
	HSLtoRGB(&color_hsl, &color_rgb);
	color_table.dark3 = RGB(color_rgb.red, color_rgb.green, color_rgb.blue);

	color_hsl.luminance = luminance * 0.2f;		//dark4
	HSLtoRGB(&color_hsl, &color_rgb);
	color_table.dark4 = RGB(color_rgb.red, color_rgb.green, color_rgb.blue);

	color_hsl.luminance = luminance * 0.15f + 60;	//light1
	HSLtoRGB(&color_hsl, &color_rgb);
	color_table.light1 = RGB(color_rgb.red, color_rgb.green, color_rgb.blue);

	color_hsl.luminance = luminance * 0.1f + 70;	//light1_5
	HSLtoRGB(&color_hsl, &color_rgb);
	color_table.light1_5 = RGB(color_rgb.red, color_rgb.green, color_rgb.blue);

	color_hsl.luminance = luminance * 0.05f + 80;	//light2
	HSLtoRGB(&color_hsl, &color_rgb);
	color_table.light2 = RGB(color_rgb.red, color_rgb.green, color_rgb.blue);

	color_hsl.luminance = luminance * 0.05f + 85;	//light2_5
	HSLtoRGB(&color_hsl, &color_rgb);
	color_table.light2_5 = RGB(color_rgb.red, color_rgb.green, color_rgb.blue);

	color_hsl.luminance = luminance * 0.05f + 92;	//light3
	HSLtoRGB(&color_hsl, &color_rgb);
	color_table.light3 = RGB(color_rgb.red, color_rgb.green, color_rgb.blue);

	color_hsl.luminance = luminance * 0.05f + 95;	//light4
	HSLtoRGB(&color_hsl, &color_rgb);
	color_table.light4 = RGB(color_rgb.red, color_rgb.green, color_rgb.blue);

}

void CColorConvert::Desaturate(COLORREF & color)
{
	BYTE r = GetRValue(color);
	BYTE g = GetGValue(color);
	BYTE b = GetBValue(color);
	color = GRAY((r + g + b) / 3);
}

void CColorConvert::ReduceLuminance(COLORREF & color)
{
	COLOR_RGB color_rgb;
	color_rgb.red = GetRValue(color);
	color_rgb.green = GetGValue(color);
	color_rgb.blue = GetBValue(color);

	COLOR_HSL color_hsl;
	RGBtoHSL(&color_rgb, &color_hsl);		//将颜色从RGB模式转换成HSL模式

	float luminance = color_hsl.luminance;		//保存原来的亮度
	if (luminance < 24)						//当亮度小于一定值时降低颜色的亮度
	{
		color_hsl.luminance = luminance * 0.2f + 40;
		HSLtoRGB(&color_hsl, &color_rgb);
		color = RGB(color_rgb.red, color_rgb.green, color_rgb.blue);
	}
}

COLORREF CColorConvert::GetGradientColor(COLORREF color1, COLORREF color2, int percent)
{
    if (percent > 100)
        percent = 100;
    if (percent < 0)
        percent = 0;

    int R1 = GetRValue(color1);
    int G1 = GetGValue(color1);
    int B1 = GetBValue(color1);
    int R2 = GetRValue(color2);
    int G2 = GetGValue(color2);
    int B2 = GetBValue(color2);

    int R_result = (R1 + (R2 - R1)*percent / 100) % 256;
    int G_result = (G1 + (G2 - G1)*percent / 100) % 256;
    int B_result = (B1 + (B2 - B1)*percent / 100) % 256;

    return RGB(R_result, G_result, B_result);
}
