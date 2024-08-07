// CSelectPlaylist.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "Player.h"
#include "CSelectPlaylist.h"
#include "InputDlg.h"
#include "Playlist.h"
#include "SongDataManager.h"
#include "RecentFolderAndPlaylist.h"
#include "MusicPlayerCmdHelper.h"
#include "CommonDialogMgr.h"
#include "FilterHelper.h"
#include "PlaylistPropertiesDlg.h"

// CSelectPlaylist 对话框

IMPLEMENT_DYNAMIC(CSelectPlaylistDlg, CMediaLibTabDlg)

CSelectPlaylistDlg::CSelectPlaylistDlg(CWnd* pParent /*=nullptr*/)
    : CMediaLibTabDlg(IDD_SELECT_PLAYLIST_DIALOG, pParent)
{

}

CSelectPlaylistDlg::~CSelectPlaylistDlg()
{
}

void CSelectPlaylistDlg::RefreshSongList()
{
    ShowSongList();
}

void CSelectPlaylistDlg::AdjustColumnWidth()
{
    vector<int> width;
    CalculateColumeWidth(width);
    for (size_t i{}; i < width.size(); i++)
        m_playlist_ctrl.SetColumnWidth(i, width[i]);
}

void CSelectPlaylistDlg::RefreshTabData()
{
    ShowPathList();
    ShowSongList();
}

bool CSelectPlaylistDlg::SetCurSel(const wstring& playlist_path)
{
    if (!m_searched)
    {
        int index{ -1 };
        for (size_t i{}; i < m_playlist_ctrl_data.size(); i++)
        {
            if (m_playlist_ctrl_data[i].path == playlist_path)
                index = i;
        }

        if (index >= 0)
        {
            m_playlist_ctrl.SetCurSel(index);
            m_playlist_ctrl.EnsureVisible(index, FALSE);
            LeftListClicked(index);
            return true;
        }
        return false;
    }
    return true;
}

void CSelectPlaylistDlg::DoDataExchange(CDataExchange* pDX)
{
    CMediaLibTabDlg::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_playlist_ctrl);
    DDX_Control(pDX, IDC_SEARCH_EDIT, m_search_edit);
    DDX_Control(pDX, IDC_SONG_LIST, m_song_list_ctrl);
    DDX_Control(pDX, IDC_HSPLITER_STATIC, m_splitter_ctrl);
}


void CSelectPlaylistDlg::QuickSearch(const wstring& key_words)
{
    m_search_result.clear();
    for (size_t i{ CPlaylistMgr::SPEC_PLAYLIST_NUM }; i < m_playlist_ctrl_data.size(); ++i)   // 跳过前两个特殊播放列表
    {
        // m_search_result存储m_playlist_ctrl_data的索引
        CFilePathHelper file_path{ m_playlist_ctrl_data[i].path };
        wstring playlist_name = file_path.GetFileNameWithoutExtension();
        if (file_path.GetFileName() != TEMP_PLAYLIST_NAME && theApp.m_chinese_pingyin_res.IsStringMatchWithPingyin(key_words, playlist_name))
            m_search_result.push_back(i);
    }
}

int CSelectPlaylistDlg::GetPlayingItem()
{
    //正在播放的项目
    int playing_item{ -1 };
    if (CPlayer::GetInstance().IsPlaylistMode() && !m_searched)
    {
        wstring playing_playlist_path = CPlayer::GetInstance().GetPlaylistPath();
        auto iter = std::find_if(m_playlist_ctrl_data.begin(), m_playlist_ctrl_data.end(), [&](const PlaylistInfo& playlist_info) { return playing_playlist_path == playlist_info.path; });
        if (iter != m_playlist_ctrl_data.end())
            playing_item = iter - m_playlist_ctrl_data.begin();
    }
    return playing_item;
}

