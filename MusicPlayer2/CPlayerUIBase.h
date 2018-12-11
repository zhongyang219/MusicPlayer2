#pragma once
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


class CPlayerUIBase
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
		CFont lyric_font;					//歌词字体
		CFont lyric_translate_font;			//歌词翻译的字体
		bool show_translate{ true };		//歌词是否显示翻译

		int client_width;					//窗口客户区宽度
		int client_height;					//窗口客户区高度
		CImage default_background;			//默认的背景
	};

public:
	CPlayerUIBase();
	~CPlayerUIBase();

	void SetToolTip(CToolTipCtrl* pToolTip);

public:
	virtual void Init(CDC* pDC) = 0;
	virtual void DrawInfo(bool narrow_mode, bool reset = false);

	virtual void RButtonUp(CPoint point, bool narrow_mode);
	virtual void MouseMove(CPoint point);
	virtual void LButtonUp(CPoint point);
	virtual void OnSizeRedraw(int cx, int cy, bool narrow_mode) = 0;

	virtual CRect GetThumbnailClipArea(bool narrow_mode) = 0;

protected:
	void DrawLyricTextMultiLine(CRect rect, CFont* font, CFont* translate_font, bool show_translate, bool midi_lyric);
	void DrawLyricTextSingleLine(CRect rect, CFont* font, CFont* translate_font, bool show_translate, bool midi_lyric);
	//void DrawControlBar(bool draw_background, CRect rect, bool draw_translate_button);

	void AddMouseToolTip(const UIButton& btn, LPCTSTR str, bool* static_bool);		//为一个按钮添加鼠标提示，只能在响应“WM_MOUSEMOVE”时调用

	static CRect DrawAreaToClient(CRect rect, CRect draw_area);

private:
	void DrawLyricDoubleLine(CRect rect, CFont* font, LPCTSTR lyric, LPCTSTR next_lyric, int progress);

protected:
	CDC* m_pDC;
	UIColors m_colors;
	CDrawCommon m_draw;		//用于绘制文本的对象

	CMenu m_popup_menu;			//歌词右键菜单
	CMenu m_main_popup_menu;

	CToolTipCtrl* m_tool_tip = nullptr;

	//UI 数据
	UIButton repetemode_btn;				//“循环模式”按钮
	UIButton volume_btn;				//音量按钮
	CRect volume_up_rect, volume_down_rect;	//音量调整条增加和减少音量的矩形区域
	bool show_volume_adj{ false };		//显示音量调整按钮
	UIButton translate_btn;				//歌词翻译按钮
	UIButton skin_btn;

};

