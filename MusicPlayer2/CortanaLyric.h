#pragma once
#include "ColorConvert.h"
#include "MusicPlayer2.h"
#include "CUIDrawer.h"

#define LIGHT_MODE_SEARCH_BOX_BACKGROUND_COLOR RGB(240, 240, 240)
#define SEARCH_BOX_DEFAULT_TRANSPARENT_COLOR RGB(255, 0, 255)
class CCortanaLyric
{
public:
    CCortanaLyric();
    ~CCortanaLyric();

    void Init();	//初始化，获取Cortana句柄、矩形区域等
    static void InitFont();
    void SetEnable(bool enable);

    void DrawInfo();

    void ResetCortanaText();		//将Cortana搜索框的文本恢复为默认
    void AlbumCoverEnable(bool enable);
    void SetBeatAmp(int beat_amp);
    void SetUIColors();
    void SetDarkMode(bool dark_mode);		//设置搜索框是否为黑色模式
    void ApplySearchBoxTransparentChanged();

private:
    struct CortanaUIColors		//界面颜色
    {
        COLORREF text_color;		//已播放歌词的颜色
        COLORREF text_color2;		//未播放歌词的颜色
        COLORREF info_text_color;	//歌曲信息文本的颜色
        COLORREF back_color;		//背景色
        //COLORREF sprctrum_color;
        bool dark;			//是否使用深色作为背景色绘制
    };

private:
	void SetCortanaBarOpaque(bool opaque);

    //在Cortana搜索框上绘制文本
    //str:	要绘制的字符串
    //align:	对齐方式
    void DrawCortanaTextSimple(LPCTSTR str, Alignment align);

    /* 在Cortana搜索框上绘制滚动显示的文本
    参数：
    	str:	要绘制的字符串
    	reset:	如果为true则重置滚动位置
    	scroll_pixel:	文本滚动一次移动的像素值（这个值越大则滚动越快）
    */
    void DrawCortanaText(LPCTSTR str, bool reset, double scroll_pixel);

    /* 在Cortana搜索框上绘制动态显示歌词的文本
    参数：
    	str:	要绘制的字符串
    	progress:	当前歌词的进度（范围为0~1000）
    */
    void DrawCortanaText(LPCTSTR str, int progress);

    void DrawAlbumCover(const CImage& album_cover);

    void DrawSpectrum();

    CRect TextRect() const;
    CRect CoverRect() const;

    double GetScrollTextPixel() const;

private:
    bool m_enable;
	HWND m_hCortanaBar{};		////Cortana栏的句柄
    HWND m_cortana_hwnd{};		//Cortana的句柄
    HWND m_hCortanaStatic;
    wstring m_cortana_default_text;	//Cortana搜索框中原来的文本
    CUIDrawer m_draw{ m_lyric_colors };		//用于在Cortana搜索框中绘图的对象
    CWnd* m_cortana_wnd{};		//Cortana搜索框的指针
    CFont m_default_font;		//在Cortana搜索框中原来的字体
    CRect m_cortana_rect;		//Cortana搜索框框的矩形区域
    int m_cover_width;
    CDC* m_pDC{};				//在Cortana搜索框中绘图的DC

    bool m_dark_mode{ true };			//Cortana搜索框是否处于深色模式

    //CPoint m_check_dark_point{};			//用于判断Cortana搜索框是否为深色模式的点的位置
    //COLORREF m_back_color;
    const COLORREF m_border_color{ GRAY(180) };		//浅色模式时边框的颜色

    CortanaUIColors m_colors;
    UIColors m_lyric_colors;

    bool m_show_album_cover{ false };			//是否在Cortana图标处显示专辑封面
    int m_beat_amp{0};						//小娜图标跳动的幅值，取值为0~1000

    bool m_cortana_opaque{ false };           //是否设置了搜索框不透明

    static CCriticalSection m_critical;

public:
    //bool m_cortana_disabled;
};

