#pragma once
#include "ListCtrlEx.h"
#include "SearchEditCtrl.h"
#include "TreeCtrlEx.h"
#include "MediaLibTabDlg.h"
#include "HorizontalSplitter.h"


// CFolderExploreDlg 对话框

class CFolderExploreDlg : public CMediaLibTabDlg
{
    DECLARE_DYNAMIC(CFolderExploreDlg)

public:
    CFolderExploreDlg(CWnd* pParent = nullptr);   // 标准构造函数
    virtual ~CFolderExploreDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_FOLDER_EXPLORE_DIALOG };
#endif

public:
    void RefreshData();

protected:
    enum SongColumeIndex
    {
        COL_FILE_NAME = 0,
        COL_TITLE,
        COL_ARTIST,
        COL_ALBUM,
        COL_PATH,
    };

protected:
    CSearchEditCtrl m_search_edit;
    CTreeCtrlEx m_folder_explore_tree;
    CListCtrlEx m_song_list_ctrl;
    CListCtrlEx::ListData m_list_data;          // 右侧列表数据


    bool m_left_selected{};                     // 最后一次选中的是左侧还是右侧
    HTREEITEM m_tree_item_selected{};
    wstring m_folder_path_selected;

    int m_right_selected_item{ -1 };            // 右侧列表选中的项目的序号
    std::vector<int> m_right_selected_items;    // 右侧列表多选选中的项目的序号
    vector<SongInfo> m_right_items;             // 右侧列表歌曲（以路径在媒体库内加载得到，cue未解析）

    wstring m_selected_string;
    bool m_searched{ false };
    bool m_initialized{ false };
    CHorizontalSplitter m_splitter_ctrl;

protected:

    void ShowFolderTree();
    void ShowSongList();
    void FolderTreeClicked(HTREEITEM hItem);
    void SongListClicked(int index);
    void SetButtonsEnable(bool enable);

    virtual void OnTabEntered() override;

    virtual void GetSongsSelected(std::vector<SongInfo>& song_list) const override;
    virtual const vector<SongInfo>& GetSongList() const override;
    virtual int GetItemSelected() const override;
    virtual const vector<int>& GetItemsSelected() const override;
    virtual void RefreshSongList() override;
    virtual void AfterDeleteFromDisk(const std::vector<SongInfo>& files) override;
    virtual wstring GetSelectedString() const override;
    virtual wstring GetNewPlaylistName() const override;   // 添加到新建播放列表命令以此方法获取建议的新播放列表名称

    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnNMRClickFolderExploreTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMClickFolderExploreTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMClickSongList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMRClickSongList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMDblclkFolderExploreTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMDblclkSongList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnEnChangeMfceditbrowse1();
protected:
    afx_msg LRESULT OnSearchEditBtnClicked(WPARAM wParam, LPARAM lParam);
public:
    virtual void OnOK();
    afx_msg void OnBrowsePath();
};
