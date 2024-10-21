#pragma once
#include "Common.h"
#include "ListCtrlEx.h"
#include "TabDlg.h"
#include "SearchEditCtrl.h"
#include "ListSearchCache.h"

// CSetPathDlg 对话框
// #define WM_PATH_SELECTED (WM_USER+107) 不再使用

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
    void AdjustColumnWidth();                       //自动调整列表宽度
    void RefreshTabData();                          //刷新标签页数据
    bool SetCurSel(const ListItem& list_item);

protected:
    CListSearchCache m_list_search_cache;           // 缓存此窗口使用的所有ListItem
    CListCtrlEx m_path_list;                        // 列表对象，数据与m_list_search_cache同步
    int m_list_selected{};                          // 选中的列表数据索引

    CEdit m_path_name;
    CSearchEditCtrl m_search_edit;

    // 选中有效时启用“播放选中”按钮
    void SetButtonsEnable();
    // 更新m_list_search_cache和m_path_list
    void ShowPathList();
    // 计算列宽
    void CalculateColumeWidth(vector<int>& width);

    virtual void OnTabEntered() override;
    virtual bool InitializeControls() override;
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    //afx_msg void OnBnClickedDeletePathButton();
    //virtual BOOL OnUiCommand(WPARAM wParam, LPARAM lParam);
    afx_msg void OnNMClickPathList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMRClickPathList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMDblclkPathList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnSize(UINT nType, int cx, int cy);
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
    afx_msg void OnBnClickedSortButton();
    afx_msg void OnLibFolderSortRecentPlayed();
    afx_msg void OnLibFolderSortRecentAdded();
    afx_msg void OnLibFolderSortPath();
    afx_msg void OnLibFolderProperties();
    afx_msg void OnFileOpenFolder();
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
    afx_msg void OnAddToNewPlaylist();
};
