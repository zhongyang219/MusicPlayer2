#pragma once
#include "IPlayerUI.h"
#include "MusicPlayer2.h"


class CMiniModeUI :
	public IPlayerUI
{
public:
	struct UIColors		//界面颜色
	{
		COLORREF color_text;				//文本颜色
		COLORREF color_text_lable;			//标签文本的颜色
		COLORREF color_text_2;				//歌词未播放文本的颜色
		//COLORREF color_text_heighlight;		//鼠标指向时文本的颜色
		COLORREF color_back;				//背景颜色
		COLORREF color_lyric_back;			//歌词界面背景颜色
		//COLORREF color_control_bar_back;	//控制条背景颜色
		COLORREF color_spectrum;			//频谱分析柱形的颜色
		//COLORREF color_spectrum_cover;		//有专辑封面时的频谱分析柱形的颜色
		COLORREF color_spectrum_back;		//频谱分析的背景颜色
		COLORREF color_button_back;			//歌词翻译按钮的背景色
		int background_transparency;		//背景不透明度0~100
	};

	struct SMiniModeUIData
	{
		int widnow_width;
		int window_height;
		int window_height2;
		int margin;
		bool m_show_volume{ false };	//用于指示是否在显示时间的控件显示音量，当滚动鼠标滚轮时的1.5秒内，此变量的值为true

		CImage* pDefaultBackground = nullptr;			//默认的背景
		DisplayFormat* pDisplayFormat = nullptr;

		void Init()
		{
			widnow_width = theApp.DPI(304);
			window_height = theApp.DPI(44);
			window_height2 = theApp.DPI(336);
			margin = theApp.DPI(3);
		}
	};

public:
	CMiniModeUI(SMiniModeUIData& ui_data, CWnd* pMiniModeWnd);
	~CMiniModeUI();

	void SetToolTip(CToolTipCtrl* pToolTip);
	bool PointInControlArea(CPoint point) const;		//判断一个点的位置是否在控件区域

	virtual void Init(CDC* pDC) override;
	virtual void DrawInfo(bool reset = false) override;

	virtual void RButtonUp(CPoint point) override;
	virtual void MouseMove(CPoint point) override;
	virtual void LButtonUp(CPoint point) override;
	void MouseLeave();
	virtual void OnSizeRedraw(int cx, int cy) override;
	bool SetCursor();

	virtual CRect GetThumbnailClipArea() override;

	void UpdateSongInfoTip(LPCTSTR str_tip);

private:
	enum BtnKey		//标识按钮的类型
	{
		BTN_PREVIOUS,
		BTN_PLAY_PAUSE,
		BTN_NEXT,
		BTN_PLAYLIST,
		BTN_RETURN,
		BTN_CLOSE,
		BTN_COVER,
		BTN_PROGRESS
	};

private:
	void DrawUIButton(CRect rect, UIButton& btn, HICON icon, bool draw_background);
	void DrawTextButton(CRect rect, UIButton& btn, LPCTSTR text, bool draw_background);
	void AddMouseToolTip(BtnKey btn, LPCTSTR str);		//为一个按钮添加鼠标提示
	void UpdateMouseToolTip(BtnKey btn, LPCTSTR str);

	void AddToolTips();

private:
	SMiniModeUIData& m_ui_data;
	CWnd* m_pMiniModeWnd = nullptr;
	CDC* m_pDC;
	CDrawCommon m_draw;
	UIColors m_colors;
	CToolTipCtrl* m_tool_tip = nullptr;

	bool m_first_draw{ true };

	std::map<BtnKey, UIButton> m_buttons;
};