void CSelectPlaylistDlg::ShowSongList()
{
    CWaitCursor wait_cursor;
    m_list_data.clear();
    int highlight_item{ -1 };
    if (LeftSelectValid())
    {
        PlaylistInfo playlist_info = GetSelectedPlaylist();
        CPlaylistFile playlist_file;
        playlist_file.LoadFromFile(playlist_info.path);
        // 如果是当前播放列表则设置当前播放高亮
        if (CPlayer::GetInstance().IsPlaylistMode() && GetSelectedPlaylist().path == CPlayer::GetInstance().GetPlaylistPath())
            highlight_item = playlist_file.GetSongIndexInPlaylist(CPlayer::GetInstance().GetCurrentSongInfo());
        playlist_file.MoveToSongList(m_cur_song_list);

        CSongDataManager::GetInstance().LoadSongsInfo(m_cur_song_list);  // 从媒体库加载歌曲属性
        int index{}, totla_time{};
        for (const SongInfo& song : m_cur_song_list)
        {
            CListCtrlEx::RowData row_data;
            row_data[COL_INDEX] = std::to_wstring(++index);
            row_data[COL_TITLE] = song.GetTitle();
            row_data[COL_ARTIST] = song.GetArtist();
            row_data[COL_ALBUM] = song.GetAlbum();
            std::wstring track_str;
            if (song.track != 0)
                track_str = std::to_wstring(song.track);
            row_data[COL_TRACK] = track_str;
            row_data[COL_GENRE] = song.GetGenre();
            row_data[COL_BITRATE] = (song.bitrate == 0 ? L"-" : std::to_wstring(song.bitrate));
            row_data[COL_PATH] = song.file_path;
            m_list_data.push_back(std::move(row_data));

            totla_time += song.length().toInt();
        }
        // 将播放列表的曲目数、总时长的变化更新到CPlaylistMgr中再更新本窗口数据
        if (playlist_info.total_time != totla_time || playlist_info.track_num != m_cur_song_list.size())
        {
            playlist_info.track_num = m_cur_song_list.size();
            playlist_info.total_time = totla_time;
            CPlaylistMgr::Instance().UpdatePlaylistInfo(playlist_info);
            m_playlist_ctrl_data[m_searched ? m_search_result[m_left_selected_item] : m_left_selected_item] = playlist_info;
            SetListRowData(m_left_selected_item, playlist_info);
        }
    }
    m_song_list_ctrl.SetListData(&m_list_data);
    m_song_list_ctrl.SetHightItem(highlight_item);
    m_song_list_ctrl.EnsureVisible(highlight_item, FALSE);
}

void CSelectPlaylistDlg::LeftListClicked(int index)
{
    m_left_selected = true;
    m_left_selected_item = index;
    m_right_selected_item = -1;         // 点击左侧列表时清空右侧列表选中项
    m_right_selected_items.clear();
    m_song_list_ctrl.SelectNone();
    SetButtonsEnable();
    ShowSongList();
}

void CSelectPlaylistDlg::SongListClicked(int index)
{
    m_left_selected = false;
    m_right_selected_item = index;
    m_song_list_ctrl.GetItemSelected(m_right_selected_items);
    SetButtonsEnable();
}

void CSelectPlaylistDlg::SetLeftListSelected(int index)
{
    m_playlist_ctrl.SetCurSel(index);
    LeftListClicked(index);
}

const vector<SongInfo>& CSelectPlaylistDlg::GetSongList() const
{
    return m_cur_song_list;
}

int CSelectPlaylistDlg::GetItemSelected() const
{
    return m_right_selected_item;
}

const vector<int>& CSelectPlaylistDlg::GetItemsSelected() const
{
    return m_right_selected_items;
}

void CSelectPlaylistDlg::AfterDeleteFromDisk(const std::vector<SongInfo>& files)
{
    ShowSongList();
}

wstring CSelectPlaylistDlg::GetSelectedString() const
{
    return m_selected_string;
}

void CSelectPlaylistDlg::OnTabEntered()
{
    if (m_playlist_ctrl.GetCurSel() != -1)
        m_left_selected_item = m_playlist_ctrl.GetCurSel();     // m_left_selected_item直接存储m_playlist_ctrl的索引
    SetButtonsEnable();
}

bool CSelectPlaylistDlg::InitializeControls()
{
    SetDlgControlText(IDC_NEW_PLAYLIST, L"TXT_LIB_PLAYLIST_NEW_PLAYLIST");
    SetDlgControlText(IDC_SORT_BUTTON, L"TXT_LIB_PLAYLIST_SORT");
    // IDC_SEARCH_EDIT
    // IDC_LIST1
    // IDC_HSPLITER_STATIC
    // IDC_SONG_LIST

    RepositionTextBasedControls({
        { CtrlTextInfo::L2, IDC_NEW_PLAYLIST, CtrlTextInfo::W32 },
        { CtrlTextInfo::L1, IDC_SORT_BUTTON, CtrlTextInfo::W32 },
        { CtrlTextInfo::R1, IDC_SEARCH_EDIT }
        });
    return true;
}

