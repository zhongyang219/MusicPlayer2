#pragma once
#include "TabDlg.h"
#include "PlaylistMgr.h"
#include "ListCtrlEx.h"
#include "SearchEditCtrl.h"
#include "MediaLibTabDlg.h"
#include "HorizontalSplitter.h"

// CSelectPlaylist 对话框
// #define WM_PLAYLIST_SELECTED (WM_USER+119) 不再使用     //WPARA: 传递对话框窗口的指针；LPARA：-2:表示要播放默认的播放列表，>=0，表示要播放列表中指定序号的曲目

class CSelectPlaylistDlg : public CMediaLibTabDlg
{
    DECLARE_DYNAMIC(CSelectPlaylistDlg)

public:
    CSelectPlaylistDlg(CWnd* pParent = nullptr);   // 标准构造函数
    virtual ~CSelectPlaylistDlg();

    // 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_SELECT_PLAYLIST_DIALOG };
#endif

    // 自动调整列表宽度
    void AdjustColumnWidth();
    // 完全重新载入标签页数据
    void RefreshTabData();

    bool SetCurSel(const wstring& playlist_path);

private:

    bool m_searched{ false };                   // 是否处于搜索状态
    wstring m_searcher_str;                     // 搜索字符串
    vector<size_t> m_search_result;             // 储存快速搜索结果的歌曲序号(筛选m_playlist_ctrl_data放入m_playlist_ctrl)
    CSearchEditCtrl m_search_edit;

    bool m_left_selected{ false };              // 最后一次选中的是左侧还是右侧
    int m_left_selected_item{ -1 };             // 左侧选中的播放列表项目的索引（搜索状态下不是m_playlist_ctrl_data的索引）
    int m_right_selected_item{ -1 };            // 右侧列表选中的项目的索引
    std::vector<int> m_right_selected_items;    // 右侧列表多选选中的项目的索引
    wstring m_selected_string;

    enum SongColumeIndex
    {
        COL_INDEX,
        COL_TITLE,
        COL_ARTIST,
        COL_ALBUM,
        COL_TRACK,
        COL_GENRE,
        COL_BITRATE,
        COL_PATH,
        COL_MAX
    };

    vector<PlaylistInfo> m_playlist_ctrl_data;  // 与CPlaylistMgr数据同步（含有特殊播放列表）
    CListCtrlEx m_playlist_ctrl;
    CListCtrlEx m_song_list_ctrl;
    CListCtrlEx::ListData m_list_data;  //右侧列表数据
    vector<SongInfo> m_cur_song_list;   //选中播放列表的数据
    CHorizontalSplitter m_splitter_ctrl;

protected:

    virtual const vector<SongInfo>& GetSongList() const override;
    virtual int GetItemSelected() const override;
    virtual const vector<int>& GetItemsSelected() const override;
    virtual void RefreshSongList() override;
    virtual void AfterDeleteFromDisk(const std::vector<SongInfo>& files) override;
    virtual wstring GetSelectedString() const override;

    virtual void OnTabEntered() override;
    virtual bool InitializeControls() override;
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
private:
    // 根据关键字执行快速查找（更新m_search_result）
    void QuickSearch(const wstring& key_words);
    // 播放列表模式下在m_playlist_ctrl_data中查找当前播放的播放列表索引，否则返回-1
    int GetPlayingItem();
    // 左侧列表选中时在右侧列表显示其内容并更新此列表曲目数/总时长
    void ShowSongList();
    // 左侧列表点击时更新选中状态（参数为m_playlist_ctrl索引）
    void LeftListClicked(int index);
    // 右侧列表点击时更新选中状态（参数为m_song_list_ctrl索引）
    void SongListClicked(int index);
    // 设置左侧列表m_playlist_ctrl索引为index的项选中并高亮
    void SetLeftListSelected(int index);
    // 计算左侧列表列宽
    void CalculateColumeWidth(vector<int>& width);
    // 更新m_playlist_ctrl_data并显示，搜索状态下搜索并显示
    void ShowPathList();
    // 更新m_playlist_ctrl索引为index的除“序号”列以外的一行显示
    void SetListRowData(int index, const PlaylistInfo& playlist_info);
    // 当前左侧列表选中有效
    bool LeftSelectValid() const;
    // 获取左侧列表当前选中项对应PlaylistInfo，无效时返回空对象
    PlaylistInfo GetSelectedPlaylist() const;
    // 向媒体库窗口发送消息更新按钮状态
    void SetButtonsEnable();
    // 判断当前选中是否可播放
    bool SelectedCanPlay() const;
    // 执行“新建播放列表”操作，返回新播放列表的路径
    wstring DoNewPlaylist();

public:

    virtual BOOL OnInitDialog();
    afx_msg void OnNMDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
    virtual void OnOK();
    afx_msg void OnBnClickedNewPlaylist();
    afx_msg void OnPlayPlaylist();
    afx_msg void OnRenamePlaylist();
    afx_msg void OnDeletePlaylist();
    afx_msg void OnNMClickList1(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnNMRClickList1(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnInitMenu(CMenu* pMenu);
    afx_msg void OnNewPlaylist();
    afx_msg void OnEnChangeSearchEdit();
    //afx_msg void OnBnClickedClearButton();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
    afx_msg LRESULT OnSearchEditBtnClicked(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnNMClickSongList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnNMRClickSongList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnNMDblclkSongList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnSaveAsNewPlaylist();
    afx_msg void OnPlaylistSaveAs();
    afx_msg void OnPlaylistFixPathError();
    afx_msg void OnPlaylistBrowseFile();
    afx_msg void OnRemoveFromPlaylist();
    afx_msg void OnBnClickedSortButton();
    afx_msg void OnLibPlaylistSortRecentPlayed();
    afx_msg void OnLibPlaylistSortRecentCreated();
    afx_msg void OnLibPlaylistSortName();
    afx_msg void OnLibPlaylistProperties();
};
