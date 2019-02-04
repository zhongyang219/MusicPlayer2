#pragma once
#include "MusicPlayer2.h"
#include "IPlayerUI.h"
#include "CPlayerUIHelper.h"

struct SLayoutData
{
	//const int control_bar_height = theApp.DPI(30);				//窗口上方的播放控制按钮部分的高度
	const int margin = theApp.DPI(4);							//边缘的余量
	const int width_threshold = theApp.DPI(600);				//界面从普通界面模式切换到窄界面模式时界面宽度的阈值
	const int info_height = theApp.DPI(198);					//窄界面模式时显示信息区域的高度
	const int info_height2 = theApp.DPI(143);					//普通界面模式时显示信息区域的高度
	const int path_edit_height = theApp.DPI(32);				//前路径Edit控件区域的高度
	const int search_edit_height = theApp.DPI(26);				//歌曲搜索框Edit控件区域的高度
	//const int progress_bar_height = theApp.DPI(20);				//(窄界面模式时)进度条区域的高度
	const CSize spectral_size{ theApp.DPI(120), theApp.DPI(90) };	//频谱分析区域的大小
};


class CPlayerUIBase : public IPlayerUI
{
public:

	struct UIData
	{
		CFont lyric_font;					//歌词字体
		CFont lyric_translate_font;			//歌词翻译的字体
		bool show_translate{ true };		//歌词是否显示翻译
		bool m_narrow_mode;					//窄界面模式
		bool show_playlist{true};

		int client_width;					//窗口客户区宽度
		int client_height;					//窗口客户区高度
		CImage default_background;			//默认的背景
	};

public:
	CPlayerUIBase(UIData& ui_data);
	~CPlayerUIBase();

	void SetToolTip(CToolTipCtrl* pToolTip);

public:
	virtual void Init(CDC* pDC) override;
	virtual void DrawInfo(bool reset = false) override;
	virtual void ClearInfo() override;

	virtual void LButtonDown(CPoint point) override;
	virtual void RButtonUp(CPoint point) override;
	virtual void MouseMove(CPoint point) override;
	virtual void LButtonUp(CPoint point) override;
	virtual void OnSizeRedraw(int cx, int cy) override;

	virtual CRect GetThumbnailClipArea() override = 0;
	void UpdateRepeatModeToolTip();
	void UpdateSongInfoToolTip();
	void UpdatePlayPauseButtonTip() override;

	virtual bool SetCursor() override;

protected:
	enum BtnKey		//标识按钮的类型
	{
		BTN_REPETEMODE,			//“循环模式”按钮
		BTN_VOLUME,				//音量按钮
		BTN_TRANSLATE,			//歌词翻译按钮
		BTN_SKIN,				//切换界面按钮
		BTN_EQ,					//音效设定按钮
		BTN_SETTING,			//设置按钮
		BTN_MINI,				//迷你模式按钮
		BTN_INFO,				//曲目信息按钮
		BTN_STOP,
		BTN_PREVIOUS,
		BTN_PLAY_PAUSE,
		BTN_NEXT,
		BTN_SHOW_PLAYLIST,
		BTN_SELECT_FOLDER,
		BTN_PROGRESS
	};

protected:
	void PreDrawInfo();
	void SetDrawRect();
	void DrawLyricTextMultiLine(CRect rect, bool midi_lyric);
	void DrawLyricTextSingleLine(CRect rect, bool midi_lyric);
	void DrawSongInfo(CRect rect, bool reset = false);
	void DrawToolBar(bool draw_background, CRect rect, bool draw_translate_button, UIData* pUIData = nullptr);
	void DrawVolumnAdjBtn(bool draw_background);
	void DrawControlBar(CRect rect, bool draw_background);
	void DrawProgressBar(CRect rect, bool draw_background);

	virtual void AddMouseToolTip(BtnKey btn, LPCTSTR str) = 0;		//为一个按钮添加鼠标提示
	virtual void UpdateMouseToolTip(BtnKey btn, LPCTSTR str) = 0;
	virtual void UpdateToolTipPosition() = 0;

	void AddToolTips();			//为每一个按钮添加鼠标提示（由于按钮的矩形区域只有在第一次绘图之后才能确定，所以此函数必须在第一次绘图之后调用）

	static CRect DrawAreaToClient(CRect rect, CRect draw_area);
	static CRect ClientAreaToDraw(CRect rect, CRect draw_area);

	bool DrawNarrowMode();			//是否使用窄界面模式绘图

private:
	void DrawLyricDoubleLine(CRect rect, LPCTSTR lyric, LPCTSTR next_lyric, int progress);
	void DrawUIButton(CRect rect, UIButton& btn, const IconRes& icon, bool draw_background);
	void DrawControlButton(CRect rect, UIButton& btn, const IconRes& icon, bool draw_background);
	void SetRepeatModeToolTipText();
	void SetSongInfoToolTipText();

protected:
	CDC* m_pDC;
	UIColors m_colors;
	CDrawCommon m_draw;		//用于绘制文本的对象
	std::shared_ptr<SLayoutData> m_pLayout{ nullptr };
	CFont m_font_time;

	CMenu m_popup_menu;			//歌词右键菜单
	CMenu m_main_popup_menu;

	CToolTipCtrl* m_tool_tip = nullptr;

	CString m_repeat_mode_tip;
	CString m_info_tip;

	int m_lyric_text_height;
	UIData& m_ui_data;

	//UI 数据
	CRect m_draw_rect;						//绘图区域
	CRect m_volume_up_rect, m_volume_down_rect;	//音量调整条增加和减少音量的矩形区域
	bool m_show_volume_adj{ false };		//显示音量调整按钮

	std::map<BtnKey, UIButton> m_buttons;

private:
	bool m_first_draw{ true };

};

