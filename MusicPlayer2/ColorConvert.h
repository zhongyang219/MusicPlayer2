#pragma once
//#define min3v(v1, v2, v3)   ((v1)>(v2)? ((v2)>(v3)?(v3):(v2)):((v1)>(v3)?(v3):(v2)))
//#define max3v(v1, v2, v3)   ((v1)<(v2)? ((v2)<(v3)?(v3):(v2)):((v1)<(v3)?(v3):(v1)))

class CColorConvert
{
public:

	struct COLOR_RGB
	{
		int  red;              // [0,255]
		int  green;            // [0,255]
		int  blue;             // [0,255]
	};

	struct COLOR_HSL
	{
		float hue;              // [0,360]
		float saturation;       // [0,100]
		float luminance;        // [0,100]
	};

	CColorConvert();
	~CColorConvert();

	template<class T>
	static T min3v(T v1, T v2, T v3)
	{
		T min = v1;
		if (v2 < min)
			min = v2;
		if (v3 < min)
			min = v3;
		return min;
	}

	template<class T>
	static T max3v(T v1, T v2, T v3)
	{
		T max = v1;
		if (v2 > max)
			max = v2;
		if (v3 > max)
			max = v3;
		return max;
	}

	// Converts RGB to HSL
	static void RGBtoHSL(const COLOR_RGB *rgb, COLOR_HSL *hsl);

	// Converts HSL to RGB
	static void HSLtoRGB(const COLOR_HSL *hsl, COLOR_RGB *rgb);

	//此函数用于根据主题色计算播放列表中当前播放项目的文本颜色
	//在保持色相和饱和度不变的情况下减少颜色的亮度值，亮度变换为一次函数y=0.5x+12
	static COLORREF ConvertToItemColor(COLORREF color);

	//此函数用于根据主题色计算播放列表中当前播放项目的背景颜色
	//在保持色相和饱和度不变的情况下增加颜色的亮度值，亮度变换为一次函数y=0.1x+90
	static COLORREF ConvertToBackColor(COLORREF color);

	//此函数用于根据主题色计算迷你模式中卡拉OK显示时歌词的颜色
	//在保持色相和饱和度不变的情况下增加颜色的亮度值，亮度变换为一次函数y=0.4x+50
	static COLORREF ConvertToLightColor(COLORREF color);
};

