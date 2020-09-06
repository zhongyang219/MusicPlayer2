#pragma once
#include "ListCtrlEx.h"
#include "SearchEditCtrl.h"
#include "TreeCtrlEx.h"
#include "MediaLibTabDlg.h"


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
    virtual void GetSongsSelected(std::vector<wstring>& song_list) const override;
	virtual void GetSongsSelected(std::vector<SongInfo>& song_list) const override;
	virtual void GetCurrentSongList(std::vector<SongInfo>& song_list) const override;
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

    CString m_folder_path_selected;
    bool m_left_selected{};                   //最后一次选中的是左侧还是右侧
    std::vector<int> m_right_selected_items;   //右侧列表选中的项目的序号
    int m_right_selected_item{ -1 };
    HTREEITEM m_tree_item_selected{};
    CString m_selected_string;
    bool m_searched{ false };
	CListCtrlEx::ListData m_list_data;	//右侧列表数据
    bool m_initialized{ false };

protected:
    virtual void RefreshSongList() override;
    void ShowFolderTree();
    void ShowSongList();
    void FolderTreeClicked(HTREEITEM hItem);
    void SongListClicked(int index);
    void SetButtonsEnable(bool enable);
    bool _OnAddToNewPlaylist(std::wstring& playlist_path);       //执行添加到新建播放列表命令，成功返回true，playlist_path用于接收新播放列表的路径

    virtual void OnTabEntered() override;
    static UINT ViewOnlineThreadFunc(LPVOID lpParam);	//执行在线查看的线程函数

	virtual const CListCtrlEx& GetSongListCtrl() const override;
	virtual int GetItemSelected() const override;
	virtual const vector<int>& GetItemsSelected() const override;
	virtual void AfterDeleteFromDisk(const std::vector<SongInfo>& files) override;
	virtual int GetPathColIndex() const override;
	virtual wstring GetSelectedString() const override;

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
    afx_msg void OnInitMenu(CMenu* pMenu);
	virtual void OnOK();
};