BEGIN_MESSAGE_MAP(CSelectPlaylistDlg, CMediaLibTabDlg)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CSelectPlaylistDlg::OnNMDblclkList1)
    ON_BN_CLICKED(IDC_NEW_PLAYLIST, &CSelectPlaylistDlg::OnBnClickedNewPlaylist)
    ON_COMMAND(ID_PLAY_PLAYLIST, &CSelectPlaylistDlg::OnPlayPlaylist)
    ON_COMMAND(ID_RENAME_PLAYLIST, &CSelectPlaylistDlg::OnRenamePlaylist)
    ON_COMMAND(ID_DELETE_PLAYLIST, &CSelectPlaylistDlg::OnDeletePlaylist)
    ON_NOTIFY(NM_CLICK, IDC_LIST1, &CSelectPlaylistDlg::OnNMClickList1)
    ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CSelectPlaylistDlg::OnNMRClickList1)
    ON_WM_INITMENU()
    ON_COMMAND(ID_NEW_PLAYLIST, &CSelectPlaylistDlg::OnNewPlaylist)
    ON_EN_CHANGE(IDC_SEARCH_EDIT, &CSelectPlaylistDlg::OnEnChangeSearchEdit)
    //ON_BN_CLICKED(IDC_CLEAR_BUTTON, &CSelectPlaylistDlg::OnBnClickedClearButton)
    ON_MESSAGE(WM_SEARCH_EDIT_BTN_CLICKED, &CSelectPlaylistDlg::OnSearchEditBtnClicked)
    ON_NOTIFY(NM_CLICK, IDC_SONG_LIST, &CSelectPlaylistDlg::OnNMClickSongList)
    ON_NOTIFY(NM_RCLICK, IDC_SONG_LIST, &CSelectPlaylistDlg::OnNMRClickSongList)
    ON_NOTIFY(NM_DBLCLK, IDC_SONG_LIST, &CSelectPlaylistDlg::OnNMDblclkSongList)
    ON_COMMAND(ID_SAVE_AS_NEW_PLAYLIST, &CSelectPlaylistDlg::OnSaveAsNewPlaylist)
    ON_COMMAND(ID_PLAYLIST_SAVE_AS, &CSelectPlaylistDlg::OnPlaylistSaveAs)
    ON_COMMAND(ID_PLAYLIST_FIX_PATH_ERROR, &CSelectPlaylistDlg::OnPlaylistFixPathError)
    ON_COMMAND(ID_PLAYLIST_BROWSE_FILE, &CSelectPlaylistDlg::OnPlaylistBrowseFile)
    ON_COMMAND(ID_REMOVE_FROM_PLAYLIST, &CSelectPlaylistDlg::OnRemoveFromPlaylist)
    ON_BN_CLICKED(IDC_SORT_BUTTON, &CSelectPlaylistDlg::OnBnClickedSortButton)
    ON_COMMAND(ID_LIB_PLAYLIST_SORT_RECENT_PLAYED, &CSelectPlaylistDlg::OnLibPlaylistSortRecentPlayed)
    ON_COMMAND(ID_LIB_PLAYLIST_SORT_RECENT_CREATED, &CSelectPlaylistDlg::OnLibPlaylistSortRecentCreated)
    ON_COMMAND(ID_LIB_PLAYLIST_SORT_NAME, &CSelectPlaylistDlg::OnLibPlaylistSortName)
    ON_COMMAND(ID_LIB_PLAYLIST_PROPERTIES, &CSelectPlaylistDlg::OnLibPlaylistProperties)
END_MESSAGE_MAP()


// CSelectPlaylist 消息处理程序


