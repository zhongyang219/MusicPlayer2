#pragma once
#include "CListBoxEnhanced.h"
#include "BaseDialog.h"
#include "SearchEditCtrl.h"


// CAddToPlaylistDlg 对话框

class CAddToPlaylistDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CAddToPlaylistDlg)

public:
	CAddToPlaylistDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CAddToPlaylistDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ADD_TO_PLAYLIST_DIALOG };
#endif

    CString GetPlaylistSelected() const { return m_playlist_selected; }

protected:
    CListBoxEnhanced m_playlist_list_ctrl;
    CString m_playlist_selected;
    CSearchEditCtrl m_search_edit;
    bool m_searched{ false };           //是否处于搜索状态
    std::list<wstring> m_list;          //播放列表的列表
    std::list<wstring> m_search_result; //搜索结果

protected:
    virtual CString GetDialogName() const override;
    void ShowList();
    void QuickSearch(const wstring& key_word);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnEnChangeSearchEdit();
protected:
    afx_msg LRESULT OnSearchEditBtnClicked(WPARAM wParam, LPARAM lParam);
};
