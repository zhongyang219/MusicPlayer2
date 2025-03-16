#pragma once
#include "afxdialogex.h"
#include "TabDlg.h"
#include "ListCtrlEx.h"
#include "SearchEditCtrl.h"
#include "ListCache.h"

// CFindListDlg 对话框

class CFindListDlg : public CTabDlg
{
    DECLARE_DYNAMIC(CFindListDlg)

public:
    CFindListDlg(CWnd* pParent = nullptr);   // 标准构造函数
    virtual ~CFindListDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_FIND_LIST_DIALOG };
#endif

private:
    enum COLUMN
    {
        COL_NAME,
        COL_TRACK_NUM,
        COL_MAX
    };

    CSearchEditCtrl m_search_edit;

    //列表的来源
    enum class ItemFrom
    {
        UNKNOWN,
        RECENT_PLAYED,	//最近播放
        MEDIALIB_ITEM,	//未播放过的媒体库项目
        FOLDER_EXPLORE	//“文件夹浏览”中未播放过的文件夹
    };
    struct FindListItem
    {
        ListItem list_data;
        ItemFrom item_from;
        bool operator==(const ListItem& another_list_data) const
        {
            return list_data == another_list_data;
        }
    };
    std::vector<FindListItem> m_all_list_items;
    CListCtrlEx m_list_ctrl;
    CListCtrlEx::ListData m_list_data;              // 列表数据
    CListCtrlEx::ListData m_list_data_searched;     // 搜索后的列表数据
    vector<int> m_search_result;		//保存搜索后的索引

    bool m_searched{ false };           // 是否处于搜索状态
    bool m_initialized{ false };

private:
    void InitListData();
    virtual void OnTabEntered() override;

    void AddListCacheData(const CListCache& list_cache);	//向m_all_list_items中添加一个CListCache中的所有项目
    void AddMediaLibItem(CMediaClassifier::ClassificationType type);	//向m_all_list_items中添加媒体库中指定类型的所有项目
    void AddAllFolders();	//向m_all_list_items中添加媒体库中所有文件夹

    void ShowList();
    void QuickSearch(const wstring& key_word);      //根据关键字执行快速查找，将结果保存在m_list_data_searched中

    //向父窗口发送消息以更新“播放选中”按钮的状态
    void SetPlaySelectedEnable(bool enable);

    FindListItem GetSelectedItem() const;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    virtual void OnOK();
protected:
    afx_msg LRESULT OnSearchEditBtnClicked(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnEnChangeSearchEdit();
    afx_msg void OnNMClickSongList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnNMDblclkSongList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnNMRClickSongList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnPlayItem();
    afx_msg void OnCopyText();
    afx_msg void OnViewInMediaLib();
    afx_msg void OnLibRecentPlayedItemProperties();
    afx_msg void OnInitMenu(CMenu* pMenu);
};
