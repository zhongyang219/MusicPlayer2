#pragma once
#include "DrawCommon.h"
#include "ColorConvert.h"
#include "MusicPlayer2.h"

class CCortanaLyric
{
public:
	CCortanaLyric();
	~CCortanaLyric();

	void Init();	//初始化，获取Cortana句柄、矩形区域等
	void SetEnable(bool enable);
	void SetColors(ColorTable colors);
	void SetCortanaColor(int color);		//设置Cortana搜索框的背景颜色（0：跟随系统，1：黑色，2：白色）

	/* 在Cortana搜索框上绘制滚动显示的文本
	参数：
		str:	要绘制的字符串
		reset:	如果为true则重置滚动位置
		scroll_pixel:	文本滚动一次移动的像素值（这个值越大则滚动越快）
	*/
	void DrawCortanaText(LPCTSTR str, bool reset, int scroll_pixel);
	
	/* 在Cortana搜索框上绘制动态显示歌词的文本
	参数：
		str:	要绘制的字符串
		progress:	当前歌词的进度（范围为0~1000）
	*/
	void DrawCortanaText(LPCTSTR str, int progress);

	//在Cortana搜索框内双行显示歌词
	void DrawLyricDoubleLine(LPCTSTR lyric, LPCTSTR next_lyric, int progress);

	void DrawAlbumCover(const CImage& album_cover);

	void ResetCortanaText();		//将Cortana搜索框的文本恢复为默认
	void CheckDarkMode();			//检查Cortana搜索框是否为黑色模式
	void AlbumCoverEnable(bool enable);
	void SetSpectrum(int spectrum);

private:
	bool m_enable;
	HWND m_cortana_hwnd{};		//Cortana的句柄
	HWND m_hCortanaStatic;
	wstring m_cortana_default_text;	//Cortana搜索框中原来的文本
	CDrawCommon m_cortana_draw;		//用于在Cortana搜索框中绘图的对象
	CWnd* m_cortana_wnd{};		//Cortana搜索框的指针
	CFont m_cortana_font;		//在Cortana搜索框中显示歌词的字体
	CFont m_font_double_line;		//双行显示时歌词的字体
	CRect m_cortana_rect;		//Cortana搜索框框的矩形区域
	CRect m_icon_rect;			//Cortana图标处的矩形区域
	int m_cortana_left_space;		//Cortana搜索框中显示文本距搜索框左侧的距离
	CDC* m_cortana_pDC{};				//在Cortana搜索框中绘图的DC

	bool m_dark_mode{ true };			//Cortana搜索框是否处于深色模式
	ColorTable m_colors;

	CPoint m_check_dark_point{};			//用于判断Cortana搜索框是否为深色模式的点的位置
	COLORREF m_back_color;
	const COLORREF m_border_color{ GRAY(180) };		//浅色模式时边框的颜色

	bool m_show_album_cover{ false };			//是否在Cortana图标处显示专辑封面
	int m_cortana_color;		//Cortana搜索框的背景颜色（0：跟随系统，1：黑色，2：白色）

	int m_spectrum{0};		//频谱的幅值，取值为0~1000

public:
	bool m_cortana_disabled;
};

