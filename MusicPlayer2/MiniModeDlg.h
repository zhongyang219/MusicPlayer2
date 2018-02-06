#pragma once
#include"Player.h"
#include"StaticEx.h"
#include "afxwin.h"
#include "PropertyDlg.h"
#include "afxcmn.h"
#include "PlayListCtrl.h"
#include "ColorConvert.h"
#include"ProgressStatic.h"
#include "DrawCommon.h"

// CMiniModeDlg 对话框

class CMiniModeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMiniModeDlg)

public:
	CMiniModeDlg(int& item_selected, CMenu& popup_menu, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMiniModeDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MINI_DIALOG };
#endif

	DisplayFormat m_display_format{};		//播放列表中项目的显示样式

	void UpdatePlayPauseButton();
	void ShowPlaylist();
	void SetPlayListColor();
	void RePaint();			//重新绘制界面的元素

protected:
	//CPlayer& m_player;

	//int m_dpi{ 96 };
	HICON m_hPreviousIcon_s;
	HICON m_hNextIcon_s;
	HICON m_hPlayIcon_s;
	HICON m_hPauseIcon_s;
	HBITMAP m_back_img;

	ColorTable m_theme_color{};

	int m_position_x;
	int m_position_y;

	int m_screen_width;
	int m_screen_height;

	CRect m_rect;		//显示播放列表时的界面大小
	CRect m_rect_s;		//不显示播放列表时的界面大小

	bool m_show_playlist{ false };		//是否显示播放列表
	bool m_dark_mode{ false };
	bool m_follow_main_color{ true };
	bool m_first_start{ true };

	int& m_item_selected;		//播放列表中鼠标选中的项目，引用MusicPlayerDlg类中的同名变量，当迷你窗口中播放列表选中的项目变化时，同步到主窗口中选中的项目

	CToolTipCtrl m_Mytip;
	CMenu m_menu;
	CMenu& m_popup_menu;		//播放列表右键菜单

	CString m_song_tip_info;

	//
#define ROW 4				//频谱柱形的数量
	CRect m_spectral_client_rect;		//频谱的矩形区域
	CRect m_spectral_rect[ROW];		//每个柱形的矩形区域
	CDC* m_spectrum_pDC;		//频谱分析区域static控件的CDC
	CDrawCommon m_draw;
	CRect m_album_rect;			//专辑封面的矩形区域

	CStaticEx m_lyric_static;
	CStaticEx m_time_static;
	CMFCButton m_previous_button;
	CMFCButton m_play_pause_button;
	CMFCButton m_next_button;
	CMFCButton m_show_list_button;
	CMFCButton m_return_button;
	CMFCButton m_exit_button;
	CStatic m_spectral_static;
	CPlayListCtrl m_playlist_ctrl{ theApp.m_player.GetPlayList() };
public:
	CProgressStatic m_progress_bar;

protected:
	int m_last_lyric_index{};		//上一句歌词的序号，用于判断歌词是否改变
	int m_last_index{};			//上一个播放曲目的序号，用于判断播放曲目是否改变
	bool m_show_volume{ false };	//用于指示是否在显示时间的控件显示音量，当滚动鼠标滚轮时的1.5秒内，此变量的值为true


	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	void SaveConfig() const;
	void LoadConfig();

	void CheckWindowPos();
	void UpdateSongTipInfo();

	void DrawSpectral();		//绘制频谱分析
	void DrawBorder();		//绘制边框
	void ShowVolume(int volume);
	void ShowInfo(bool force_refresh);

	void ColorChanged();		//当主题颜色改变时调用
	void SetVolume(bool up);	//

	DECLARE_MESSAGE_MAP()


protected:

	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	virtual BOOL PreTranslateMessage(MSG* pMsg);
//	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMiniModeExit();
//	afx_msg void OnSongInfo();
	afx_msg void OnInitMenu(CMenu* pMenu);
	afx_msg void OnSetTextColor();
	afx_msg void OnBnClickedMPlayPause();
	afx_msg void OnBnClickedMPrevious();
	afx_msg void OnBnClickedMNext();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnNMDblclkList2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickList2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedShowListButton();
	afx_msg void OnPaint();
	afx_msg void OnDarkMode();
	afx_msg void OnFollowMainColor();
public:
	afx_msg void OnStnClickedMiniProgressStatic();
};
