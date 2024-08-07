#pragma once
#include "Common.h"
#include "RecentFolderMgr.h"
#include "ListCtrlEx.h"
#include "TabDlg.h"
#include "SearchEditCtrl.h"

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
    void QuickSearch(const wstring& key_words);     //根据关键字执行快速查找（更新m_search_result）
    void AdjustColumnWidth();                       //自动调整列表宽度
    void RefreshTabData();                          //刷新标签页数据
    bool SetCurSel(const wstring& folder_path);

protected:
    bool m_searched{ false };           // 是否处于搜索状态
    wstring m_searched_str;
    vector<size_t> m_search_result;     // 储存快速搜索结果的歌曲序号

    CListCtrlEx m_path_list;            // 列表对象，数据加载自m_path_list_info，搜索状态下经过m_search_result筛选
    vector<PathInfo> m_path_list_info;  // 更新时复制自recent_path，因为无法确认recent_path修改时总能通知此窗口故使用复制保证与m_path_list的同步
    int m_list_selected{};              // 选中的列表数据索引（搜索状态下不是m_path_list_info的索引）

    CEdit m_path_name;
    CSearchEditCtrl m_search_edit;

    // 判断选择是否有效
    bool SelectValid() const;
    // 获取选择的路径
    PathInfo GetSelPath() const;
    // 返回当前选中GetSelPath()是否能够播放
    bool SelectedCanPlay() const;
    // 判断并设置“播放选中”按钮状态
    void SetButtonsEnable();
    // 更新m_path_list_info和m_path_list，搜索状态下请确保m_search_result不越界
    void ShowPathList();
    // 计算列宽
    void CalculateColumeWidth(vector<int>& width);
    void SetListRowData(int index, const PathInfo& path_info);

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
