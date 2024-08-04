#pragma once
#include "BaseDialog.h"
#include "CListBoxEnhanced.h"
#include "SearchEditCtrl.h"
#include "RecentFolderAndPlaylist.h"

// CMoreRecentItemDlg 对话框

class CMoreRecentItemDlg : public CBaseDialog
{
    DECLARE_DYNAMIC(CMoreRecentItemDlg)

public:
    CMoreRecentItemDlg(CWnd* pParent = nullptr);   // 标准构造函数
    virtual ~CMoreRecentItemDlg();
    const CRecentFolderAndPlaylist::Item* GetSelectedItem() const;

// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_ADD_TO_PLAYLIST_DIALOG };
#endif

protected:
    CListBoxEnhanced m_list_ctrl;
    CSearchEditCtrl m_search_edit;
    bool m_searched{ false };           //是否处于搜索状态
    std::vector<CRecentFolderAndPlaylist::Item> m_search_result; //搜索结果
    int m_selected_item{};

protected:
    virtual CString GetDialogName() const override;
    virtual bool InitializeControls() override;

    void ShowList();
    void QuickSearch(const wstring& key_word);

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