BOOL CSelectPlaylistDlg::OnInitDialog()
{
    CMediaLibTabDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

    SetButtonIcon(IDC_NEW_PLAYLIST, IconMgr::IconType::IT_Add);
    SetButtonIcon(IDC_SORT_BUTTON, IconMgr::IconType::IT_Sort_Mode);

    //初始化播放列表控件
    vector<int> width;
    CalculateColumeWidth(width);
    m_playlist_ctrl.SetExtendedStyle(m_playlist_ctrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_LABELTIP);
    m_playlist_ctrl.InsertColumn(0, theApp.m_str_table.LoadText(L"TXT_SERIAL_NUMBER").c_str(), LVCFMT_LEFT, width[0]);
    m_playlist_ctrl.InsertColumn(1, theApp.m_str_table.LoadText(L"TXT_PLAYLIST").c_str(), LVCFMT_LEFT, width[1]);
    m_playlist_ctrl.InsertColumn(2, theApp.m_str_table.LoadText(L"TXT_LAST_PLAYED_TRACK").c_str(), LVCFMT_LEFT, width[2]);
    m_playlist_ctrl.InsertColumn(3, theApp.m_str_table.LoadText(L"TXT_NUM_OF_TRACK").c_str(), LVCFMT_LEFT, width[3]);
    m_playlist_ctrl.InsertColumn(4, theApp.m_str_table.LoadText(L"TXT_TOTAL_LENGTH").c_str(), LVCFMT_LEFT, width[4]);

    ////设置列表控件的提示总是置顶，用于解决如果弹出此窗口的父窗口具有置顶属性时，提示信息在窗口下面的问题
    //m_playlist_ctrl.GetToolTips()->SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    //m_Mytip.SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    //初始化右侧列表
    m_song_list_ctrl.SetExtendedStyle(m_song_list_ctrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_LABELTIP);
    m_song_list_ctrl.InsertColumn(COL_INDEX, theApp.m_str_table.LoadText(L"TXT_SERIAL_NUMBER").c_str(), LVCFMT_LEFT, theApp.DPI(40));
    m_song_list_ctrl.InsertColumn(COL_TITLE, theApp.m_str_table.LoadText(L"TXT_TITLE").c_str(), LVCFMT_LEFT, theApp.DPI(150));
    m_song_list_ctrl.InsertColumn(COL_ARTIST, theApp.m_str_table.LoadText(L"TXT_ARTIST").c_str(), LVCFMT_LEFT, theApp.DPI(100));
    m_song_list_ctrl.InsertColumn(COL_ALBUM, theApp.m_str_table.LoadText(L"TXT_ALBUM").c_str(), LVCFMT_LEFT, theApp.DPI(150));
    m_song_list_ctrl.InsertColumn(COL_TRACK, theApp.m_str_table.LoadText(L"TXT_TRACK_NUM").c_str(), LVCFMT_LEFT, theApp.DPI(60));
    m_song_list_ctrl.InsertColumn(COL_GENRE, theApp.m_str_table.LoadText(L"TXT_GENRE").c_str(), LVCFMT_LEFT, theApp.DPI(100));
    m_song_list_ctrl.InsertColumn(COL_BITRATE, theApp.m_str_table.LoadText(L"TXT_BITRATE").c_str(), LVCFMT_LEFT, theApp.DPI(60));
    m_song_list_ctrl.InsertColumn(COL_PATH, theApp.m_str_table.LoadText(L"TXT_FILE_PATH").c_str(), LVCFMT_LEFT, theApp.DPI(600));
    m_song_list_ctrl.SetCtrlAEnable(true);

    // ShowPathList初始化m_playlist_ctrl_data，需要在其他方法之前
    ShowPathList();
    SetLeftListSelected(GetPlayingItem());  // 初始化时选中正在播放的播放列表
    ShowSongList();
    m_search_edit.SetFocus();       //初始时将焦点设置到搜索框
    wstring prompt_str = theApp.m_str_table.LoadText(L"TXT_SEARCH_PROMPT") + L"(F)";
    m_search_edit.SetCueBanner(prompt_str.c_str(), TRUE);

    //初始化分隔条
    m_splitter_ctrl.AttachCtrlAsLeftPane(IDC_LIST1);
    m_splitter_ctrl.AttachCtrlAsRightPane(IDC_SONG_LIST);

    return FALSE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}

