#pragma once
#include "MusicPlayer2.h"
#include "IPlayerUI.h"
#include "CPlayerUIHelper.h"

struct SLayoutData
{
	const int margin = theApp.DPI(4);							//边缘的余量
	const int width_threshold = theApp.DPI(600);				//界面从普通界面模式切换到窄界面模式时界面宽度的阈值
	const int info_height = theApp.DPI(216);					//窄界面模式时显示信息区域的高度
	const int path_edit_height = theApp.DPI(32);				//前路径Edit控件区域的高度
	const int search_edit_height = theApp.DPI(26);				//歌曲搜索框Edit控件区域的高度
	const CSize spectral_size{ theApp.DPI(120), theApp.DPI(90) };	//频谱分析区域的大小
};


class CPlayerUIBase : public IPlayerUI
{
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

	virtual CRect GetThumbnailClipArea() override;
	void UpdateRepeatModeToolTip();
	void UpdateSongInfoToolTip();
	void UpdatePlayPauseButtonTip() override;
	virtual void UpdateFullScreenTip() override;

	virtual bool SetCursor() override;
	virtual void MouseLeave() override;

	void ClearBtnRect();

	static bool IsMidiLyric();		//是否绘制MIDI音乐的歌词

	int Margin() const;
	int EdgeMargin(bool x = true) const;
	int WidthThreshold() const;
	int DrawAreaHeight() const;		//窄界面模式下显示播放列表时绘图区的高度

protected:
	enum BtnKey		//标识按钮的类型
	{
		BTN_REPETEMODE,			//“循环模式”按钮
		BTN_VOLUME,				//音量按钮
		BTN_VOLUME_UP,
		BTN_VOLUME_DOWN,
		BTN_TRANSLATE,			//歌词翻译按钮
		BTN_SKIN,				//切换界面按钮
		BTN_EQ,					//音效设定按钮
		BTN_SETTING,			//设置按钮
		BTN_MINI,				//迷你模式按钮
		BTN_INFO,				//曲目信息按钮
		BTN_FIND,				//查找歌曲按钮
		BTN_STOP,				//停止
		BTN_PREVIOUS,			//上一曲
		BTN_PLAY_PAUSE,			//播放/暂停
		BTN_NEXT,				//下一曲
		BTN_SHOW_PLAYLIST,		//显示/隐藏播放列表
		BTN_SELECT_FOLDER,		//选择文件夹
		BTN_PROGRESS,			//进度条
		BTN_COVER,
		BTN_FULL_SCREEN
	};

	struct DrawData
	{
		//CRect cover_rect;
		CRect lyric_rect;
		CRect thumbnail_rect;
	};

protected:
	void PreDrawInfo();
	void SetDrawRect();
	void DrawBackground();
	void DrawLryicCommon(CRect rect);
	void DrawSongInfo(CRect rect, bool reset = false);
	void DrawToolBar(CRect rect, bool draw_translate_button);
	void DrawVolumnAdjBtn();
	void DrawControlBar(CRect rect);
	void DrawProgressBar(CRect rect);
	void DrawTranslateButton(CRect rect);
	int DrawFullScreenIcon();			//绘制全屏显示图标。返回图标的大小
	void DrawCurrentTime();				//在右上角绘制当前系统时间

	void DrawUIButton(CRect rect, UIButton& btn, const IconRes& icon);
	void DrawControlButton(CRect rect, UIButton& btn, const IconRes& icon);

	virtual void AddMouseToolTip(BtnKey btn, LPCTSTR str) = 0;		//为一个按钮添加鼠标提示
	virtual void UpdateMouseToolTip(BtnKey btn, LPCTSTR str) = 0;
	virtual void UpdateToolTipPosition() = 0;

	void AddToolTips();			//为每一个按钮添加鼠标提示（由于按钮的矩形区域只有在第一次绘图之后才能确定，所以此函数必须在第一次绘图之后调用）

	static CRect DrawAreaToClient(CRect rect, CRect draw_area);
	static CRect ClientAreaToDraw(CRect rect, CRect draw_area);

	bool IsDrawNarrowMode();			//是否使用窄界面模式绘图
	bool IsDrawBackgroundAlpha() const;	//是否需要绘制透明背景

	int DPI(int pixel);
	int DPI(double pixel);

private:
	void DrawLyricTextMultiLine(CRect rect);
	void DrawLyricTextSingleLine(CRect rect);
	void DrawLyricDoubleLine(CRect rect, LPCTSTR lyric, LPCTSTR next_lyric, int progress);
	void SetRepeatModeToolTipText();
	void SetSongInfoToolTipText();
	void SetCoverToolTipText();

protected:
	CDC* m_pDC;
	UIColors m_colors;
	CDrawCommon m_draw;		//用于绘制文本的对象
	SLayoutData m_layout;
	//CFont m_font_time;
	DrawData m_draw_data;

	CMenu m_popup_menu;			//歌词右键菜单
	CMenu m_main_popup_menu;

	CToolTipCtrl* m_tool_tip = nullptr;

	CString m_repeat_mode_tip;
	CString m_info_tip;
	CString m_cover_tip;

	int m_lyric_text_height;
	UIData& m_ui_data;

	//UI 数据
	CRect m_draw_rect;						//绘图区域
	bool m_show_volume_adj{ false };		//显示音量调整按钮

	std::map<BtnKey, UIButton> m_buttons;

	const int m_progress_on_top_threshold = theApp.DPI(350);		//当控制条的宽度小于此值，将进度条显示在播放控制按钮的上方

private:
	bool m_first_draw{ true };

};

