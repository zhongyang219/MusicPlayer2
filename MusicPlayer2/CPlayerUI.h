#pragma once
#include "DrawCommon.h"
#include "MusicPlayer2.h"

struct SLayoutData
{
	const int control_bar_height = theApp.DPI(30);				//窗口上方的播放控制按钮部分的高度
	const int margin = theApp.DPI(4);							//边缘的余量
	const int width_threshold = theApp.DPI(600);				//界面从普通界面模式切换到窄界面模式时界面宽度的阈值
	const int info_height = theApp.DPI(166);					//窄界面模式时显示信息区域的高度
	const int info_height2 = theApp.DPI(143);					//普通界面模式时显示信息区域的高度
	const int path_edit_height = theApp.DPI(32);				//前路径Edit控件区域的高度
	const int search_edit_height = theApp.DPI(26);				//歌曲搜索框Edit控件区域的高度
	const int progress_bar_height = theApp.DPI(20);				//(窄界面模式时)进度条区域的高度
	const CSize spectral_size{ theApp.DPI(120), theApp.DPI(90) };	//频谱分析区域的大小
};

class CPlayerUI
{
public:
	struct UIColors		//界面颜色
	{
		COLORREF color_text;				//文本颜色
		COLORREF color_text_lable;			//标签文本的颜色
		COLORREF color_text_2;				//歌词未播放文本的颜色
		COLORREF color_text_heighlight;		//鼠标指向时文本的颜色
		COLORREF color_back;				//背景颜色
		COLORREF color_lyric_back;			//歌词界面背景颜色
		COLORREF color_control_bar_back;	//控制条背景颜色
		COLORREF color_spectrum;			//频谱分析柱形的颜色
		COLORREF color_spectrum_cover;		//有专辑封面时的频谱分析柱形的颜色
		COLORREF color_spectrum_back;		//频谱分析的背景颜色
		COLORREF color_button_back;			//歌词翻译按钮的背景色
		int background_transparency;		//背景不透明度0~100
	};

	struct UIButton		//界面中绘制的按钮
	{
		CRect rect;				//按钮的矩形区域
		bool hover{ false };	//鼠标是否指向按钮
		bool enable{ true };	//按钮是否启用
	};

	struct UIData
	{
		CRect draw_rect;					//绘图区域

		CRect cover_rect;					//显示专辑封面的矩形区域（以绘图区域左上角为原点）
		CRect repetemode_rect;				//显示“循环模式”的矩形区域
		bool repetemode_hover{ false };		//鼠标指向了“循环模式”的矩形区域
		UIButton volume_btn;				//音量按钮
		CRect volume_up_rect, volume_down_rect;	//音量调整条增加和减少音量的矩形区域
		bool show_volume_adj{ false };		//显示音量调整按钮
		UIButton translate_btn;				//歌词翻译按钮

		CFont lyric_font;					//歌词字体
		CFont lyric_translate_font;			//歌词翻译的字体

		bool show_translate{ true };		//歌词是否显示翻译

		int client_width;					//窗口客户区宽度
		int client_height;					//窗口客户区高度
		CImage default_background;			//默认的背景

	};

public:
	CPlayerUI(UIData& ui_data);
	~CPlayerUI();

	void Init(CDC* pDC);
	void DrawInfo(bool narrow_mode, bool reset = false);		//绘制信息

protected:
	void DrawLyricsSingleLine(CRect lyric_rect);			//绘制歌词（窄界面模式下单行显示），参数为显示歌词的矩形区域
	void DrawLyricsMulityLine(CRect lyric_rect, CDC* pDC);			//绘制歌词（普通模式下多行显示），参数为显示歌词的矩形区域


protected:
	CDC* m_pDC;
	UIData& m_ui_data;
	UIColors m_colors;

	CDrawCommon m_draw;		//用于绘制文本的对象

	std::shared_ptr<SLayoutData> m_pLayout{ nullptr };
};