void CSelectPlaylistDlg::CalculateColumeWidth(vector<int>& width)
{
    CRect rect;
    m_playlist_ctrl.GetWindowRect(rect);
    width.resize(5);

    width[2] = width[3] = rect.Width() / 8;
    width[4] = rect.Width() / 5;

    width[0] = theApp.DPI(40);
    width[1] = rect.Width() - width[2] - width[3] - width[4] - width[0] - theApp.DPI(20) - 1;

}

void CSelectPlaylistDlg::ShowPathList()
{
    // 更新m_playlist_ctrl_data，此窗口仅在此处获取CPlaylistMgr数据以保证窗口内部数据一致性
    CPlaylistMgr::Instance().GetAllPlaylistInfo(m_playlist_ctrl_data);

    m_playlist_ctrl.EnableWindow(TRUE);
    m_playlist_ctrl.DeleteAllItems();
    if (!m_searched)        //显示所有项目
    {
        for (size_t i{}; i < m_playlist_ctrl_data.size(); ++i)
        {
            m_playlist_ctrl.InsertItem(i, std::to_wstring(i + 1).c_str());
            SetListRowData(i, m_playlist_ctrl_data[i]);
        }
    }
    else        //只显示搜索结果的曲目
    {
        QuickSearch(m_searcher_str);
        if (m_search_result.empty())
        {
            m_playlist_ctrl.InsertItem(0, _T(""));
            m_playlist_ctrl.SetItemText(0, 1, theApp.m_str_table.LoadText(L"TXT_PLAYLIST_CTRL_NO_RESULT_TO_SHOW").c_str());
            m_playlist_ctrl.EnableWindow(FALSE);
            return;
        }
        int index{};
        for (size_t i : m_search_result)
        {
            m_playlist_ctrl.InsertItem(index, std::to_wstring(i + 1).c_str());
            SetListRowData(index, m_playlist_ctrl_data[i]);
            ++index;
        }
    }
    m_playlist_ctrl.SetHightItem(GetPlayingItem());
}

void CSelectPlaylistDlg::SetListRowData(int index, const PlaylistInfo& playlist_info)
{
    wstring playlist_name = CPlaylistMgr::GetPlaylistDisplayName(playlist_info.path);
    m_playlist_ctrl.SetItemText(index, 1, playlist_name.c_str());

    CString str;
    if (playlist_info.track_num <= 0)
        str = _T("0");
    else
        str.Format(_T("%d"), playlist_info.track + 1);
    m_playlist_ctrl.SetItemText(index, 2, str);

    str.Format(_T("%d"), playlist_info.track_num);
    m_playlist_ctrl.SetItemText(index, 3, str);

    Time total_time{ playlist_info.total_time };
    m_playlist_ctrl.SetItemText(index, 4, total_time.toString3().c_str());
}


bool CSelectPlaylistDlg::LeftSelectValid() const
{
    int index{ m_left_selected_item };
    if (m_searched && index >= 0 && index < static_cast<int>(m_search_result.size()))
        index = m_search_result[index];
    return index >= 0 && index < static_cast<int>(m_playlist_ctrl_data.size());
}

PlaylistInfo CSelectPlaylistDlg::GetSelectedPlaylist() const
{
    if (LeftSelectValid())
        if (m_searched)
            return m_playlist_ctrl_data[m_search_result[m_left_selected_item]];
        else
            return m_playlist_ctrl_data[m_left_selected_item];
    else
        return PlaylistInfo{};
}

void CSelectPlaylistDlg::SetButtonsEnable()
{
    bool enable = SelectedCanPlay();
    CWnd* pParent = GetParentWindow();
    ::SendMessage(pParent->GetSafeHwnd(), WM_PLAY_SELECTED_BTN_ENABLE, WPARAM(enable), 0);
}

bool CSelectPlaylistDlg::SelectedCanPlay() const
{
    return LeftSelectValid() &&
        (
            !CPlayer::GetInstance().IsPlaylistMode() ||
            GetSelectedPlaylist().path != CPlayer::GetInstance().GetPlaylistPath() ||
            (m_right_selected_item >= 0 && GetSelectedPlaylist().track != m_right_selected_item)
            );
}

void CSelectPlaylistDlg::OnNMDblclkList1(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    LeftListClicked(pNMItemActivate->iItem);

    OnOK();

    *pResult = 0;
}


void CSelectPlaylistDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类
    if (SelectedCanPlay())
    {
        bool ok{};
        PlaylistInfo sel_playlist = GetSelectedPlaylist();
        if (m_left_selected || m_right_selected_item < 0)    // 左侧选中或右侧选中无效则使用之前保存的信息播放选中播放列表，设置play为false，force为false
            ok = CPlayer::GetInstance().SetPlaylist(sel_playlist.path, sel_playlist.track, sel_playlist.position, false, false);
        else        // 否则播放m_right_selected_item指定曲目，设置play为true，force为true
            ok = CPlayer::GetInstance().SetPlaylist(sel_playlist.path, m_right_selected_item, 0, true, true);
        if (!ok)
        {
            const wstring& info = theApp.m_str_table.LoadText(L"MSG_WAIT_AND_RETRY");
            MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
        }
        else
        {
            CTabDlg::OnOK();
            CWnd* pParent = GetParentWindow();
            if (pParent != nullptr)
                ::PostMessage(pParent->GetSafeHwnd(), WM_COMMAND, IDOK, 0);
        }
    }
}


void CSelectPlaylistDlg::OnBnClickedNewPlaylist()
{
    // TODO: 在此添加控件通知处理程序代码
    DoNewPlaylist();
}


wstring CSelectPlaylistDlg::DoNewPlaylist()
{
    CMusicPlayerCmdHelper helper(this);
    std::wstring new_playlist_path = helper.OnNewPlaylist();
    if (!new_playlist_path.empty())
    {
        ShowPathList();
        SetLeftListSelected(CPlaylistMgr::SPEC_PLAYLIST_NUM);     //选中新增的播放列表。添加新的播放列表后，新增的播放会排到前面，在特殊的播放列表的后一个位置
    }
    return new_playlist_path;
}

void CSelectPlaylistDlg::OnPlayPlaylist()
{
    // TODO: 在此添加命令处理程序代码
    OnOK();
}


void CSelectPlaylistDlg::OnRenamePlaylist()
{
    // TODO: 在此添加命令处理程序代码
    wstring sel_playlist_path = GetSelectedPlaylist().path;

    CMusicPlayerCmdHelper helper(this);
    if (helper.OnRenamePlaylist(sel_playlist_path))
        ShowPathList();
}


void CSelectPlaylistDlg::OnDeletePlaylist()
{
    // TODO: 在此添加命令处理程序代码
    wstring del_path = GetSelectedPlaylist().path;
    CMusicPlayerCmdHelper helper(this);
    if (helper.OnDeletePlaylist(del_path))
    {
        ShowPathList();
        SetLeftListSelected(GetPlayingItem());
        ShowSongList();
    }
}


void CSelectPlaylistDlg::OnNMClickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    LeftListClicked(pNMItemActivate->iItem);

    *pResult = 0;
}


void CSelectPlaylistDlg::OnNMRClickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    LeftListClicked(pNMItemActivate->iItem);
    m_selected_string = m_playlist_ctrl.GetItemText(pNMItemActivate->iItem, pNMItemActivate->iSubItem);

    //弹出右键菜单
    CMenu* pContextMenu = theApp.m_menu_mgr.GetMenu(MenuMgr::LibPlaylistMenu);
    m_playlist_ctrl.ShowPopupMenu(pContextMenu, pNMItemActivate->iItem, this);

    *pResult = 0;
}


