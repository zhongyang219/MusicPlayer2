#pragma once
#include "PlayListCtrl.h"
#include "StaticEx.h"
#include "CPlayerUIBase.h"


// CFloatPlaylistDlg 对话框

class CFloatPlaylistDlg : public CDialog
{
	DECLARE_DYNAMIC(CFloatPlaylistDlg)

public:
	CFloatPlaylistDlg(int& item_selected, vector<int>& items_selected, CMenu& popup_menu, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CFloatPlaylistDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MUSICPLAYER2_DIALOG };
#endif

	void RefreshData();			//刷新数据
	void ReSizeControl(int cx, int cy);		//调整控件的大小和位置

private:
	CPlayListCtrl m_playlist_ctrl{ CPlayer::GetInstance().GetPlayList() };
	CStaticEx m_path_static;
	CEdit m_path_edit;
	CButton m_set_path_button;
	CEdit m_search_edit;
	CButton m_clear_search_button;

	CMenu& m_popup_menu;		//播放列表右键菜单

	SLayoutData m_layout;		//窗口布局的固定数据

	bool m_searched;		//播放列表是否处于搜索状态
	int& m_item_selected;		//播放列表中鼠标选中的项目
	vector<int>& m_items_selected;

private:
	bool Initilized() const;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNMRClickPlaylistList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkPlaylistList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeSearchEdit();
	afx_msg void OnBnClickedClearSearchButton();
};
