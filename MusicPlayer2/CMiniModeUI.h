#pragma once
#include "IPlayerUI.h"
#include "MusicPlayer2.h"
#include "CPlayerUIHelper.h"


class CMiniModeUI /*:	public IPlayerUI*/
{
public:

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

	virtual void Init(CDC* pDC);
	virtual void DrawInfo(bool reset = false);

	virtual void LButtonDown(CPoint point);
	virtual void RButtonUp(CPoint point);
	virtual void MouseMove(CPoint point);
	virtual void LButtonUp(CPoint point);
	void MouseLeave();
	virtual void OnSizeRedraw(int cx, int cy);
	bool SetCursor();

	virtual CRect GetThumbnailClipArea();

	void UpdateSongInfoTip(LPCTSTR str_tip);
	void UpdatePlayPauseButtonTip();

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
	void DrawUIButton(CRect rect, IPlayerUI::UIButton& btn, HICON icon, bool draw_background);
	void DrawTextButton(CRect rect, IPlayerUI::UIButton& btn, LPCTSTR text, bool draw_background);
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

	std::map<BtnKey, IPlayerUI::UIButton> m_buttons;
};