void CSelectPlaylistDlg::OnInitMenu(CMenu* pMenu)
{
    CMediaLibTabDlg::OnInitMenu(pMenu);

    // TODO: 在此处添加消息处理程序代码
    bool select_valid{ LeftSelectValid() };
    wstring sel_playlist_name = CFilePathHelper(GetSelectedPlaylist().path).GetFileName();
    bool is_spec_playlist{ sel_playlist_name == DEFAULT_PLAYLIST_NAME || sel_playlist_name == FAVOURITE_PLAYLIST_NAME };
    bool is_temp_playlist{ sel_playlist_name == TEMP_PLAYLIST_NAME };
    pMenu->EnableMenuItem(ID_RENAME_PLAYLIST, MF_BYCOMMAND | (select_valid && !is_spec_playlist && !is_temp_playlist ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_DELETE_PLAYLIST, MF_BYCOMMAND | (select_valid && !is_spec_playlist ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_PLAY_PLAYLIST, MF_BYCOMMAND | (SelectedCanPlay() ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_SAVE_AS_NEW_PLAYLIST, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_PLAYLIST_SAVE_AS, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_PLAYLIST_FIX_PATH_ERROR, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_PLAYLIST_BROWSE_FILE, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));

    switch (CPlaylistMgr::Instance().GetSortMode())
    {
    case CPlaylistMgr::SM_RECENT_PLAYED: pMenu->CheckMenuRadioItem(ID_LIB_PLAYLIST_SORT_RECENT_PLAYED, ID_LIB_PLAYLIST_SORT_NAME, ID_LIB_PLAYLIST_SORT_RECENT_PLAYED, MF_BYCOMMAND | MF_CHECKED); break;
    case CPlaylistMgr::SM_RECENT_CREATED: pMenu->CheckMenuRadioItem(ID_LIB_PLAYLIST_SORT_RECENT_PLAYED, ID_LIB_PLAYLIST_SORT_NAME, ID_LIB_PLAYLIST_SORT_RECENT_CREATED, MF_BYCOMMAND | MF_CHECKED); break;
    case CPlaylistMgr::SM_NAME: pMenu->CheckMenuRadioItem(ID_LIB_PLAYLIST_SORT_RECENT_PLAYED, ID_LIB_PLAYLIST_SORT_NAME, ID_LIB_PLAYLIST_SORT_NAME, MF_BYCOMMAND | MF_CHECKED); break;
    }

}


void CSelectPlaylistDlg::OnNewPlaylist()
{
    // TODO: 在此添加命令处理程序代码
    OnBnClickedNewPlaylist();
}


void CSelectPlaylistDlg::OnEnChangeSearchEdit()
{
    // TODO:  如果该控件是 RICHEDIT 控件，它将不
    // 发送此通知，除非重写 CMediaLibTabDlg::OnInitDialog()
    // 函数并调用 CRichEditCtrl().SetEventMask()，
    // 同时将 ENM_CHANGE 标志“或”运算到掩码中。

    // TODO:  在此添加控件通知处理程序代码
    CString key_word;
    m_search_edit.GetWindowText(key_word);
    m_searcher_str = key_word;
    m_searched = !m_searcher_str.empty();
    ShowPathList();
}


//void CSelectPlaylistDlg::OnBnClickedClearButton()
//{
//    // TODO: 在此添加控件通知处理程序代码
//    if (m_searched)
//    {
//        //清除搜索结果
//        m_searched = false;
//        m_search_edit.SetWindowText(_T(""));
//        ShowPathList();
//        SetHighlightItem();
//    }
//}


BOOL CSelectPlaylistDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if (pMsg->message == WM_KEYDOWN && pMsg->hwnd != m_search_edit.GetSafeHwnd())
    {
        if (pMsg->wParam == 'F')    //按F键快速查找
        {
            m_search_edit.SetFocus();
            return TRUE;
        }
    }

    //if (pMsg->message == WM_MOUSEMOVE)
    //    m_Mytip.RelayEvent(pMsg);

    return CMediaLibTabDlg::PreTranslateMessage(pMsg);
}


afx_msg LRESULT CSelectPlaylistDlg::OnSearchEditBtnClicked(WPARAM wParam, LPARAM lParam)
{
    if (m_searched)
    {
        //清除搜索结果
        m_searched = false;
        m_search_edit.SetWindowText(_T(""));
        ShowPathList();
    }
    return 0;
}


void CSelectPlaylistDlg::OnNMClickSongList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    SongListClicked(pNMItemActivate->iItem);
    *pResult = 0;
}


void CSelectPlaylistDlg::OnNMRClickSongList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    SongListClicked(pNMItemActivate->iItem);
    m_selected_string = m_song_list_ctrl.GetItemText(pNMItemActivate->iItem, pNMItemActivate->iSubItem);

    if (!m_right_selected_items.empty())
    {
        //弹出右键菜单
        CMenu* pMenu = theApp.m_menu_mgr.GetMenu(MenuMgr::LibPlaylistRightMenu);
        ASSERT(pMenu != nullptr);
        if (pMenu != nullptr)
        {
            m_song_list_ctrl.ShowPopupMenu(pMenu, pNMItemActivate->iItem, this);
        }
    }

    *pResult = 0;
}


