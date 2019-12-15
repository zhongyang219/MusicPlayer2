#pragma once
#include "ListCtrlEx.h"
#include "SearchEditCtrl.h"
#include "TreeCtrlEx.h"


// CFolderExploreDlg 对话框

class CFolderExploreDlg : public CTabDlg
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
    void GetSongsSelected(std::vector<wstring>& song_list) const;
    void GetSongsSelected(std::vector<SongInfo>& song_list) const;
    void GetCurrentSongList(std::vector<SongInfo>& song_list) const;

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

    CString m_folder_path_selected;
    bool m_left_selected{};                   //最后一次选中的是左侧还是右侧
    std::vector<int> m_right_selected_items;   //右侧列表选中的项目的序号
    int m_right_selected_item{ -1 };
    HTREEITEM m_tree_item_selected{};
    CString m_selected_string;
    bool m_searched{ false };

protected:
    void ShowFolderTree();
    void ShowSongList();
    void FolderTreeClicked(HTREEITEM hItem);
    void SongListClicked(int index);
    void SetButtonsEnable(bool enable);
    bool _OnAddToNewPlaylist(std::wstring& playlist_path);       //执行添加到新建播放列表命令，成功返回true，playlist_path用于接收新播放列表的路径

    virtual void OnTabEntered() override;
    static UINT ViewOnlineThreadFunc(LPVOID lpParam);	//执行在线查看的线程函数

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnNMRClickFolderExploreTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMClickFolderExploreTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnPlayItem();
    afx_msg void OnNMClickSongList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMRClickSongList(NMHDR *pNMHDR, LRESULT *pResult);
    virtual void OnOK();
    afx_msg void OnAddToNewPlaylist();
    afx_msg void OnAddToNewPalylistAndPlay();
    afx_msg void OnPlayItemInFolderMode();
    afx_msg void OnExploreOnline();
    afx_msg void OnFormatConvert();
    afx_msg void OnExploreTrack();
    afx_msg void OnItemProperty();
    afx_msg void OnCopyText();
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
    virtual void OnCancel();
    afx_msg void OnNMDblclkFolderExploreTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMDblclkSongList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnEnChangeMfceditbrowse1();
protected:
    afx_msg LRESULT OnSearchEditBtnClicked(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnInitMenu(CMenu* pMenu);
};
