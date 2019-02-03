#pragma once
#include"Player.h"
#include "afxwin.h"
#include "PropertyDlg.h"
#include "afxcmn.h"
#include "PlayListCtrl.h"
#include "ColorConvert.h"
#include"ProgressStatic.h"
#include "DrawCommon.h"
#include "CMiniModeUI.h"

// CMiniModeDlg 对话框

class CMiniModeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMiniModeDlg)

public:
	CMiniModeDlg(int& item_selected, vector<int>& items_selected, CMenu& popup_menu, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMiniModeDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MINI_DIALOG };
#endif

	void UpdatePlayPauseButton();
	void ShowPlaylist();
	void SetPlayListColor();
	void SetDefaultBackGround(CImage* pImage);
	void SetDisplayFormat(DisplayFormat* pDisplayFormat);

	void SetVolume(bool up);	//

protected:

	int m_position_x;
	int m_position_y;

	int m_screen_width;
	int m_screen_height;
	bool m_show_playlist{ false };		//是否显示播放列表

	int& m_item_selected;		//播放列表中鼠标选中的项目，引用MusicPlayerDlg类中的同名变量，当迷你窗口中播放列表选中的项目变化时，同步到主窗口中选中的项目
	vector<int>& m_items_selected;

	CToolTipCtrl m_Mytip;
	CMenu m_menu;
	CMenu& m_popup_menu;		//播放列表右键菜单

	CMiniModeUI::SMiniModeUIData m_ui_data;
	CMiniModeUI m_ui{ m_ui_data, this };

	CDC* m_pDC;

	CPlayListCtrl m_playlist_ctrl{ CPlayer::GetInstance().GetPlayList() };

	bool m_first_start{ true };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	void SaveConfig() const;
	void LoadConfig();

	void CheckWindowPos();
	void UpdateSongTipInfo();
	void SetTitle();

	DECLARE_MESSAGE_MAP()


protected:

	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDestroy();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMiniModeExit();
	afx_msg void OnInitMenu(CMenu* pMenu);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnNMDblclkList2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickList2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPaint();
public:
	//afx_msg void OnStnClickedMiniProgressStatic();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnShowPlayList();
	afx_msg void OnMouseLeave();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMiniMideMinimize();
};