void CSelectPlaylistDlg::OnNMDblclkSongList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码

    if (m_right_selected_item >= 0 && m_right_selected_item < m_song_list_ctrl.GetItemCount())
        OnOK();
    *pResult = 0;
}


void CSelectPlaylistDlg::OnSaveAsNewPlaylist()
{
    // TODO: 在此添加命令处理程序代码
    PlaylistInfo playlist_info{ GetSelectedPlaylist() };
    wstring new_playlist_path = DoNewPlaylist();
    if (!new_playlist_path.empty())
    {
        CopyFile(playlist_info.path.c_str(), new_playlist_path.c_str(), FALSE);
        playlist_info.path = new_playlist_path;
        playlist_info.last_played_time = 0;
        CPlaylistMgr::Instance().UpdatePlaylistInfo(playlist_info);
        ShowPathList();
    }
}


void CSelectPlaylistDlg::OnPlaylistSaveAs()
{
    // TODO: 在此添加命令处理程序代码
    PlaylistInfo playlist_info{ GetSelectedPlaylist() };
    CMusicPlayerCmdHelper helper(this);
    helper.OnPlaylistSaveAs(playlist_info.path);
}


void CSelectPlaylistDlg::OnPlaylistFixPathError()
{
    CMusicPlayerCmdHelper helper(this);
    PlaylistInfo playlist_info{ GetSelectedPlaylist() };
    if (helper.OnPlaylistFixPathError(playlist_info.path))
    {
        ShowSongList();
    }
}


void CSelectPlaylistDlg::OnPlaylistBrowseFile()
{
    if (LeftSelectValid())
    {
        PlaylistInfo playlist_info{ GetSelectedPlaylist() };
        CString str;
        str.Format(_T("/select,\"%s\""), playlist_info.path.c_str());
        ShellExecute(NULL, _T("open"), _T("explorer"), str, NULL, SW_SHOWNORMAL);
    }
}


void CSelectPlaylistDlg::OnRemoveFromPlaylist()
{
    vector<SongInfo> songs_selected;
    GetSongsSelected(songs_selected);
    PlaylistInfo playlist_info{ GetSelectedPlaylist() };
    CMusicPlayerCmdHelper helper;
    if (helper.OnRemoveFromPlaylist(songs_selected, playlist_info.path))
    {
        ShowSongList();
    }
}


void CSelectPlaylistDlg::OnBnClickedSortButton()
{
    CWnd* pBtn = GetDlgItem(IDC_SORT_BUTTON);
    CPoint point;
    if (pBtn != nullptr)
    {
        CRect rect;
        pBtn->GetWindowRect(rect);
        point.x = rect.left;
        point.y = rect.bottom;
        CMenu* pMenu = theApp.m_menu_mgr.GetMenu(MenuMgr::LibPlaylistSortMenu);
        if (pMenu != NULL)
            pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
    }
}


void CSelectPlaylistDlg::OnLibPlaylistSortRecentPlayed()
{
    if (CPlaylistMgr::Instance().SetSortMode(CPlaylistMgr::SM_RECENT_PLAYED))
    {
        ShowPathList();
        theApp.m_pMainWnd->SendMessage(WM_INIT_ADD_TO_MENU);
    }
}


void CSelectPlaylistDlg::OnLibPlaylistSortRecentCreated()
{
    if (CPlaylistMgr::Instance().SetSortMode(CPlaylistMgr::SM_RECENT_CREATED))
    {
        ShowPathList();
        theApp.m_pMainWnd->SendMessage(WM_INIT_ADD_TO_MENU);
    }
}


void CSelectPlaylistDlg::OnLibPlaylistSortName()
{
    if (CPlaylistMgr::Instance().SetSortMode(CPlaylistMgr::SM_NAME))
    {
        ShowPathList();
        theApp.m_pMainWnd->SendMessage(WM_INIT_ADD_TO_MENU);
    }
}

void CSelectPlaylistDlg::OnLibPlaylistProperties()
{
    CPlaylistPropertiesDlg dlg(GetSelectedPlaylist());
    dlg.DoModal();
}
