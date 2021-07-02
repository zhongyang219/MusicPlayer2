#pragma once
#include "afxwin.h"
#include "Common.h"
#include "AudioCommon.h"
#include "afxcmn.h"
#include "ListCtrlEx.h"
#include "TabDlg.h"
#include "SearchEditCtrl.h"

// CSetPathDlg 对话框
#define WM_PATH_SELECTED (WM_USER+107)

class CSetPathDlg : public CTabDlg
{
	DECLARE_DYNAMIC(CSetPathDlg)

public:
	CSetPathDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSetPathDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SET_PATH_DIALOG };
#endif

public:
	void QuickSearch(const wstring& key_words);		//根据关键字执行快速查找m_search_result中
    void AdjustColumnWidth();                       //自动调整列表宽度
    void RefreshTabData();                          //刷新标签页数据

protected:
	deque<PathInfo>& m_recent_path;		//最近打开过的路径
	int m_path_selected{};		//选择的路径

	CEdit m_path_name;
	CListCtrlEx m_path_list;
	//CMenu m_menu;
	CSearchEditCtrl m_search_edit;
	//CToolTipCtrl m_Mytip;

	CSize m_min_size;		//窗口的最小大小

	vector<int> m_search_result;			//储存快速搜索结果的歌曲序号
	bool m_searched{ false };				//是否处理搜索状态
    bool m_folder_selected{ false };
    bool m_current_folder_contain_sub_folder{ false };      //初始时正在播放的文件夹是否包含子文件夹

protected:
	void ShowPathList();
	void SetButtonsEnable(bool enable);
	void CalculateColumeWidth(vector<int>& width);
	void SetListRowData(int index, const PathInfo& path_info);
	bool IsSelectedPlayEnable() const;			//判断选中路径是否可以播放
    virtual void OnTabEntered() override;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    PathInfo GetSelPath() const;		//获取选择的路径
	//int GetTrack() const;
	//int GetPosition() const;
	//SortMode GetSortMode() const;
	bool SelectValid() const;		//判断选择是否有效

	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//afx_msg void OnBnClickedDeletePathButton();
	//virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnNMClickPathList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickPathList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkPathList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnBnClickedOpenFolder();
	afx_msg void OnPlayPath();
	afx_msg void OnDeletePath();
	afx_msg void OnBrowsePath();
	afx_msg void OnClearInvalidPath();
	afx_msg void OnInitMenu(CMenu* pMenu);
	afx_msg void OnEnChangeSearchEdit();
	//afx_msg void OnBnClickedClearButton();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
    afx_msg LRESULT OnSearchEditBtnClicked(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnContainSubFolder();
};
