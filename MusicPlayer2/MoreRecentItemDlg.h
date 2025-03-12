#pragma once
#include "BaseDialog.h"
#include "CListBoxEnhanced.h"
#include "SearchEditCtrl.h"
#include "ListCache.h"

// CMoreRecentItemDlg 对话框

class CMoreRecentItemDlg : public CBaseDialog
{
    DECLARE_DYNAMIC(CMoreRecentItemDlg)

public:
    CMoreRecentItemDlg(CWnd* pParent = nullptr);   // 标准构造函数
    virtual ~CMoreRecentItemDlg();
    ListItem GetSelectedItem() const;

// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_ADD_TO_PLAYLIST_DIALOG };
#endif

protected:
    CListBoxEnhanced m_list_ctrl;
    CSearchEditCtrl m_search_edit;
    wstring m_search_str;               // 搜索的字符串，非空时处于搜索状态
    vector<size_t> m_search_result;     // 搜索结果(与m_list_ctrl保持同步)
    int m_selected_item{};

    CListCache m_list_cache;

protected:
    virtual CString GetDialogName() const override;
    virtual bool InitializeControls() override;

    void ShowList();

    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMRClickList1(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnEnChangeSearchEdit();
protected:
    afx_msg LRESULT OnSearchEditBtnClicked(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnListboxSelChanged(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnPlayItem();
    afx_msg void OnRecentPlayedRemove();
    afx_msg void OnCopyText();
    afx_msg void OnViewInMediaLib();
    afx_msg void OnLibRecentPlayedItemProperties();
    afx_msg void OnInitMenu(CMenu* pMenu);
};
