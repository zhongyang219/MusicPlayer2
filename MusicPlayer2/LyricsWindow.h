// ========================================================
// 文件:LyricsWindow.h
// 类名:CLyricsWindow
// 功能:歌词显示窗口
// 描述:使用GDIPlus绘制歌词
// 作者:邓学彬(泪闯天涯)
// ========================================================
// 接口说明
// Create				创建歌词窗口
// UpdateLyrics			更新歌词(歌词文本,高亮进度百分比)
// Draw					重画歌词窗口
// SetLyricsColor		设置歌词颜色
// SetLyricsBorder		设置歌词边框
// SetHighlightColor	设置高亮歌词颜色
// SetHighlightBorder	设置高亮歌词边框
// SetLyricsShadow		设置歌词阴影
// SetLyricsFont		设置歌词字体
// ========================================================

/*
说明：此类的原作者：邓学彬，地址：https://blog.csdn.net/CometNet/article/details/37508613
在原有的基础上有修改
*/

#pragma once

#include <gdiplus.h>
#pragma comment(lib,"GdiPlus.lib")
#include "DrawCommon.h"

//歌词渐变模式
enum LyricsGradientMode
{
	LyricsGradientMode_None		=0, //无渐变
	LyricsGradientMode_Two		=1, //两色渐变
	LyricsGradientMode_Three	=2 //三色渐变
};

// CLyricsWindow

class CLyricsWindow : public CWnd
{
	DECLARE_DYNAMIC(CLyricsWindow)

public:
	CLyricsWindow();
	virtual ~CLyricsWindow();

public:
	//创建窗口
	BOOL Create(int nHeight = -1);
	BOOL Create(LPCTSTR lpszClassName);
	BOOL Create(LPCTSTR lpszClassName,int nWidth,int nHeight);
public:
	//更新歌词(歌词文本,高亮进度百分比)
	void UpdateLyrics(LPCTSTR lpszLyrics,int nHighlight);
	//更新高亮进度(高亮进度百分比)
	void UpdateLyrics(int nHighlight);
	//更新歌词翻译文本
	void UpdateLyricTranslate(LPCTSTR lpszLyricTranslate);
	//重画歌词窗口
	void Draw();
	//设置歌词颜色
	void SetLyricsColor(Gdiplus::Color TextColor1);
	void SetLyricsColor(Gdiplus::Color TextColor1,Gdiplus::Color TextColor2,LyricsGradientMode TextGradientMode);
	//设置歌词边框
	void SetLyricsBorder(Gdiplus::Color BorderColor, Gdiplus::REAL BorderWidth = 1.0f);
	//设置高亮歌词颜色
	void SetHighlightColor(Gdiplus::Color TextColor1);
	void SetHighlightColor(Gdiplus::Color TextColor1,Gdiplus::Color TextColor2,LyricsGradientMode TextGradientMode);
	//设置高亮歌词边框
	void SetHighlightBorder(Gdiplus::Color BorderColor, Gdiplus::REAL BorderWidth = 1.0f);
	//设置歌词阴影
	void SetLyricsShadow(Gdiplus::Color ShadowColor,int nShadowOffset=2);
	//设置歌词字体
	void SetLyricsFont(const WCHAR * familyName, Gdiplus::REAL emSize,INT style= Gdiplus::FontStyleRegular, Gdiplus::Unit unit= Gdiplus::UnitPixel);
	//设置是否双行显示
	void SetLyricDoubleLine(bool doubleLine);
	void SetNextLyric(LPCTSTR lpszNextLyric);
    void SetShowTranslate(bool showTranslate);
    //设置不透明度
    void SetAlpha(int alpha);
    //获取当前歌词文本
    const CString& GetLyricStr() const;
    void SetLyricChangeFlag(bool bFlag);
	//设置对齐方式
	void SetAlignment(Alignment alignment);
    //设置歌词卡拉OK样式显示
    void SetLyricKaraokeDisplay(bool karaoke_disp);
private:
	//绘制高亮歌词
	void DrawHighlightLyrics(Gdiplus::Graphics* pGraphics,Gdiplus::GraphicsPath* pPath, Gdiplus::RectF& dstRect);
	//创建渐变画刷
	Gdiplus::Brush* CreateGradientBrush(LyricsGradientMode TextGradientMode,Gdiplus::Color& Color1,Gdiplus::Color& Color2, Gdiplus::RectF& dstRect);
	//注册窗口类
	BOOL RegisterWndClass(LPCTSTR lpszClassName);

protected:
	void DrawLyricText(Gdiplus::Graphics* pGraphics, LPCTSTR strText, Gdiplus::RectF rect, bool bDrawHighlight, bool bDrawTranslate = false);
	//绘制歌词
	void DrawLyrics(Gdiplus::Graphics* pGraphics);
    void DrawLyricsDoubleLine(Gdiplus::Graphics* pGraphics);
    //在绘制歌词前绘制的内容
    virtual void PreDrawLyric(Gdiplus::Graphics* pGraphics) { }
    //在绘制歌词后绘制的内容
    virtual void AfterDrawLyric(Gdiplus::Graphics* pGraphics) { }

protected:
	DECLARE_MESSAGE_MAP()
private:
	HDC m_hCacheDC;//缓存DC
	int m_nWidth;
	int m_nHeight;
	CString m_lpszLyrics;//Unicode格式的歌词
	int m_nHighlight;//高亮歌词的百分比 0--1000
	Gdiplus::Color m_TextColor1;//普通歌词颜色,ARGB颜色
	Gdiplus::Color m_TextColor2;//普通歌词颜色,ARGB颜色
	LyricsGradientMode m_TextGradientMode;//普通歌词渐变模式
	Gdiplus::Pen* m_pTextPen;//普通歌词边框画笔
	Gdiplus::Color m_HighlightColor1;//高亮歌词颜色,ARGB颜色
	Gdiplus::Color m_HighlightColor2;//高亮歌词颜色,ARGB颜色
	LyricsGradientMode m_HighlightGradientMode;//高亮歌词渐变模式
	Gdiplus::Pen*m_pHighlightPen;//高亮歌词边框画笔
	Gdiplus::Brush* m_pShadowBrush;//阴影画刷,GDIPlus画刷
	int m_nShadowOffset;//阴影偏移
	Gdiplus::Font* m_pFont;//GDIPlus字体
	int m_FontStyle;
	Gdiplus::REAL m_FontSize;
	Gdiplus::FontFamily* m_pFontFamily;
	Gdiplus::StringFormat* m_pTextFormat;

protected:
	bool m_bDoubleLine = false;		//歌词双行显示
    bool m_bShowTranslate = false;  //显示歌词翻译
	CString m_strTranslate;			//歌词翻译
	CString m_strNextLyric;			//下一句歌词
    int m_alpha = 255;              //不透明度
    bool m_lyricChangeFlag = false; //歌词发生改变标志
    CSize m_frameSize{};
    CRect m_rcWindow;
    int m_toobar_height = 0;
	Alignment m_alignment{ Alignment::CENTER };	//对齐方式
    bool m_lyric_karaoke_disp = true;   //歌词卡拉OK样式显示

protected:
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

};
