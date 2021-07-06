// CSelectPlaylist.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "CSelectPlaylist.h"
#include "afxdialogex.h"
#include "InputDlg.h"
#include "Playlist.h"
#include "SongDataManager.h"
#include "COSUPlayerHelper.h"


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
    ShowPathList();     // 对播放列表列表刷新特别处理，刷新左侧列表
    ShowSongList();
}

wstring CSelectPlaylistDlg::GetSelPlaylistPath() const
{
    return GetSelectedPlaylist().path;
}

int CSelectPlaylistDlg::GetTrack() const
{
    return GetSelectedPlaylist().track;
}

int CSelectPlaylistDlg::GetPosition() const
{
    return GetSelectedPlaylist().position;
}

bool CSelectPlaylistDlg::IsPlaylistModified() const
{
    return m_playlist_modified;
}

void CSelectPlaylistDlg::AdjustColumnWidth()
{
    vector<int> width;
    CalculateColumeWidth(width);
    for (size_t i{}; i < width.size(); i++)
        m_playlist_ctrl.SetColumnWidth(i, width[i]);
}

bool CSelectPlaylistDlg::IsLeftSelected() const
{
    return m_left_selected;
}

void CSelectPlaylistDlg::DoDataExchange(CDataExchange* pDX)
{
    CMediaLibTabDlg::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_playlist_ctrl);
    DDX_Control(pDX, IDC_SEARCH_EDIT, m_search_edit);
    DDX_Control(pDX, IDC_SONG_LIST, m_song_list_ctrl);
}


void CSelectPlaylistDlg::QuickSearch(const wstring& key_words)
{
    m_search_result.clear();
    const auto& recent_playlists = CPlayer::GetInstance().GetRecentPlaylist().m_recent_playlists;
    for (size_t i{}; i < recent_playlists.size(); i++)
    {
        CFilePathHelper file_path{ recent_playlists[i].path };
        wstring playlist_name = file_path.GetFileNameWithoutExtension();
        if (CCommon::StringFindNoCase(playlist_name, key_words) != wstring::npos)
            m_search_result.push_back(i);
    }
}

int CSelectPlaylistDlg::GetPlayingItem()
{
    //正在播放的项目
    int playing_item{ -1 };
    if (CPlayer::GetInstance().IsPlaylistMode() && !m_searched)
    {
        if (CPlayer::GetInstance().GetRecentPlaylist().m_cur_playlist_type == PT_DEFAULT)
        {
            playing_item = 0;
        }
        else if (CPlayer::GetInstance().GetRecentPlaylist().m_cur_playlist_type == PT_FAVOURITE)
        {
            playing_item = 1;
        }
        else
        {
            auto& recent_playlist{ CPlayer::GetInstance().GetRecentPlaylist().m_recent_playlists };
            wstring current_playlist{ CPlayer::GetInstance().GetPlaylistPath() };
            auto iter = std::find_if(recent_playlist.begin(), recent_playlist.end(), [current_playlist](const PlaylistInfo& playlist_info)
            {
                return current_playlist == playlist_info.path;
            });
            playing_item = iter - recent_playlist.begin() + SPEC_PLAYLIST_NUM;
        }
    }
    return playing_item;
}

void CSelectPlaylistDlg::OnTabEntered()
{
    if(m_playlist_ctrl.GetCurSel() != -1)
        m_row_selected = m_playlist_ctrl.GetCurSel();
    SetButtonsEnable();
}

void CSelectPlaylistDlg::ShowSongList()
{
    CWaitCursor wait_cursor;
    m_list_data.clear();
    m_song_list_ctrl.SetHightItem(-1);
    if (m_row_selected >= 0)
    {
        wstring playlist_path = GetSelPlaylistPath();
        CPlaylistFile playlist_file;
        playlist_file.LoadFromFile(playlist_path);
        m_cur_song_list = std::move(playlist_file.GetPlaylist());
        int index{};
        for (SongInfo& song : m_cur_song_list)
        {
            if (!song.info_acquired)
            {
                song.CopySongInfo(CSongDataManager::GetInstance().GetSongInfo(song.file_path));
            }
            CListCtrlEx::RowData row_data;
            row_data[COL_INDEX] = std::to_wstring(index + 1);
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

            if (CPlayer::GetInstance().IsPlaylistMode() && song.IsSameSong(CPlayer::GetInstance().GetCurrentSongInfo()) && GetSelectedPlaylist().path == CPlayer::GetInstance().GetPlaylistPath())
                m_song_list_ctrl.SetHightItem(index);

            index++;
        }
    }
    m_song_list_ctrl.SetListData(&m_list_data);
}

void CSelectPlaylistDlg::LeftListClicked(int index)
{
    m_left_selected = true;
    if (!m_searched)
    {
        m_row_selected = index;
    }
    else
    {
        CString str;
        str = m_playlist_ctrl.GetItemText(index, 0);
        m_row_selected = _ttoi(str) - 1;
    }
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

const CListCtrlEx& CSelectPlaylistDlg::GetSongListCtrl() const
{
    return m_song_list_ctrl;
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

int CSelectPlaylistDlg::GetPathColIndex() const
{
    return COL_PATH;
}

wstring CSelectPlaylistDlg::GetSelectedString() const
{
    return wstring(m_selected_string);
}

void CSelectPlaylistDlg::GetSongsSelected(std::vector<SongInfo>& song_list) const
{
    for (int index : GetItemsSelected())
    {
        if (index < 0 || index >= static_cast<int>(m_cur_song_list.size()))
            continue;
        song_list.push_back(m_cur_song_list[index]);
    }
}

void CSelectPlaylistDlg::GetCurrentSongList(std::vector<SongInfo>& song_list) const
{
    song_list = m_cur_song_list;
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
END_MESSAGE_MAP()


// CSelectPlaylist 消息处理程序


BOOL CSelectPlaylistDlg::OnInitDialog()
{
    CMediaLibTabDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

    CButton* new_btn = (CButton*)(GetDlgItem(IDC_NEW_PLAYLIST));
    if (new_btn != nullptr)
        new_btn->SetIcon(theApp.m_icon_set.add.GetIcon(true));


    //初始化播放列表控件
    vector<int> width;
    CalculateColumeWidth(width);
    m_playlist_ctrl.SetExtendedStyle(m_playlist_ctrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_LABELTIP);
    m_playlist_ctrl.InsertColumn(0, CCommon::LoadText(IDS_NUMBER), LVCFMT_LEFT, width[0]);
    m_playlist_ctrl.InsertColumn(1, CCommon::LoadText(IDS_PLAYLIST), LVCFMT_LEFT, width[1]);
    m_playlist_ctrl.InsertColumn(2, CCommon::LoadText(IDS_TRACK_PLAYED), LVCFMT_LEFT, width[2]);
    m_playlist_ctrl.InsertColumn(3, CCommon::LoadText(IDS_TRACK_TOTAL_NUM), LVCFMT_LEFT, width[3]);
    m_playlist_ctrl.InsertColumn(4, CCommon::LoadText(IDS_TOTAL_LENGTH), LVCFMT_LEFT, width[4]);

    ////初始化提示信息
    //m_Mytip.Create(this, TTS_ALWAYSTIP);
    //m_Mytip.AddTool(GetDlgItem(IDC_CLEAR_BUTTON), CCommon::LoadText(IDS_CLEAR_SEARCH_RESULT));
    //m_Mytip.AddTool(&m_search_edit, CCommon::LoadText(IDS_INPUT_KEY_WORD));

    ////设置列表控件的提示总是置顶，用于解决如果弹出此窗口的父窗口具有置顶属性时，提示信息在窗口下面的问题
    //m_playlist_ctrl.GetToolTips()->SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    //m_Mytip.SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    //初始化右侧列表
    m_song_list_ctrl.SetExtendedStyle(m_song_list_ctrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_LABELTIP);
    m_song_list_ctrl.InsertColumn(COL_INDEX, CCommon::LoadText(IDS_NUMBER), LVCFMT_LEFT, theApp.DPI(40));
    m_song_list_ctrl.InsertColumn(COL_TITLE, CCommon::LoadText(IDS_TITLE), LVCFMT_LEFT, theApp.DPI(150));
    m_song_list_ctrl.InsertColumn(COL_ARTIST, CCommon::LoadText(IDS_ARTIST), LVCFMT_LEFT, theApp.DPI(100));
    m_song_list_ctrl.InsertColumn(COL_ALBUM, CCommon::LoadText(IDS_ALBUM), LVCFMT_LEFT, theApp.DPI(150));
    m_song_list_ctrl.InsertColumn(COL_TRACK, CCommon::LoadText(IDS_TRACK_NUM), LVCFMT_LEFT, theApp.DPI(60));
    m_song_list_ctrl.InsertColumn(COL_GENRE, CCommon::LoadText(IDS_GENRE), LVCFMT_LEFT, theApp.DPI(100));
    m_song_list_ctrl.InsertColumn(COL_BITRATE, CCommon::LoadText(IDS_BITRATE), LVCFMT_LEFT, theApp.DPI(60));
    m_song_list_ctrl.InsertColumn(COL_PATH, CCommon::LoadText(IDS_FILE_PATH), LVCFMT_LEFT, theApp.DPI(600));
    m_song_list_ctrl.SetCtrlAEnable(true);

    m_row_selected = GetPlayingItem(); // 初始化时选中正在播放的播放列表
    ShowPathList();
    ShowSongList();
    m_search_edit.SetFocus();		//初始时将焦点设置到搜索框
    m_search_edit.SetCueBanner(CCommon::LoadText(IDS_SEARCH_HERE), TRUE);

    return FALSE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}

void CSelectPlaylistDlg::CalculateColumeWidth(vector<int>& width)
{
    CRect rect;
    m_playlist_ctrl.GetWindowRect(rect);
    width.resize(5);

    width[2] = width[3] = rect.Width() / 8;
    width[4] = rect.Width() / 6;

    width[0] = theApp.DPI(40);
    width[1] = rect.Width() - width[2] - width[3] - width[4] - width[0] - theApp.DPI(20) - 1;

}

void CSelectPlaylistDlg::ShowPathList()
{
    //m_playlist_ctrl.DeleteAllItems();
    //m_playlist_ctrl.InsertItem(0, _T("1"));
    //SetListRowData(0, CPlayer::GetInstance().GetRecentPlaylist().m_default_playlist);
    //m_playlist_ctrl.InsertItem(1, _T("2"));
    //SetListRowData(1, CPlayer::GetInstance().GetRecentPlaylist().m_favourite_playlist);
    //auto& recent_playlists = CPlayer::GetInstance().GetRecentPlaylist().m_recent_playlists;
    //int index = 2;
    //for (const auto& playlist_info : recent_playlists)
    //{
    //    CString str;
    //    str.Format(_T("%d"), index + 1);
    //    m_playlist_ctrl.InsertItem(index, str);
    //    SetListRowData(index, playlist_info);
    //    index++;
    //}

    const auto& recent_playlists = CPlayer::GetInstance().GetRecentPlaylist().m_recent_playlists;
    m_playlist_ctrl.EnableWindow(TRUE);
    if (!m_searched)		//显示所有项目
    {
        m_playlist_ctrl.DeleteAllItems();
        for (int i{}; i < SPEC_PLAYLIST_NUM; i++)
        {
            CString str;
            str.Format(_T("%d"), i + 1);
            m_playlist_ctrl.InsertItem(i, str);
        }
        SetListRowData(0, CPlayer::GetInstance().GetRecentPlaylist().m_default_playlist);
        SetListRowData(1, CPlayer::GetInstance().GetRecentPlaylist().m_favourite_playlist);
        int index = SPEC_PLAYLIST_NUM;
        for (const auto& playlist_info : recent_playlists)
        {
            CString str;
            str.Format(_T("%d"), index + 1);
            m_playlist_ctrl.InsertItem(index, str);
            SetListRowData(index, playlist_info);
            index++;
        }
        //最后一行显示临时的播放列表，只有当列表中有歌曲时才显示
        if(CPlayer::GetInstance().GetRecentPlaylist().m_temp_playlist.track_num > 0)
        {
            m_playlist_ctrl.InsertItem(index, std::to_wstring(index + 1).c_str());
            SetListRowData(index, CPlayer::GetInstance().GetRecentPlaylist().m_temp_playlist);
        }
    }
    else		//只显示搜索结果的曲目
    {
        if (m_search_result.empty())
        {
            m_playlist_ctrl.DeleteAllItems();
            m_playlist_ctrl.InsertItem(0, _T(""));
            m_playlist_ctrl.SetItemText(0, 1, CCommon::LoadText(IDS_NO_RESULT_TO_SHOW));
            m_playlist_ctrl.EnableWindow(FALSE);
            return;
        }

        int item_num_before = m_playlist_ctrl.GetItemCount();
        int item_num_after = m_search_result.size();
        //如果当前列表中项目的数量小于原来的，则直接清空原来列表中所有的项目，重新添加
        if (item_num_after < item_num_before)
        {
            m_playlist_ctrl.DeleteAllItems();
            item_num_before = 0;
        }
        CString str;
        for (int i{}; i < item_num_after; i++)
        {
            str.Format(_T("%u"), m_search_result[i] + SPEC_PLAYLIST_NUM + 1);
            if (i >= item_num_before)	//如果当前列表中的项目数量大于之前的数量，则需要在不够时插入新的项目
            {
                m_playlist_ctrl.InsertItem(i, str);
            }
            m_playlist_ctrl.SetItemText(i, 0, str);
            SetListRowData(i, recent_playlists[m_search_result[i]]);
        }
    }
    m_playlist_ctrl.SetHightItem(GetPlayingItem());
}

void CSelectPlaylistDlg::SetListRowData(int index, const PlaylistInfo& playlist_info)
{
    CFilePathHelper path_helper{ playlist_info.path };
    wstring playlist_name = path_helper.GetFileName();
    if (playlist_name == DEFAULT_PLAYLIST_NAME)
        playlist_name = CCommon::LoadText(_T("["), IDS_DEFAULT, _T("]"));
    else if (playlist_name == FAVOURITE_PLAYLIST_NAME)
        playlist_name = CCommon::LoadText(_T("["), IDS_MY_FAVURITE, _T("]"));
    else if (playlist_name == TEMP_PLAYLIST_NAME)
        playlist_name = CCommon::LoadText(_T("["), IDS_TEMP_PLAYLIST, _T("]"));
    else
        playlist_name = path_helper.GetFileNameWithoutExtension();

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


bool CSelectPlaylistDlg::SelectValid() const
{
    if (m_row_selected == 0 || m_row_selected == 1)
        return true;
    int index = m_row_selected - SPEC_PLAYLIST_NUM;
    int playlists_size{ static_cast<int>(CPlayer::GetInstance().GetRecentPlaylist().m_recent_playlists.size()) };
    // 对一般播放列表返回true，对临时播放列表仅当列表不为空时返回true
    return ((index >= 0 && index < playlists_size) ||
        (index == playlists_size && CPlayer::GetInstance().GetRecentPlaylist().m_temp_playlist.track_num > 0));
}

PlaylistInfo CSelectPlaylistDlg::GetSelectedPlaylist() const
{
    if (m_row_selected == 0)
        return CPlayer::GetInstance().GetRecentPlaylist().m_default_playlist;
    else if (m_row_selected == 1)
        return CPlayer::GetInstance().GetRecentPlaylist().m_favourite_playlist;
    int index = m_row_selected - SPEC_PLAYLIST_NUM;
    if (index >= 0 && index < static_cast<int>(CPlayer::GetInstance().GetRecentPlaylist().m_recent_playlists.size()))
        return CPlayer::GetInstance().GetRecentPlaylist().m_recent_playlists[index];
    else
        return CPlayer::GetInstance().GetRecentPlaylist().m_temp_playlist;
}

void CSelectPlaylistDlg::SetButtonsEnable()
{
    bool enable = SelectedCanPlay();
    CWnd* pParent = GetParentWindow();
    ::SendMessage(pParent->GetSafeHwnd(), WM_PLAY_SELECTED_BTN_ENABLE, WPARAM(enable), 0);
}

bool CSelectPlaylistDlg::SelectedCanPlay() const
{
    return SelectValid() && 
        (
            !CPlayer::GetInstance().IsPlaylistMode() ||
            GetSelectedPlaylist().path != CPlayer::GetInstance().GetPlaylistPath() ||
            (m_right_selected_item >= 0 && GetSelectedPlaylist().track != m_right_selected_item)
        );
}

void CSelectPlaylistDlg::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    if(!m_searched)
    {
        m_row_selected = pNMItemActivate->iItem;
    }
    else
    {
        CString str;
        str = m_playlist_ctrl.GetItemText(pNMItemActivate->iItem, 0);
        m_row_selected = _ttoi(str) - 1;
    }
    OnOK();

    *pResult = 0;
}


void CSelectPlaylistDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类

    if(SelectedCanPlay())
    {
        int index = -1;
        if (!m_left_selected)
        {
            index = m_right_selected_item;
        }
        ::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_PLAYLIST_SELECTED, (WPARAM)this, (LPARAM)index);

        CTabDlg::OnOK();

        CWnd* pParent = GetParentWindow();
        if (pParent != nullptr)
            ::SendMessage(pParent->GetSafeHwnd(), WM_COMMAND, IDOK, 0);
    }

}


void CSelectPlaylistDlg::OnBnClickedNewPlaylist()
{
    // TODO: 在此添加控件通知处理程序代码
    CInputDlg imput_dlg(this);
    imput_dlg.SetTitle(CCommon::LoadText(IDS_NEW_PLAYLIST));
    imput_dlg.SetInfoText(CCommon::LoadText(IDS_INPUT_PLAYLIST_NAME));
    if (imput_dlg.DoModal() == IDOK)
    {
        CString playlist_name = imput_dlg.GetEditText();
        if (playlist_name.IsEmpty())
        {
            MessageBox(CCommon::LoadText(IDS_PLAYLIST_NAME_EMPTY_WARNING), NULL, MB_ICONWARNING | MB_OK);
            return;
        }
        if (!CCommon::IsFileNameValid(wstring(playlist_name.GetString())))
        {
            MessageBox(CCommon::LoadText(IDS_FILE_NAME_INVALID_WARNING), NULL, MB_ICONWARNING | MB_OK);
            return;
        }
        wstring playlist_path = theApp.m_playlist_dir + playlist_name.GetString() + PLAYLIST_EXTENSION;
        if (CCommon::FileExist(playlist_path))
        {
            MessageBox(CCommon::LoadTextFormat(IDS_PLAYLIST_EXIST_WARNING, { playlist_name }), NULL, MB_ICONWARNING | MB_OK);
            return;
        }

        CPlayer::GetInstance().GetRecentPlaylist().AddNewPlaylist(playlist_path);
        ShowPathList();
        m_playlist_modified = true;
    }
}


void CSelectPlaylistDlg::OnPlayPlaylist()
{
    // TODO: 在此添加命令处理程序代码
    ::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_PLAYLIST_SELECTED, (WPARAM)this, -1);
}


void CSelectPlaylistDlg::OnRenamePlaylist()
{
    // TODO: 在此添加命令处理程序代码
    CInputDlg imput_dlg;
    imput_dlg.SetTitle(CCommon::LoadText(IDS_RENAME_PLAYLIST));
    imput_dlg.SetInfoText(CCommon::LoadText(IDS_INPUT_PLAYLIST_NAME));

    CString old_playlist_name = m_playlist_ctrl.GetItemText(m_row_selected, 1);
    imput_dlg.SetEditText(old_playlist_name);

    if (imput_dlg.DoModal() == IDOK)
    {
        CString playlist_name = imput_dlg.GetEditText();
        if (playlist_name.IsEmpty())
        {
            MessageBox(CCommon::LoadText(IDS_PLAYLIST_NAME_EMPTY_WARNING), NULL, MB_ICONWARNING | MB_OK);
            return;
        }
        if (!CCommon::IsFileNameValid(wstring(playlist_name.GetString())))
        {
            MessageBox(CCommon::LoadText(IDS_FILE_NAME_INVALID_WARNING), NULL, MB_ICONWARNING | MB_OK);
            return;
        }

        if (CCommon::FileExist(theApp.m_playlist_dir + playlist_name.GetString() + PLAYLIST_EXTENSION))
        {
            MessageBox(CCommon::LoadTextFormat(IDS_PLAYLIST_EXIST_WARNING, { playlist_name }), NULL, MB_ICONWARNING | MB_OK);
            return;
        }

        int index = m_row_selected - SPEC_PLAYLIST_NUM;
        if (index >= 0 && index < static_cast<int>(CPlayer::GetInstance().GetRecentPlaylist().m_recent_playlists.size()))
        {
            wstring playlist_path = CPlayer::GetInstance().GetRecentPlaylist().m_recent_playlists[index].path;      //播放列表重命名前的路径
            wstring new_path = CCommon::FileRename(playlist_path, wstring(playlist_name));                          //播放列表后命名后的路径

            if (new_path.empty())
            {
                MessageBox(CCommon::LoadText(IDS_REMANE_FAILED), NULL, MB_ICONWARNING | MB_OK);
                return;
            }

            if (playlist_path == new_path)
                return;

            CPlayer::GetInstance().GetRecentPlaylist().m_recent_playlists[index].path = new_path;

            if (playlist_path == CPlayer::GetInstance().GetPlaylistPath())          //如果重命名的播放是当前播放的播放列表，就重新设置当前播放列表的路径
            {
                CPlayer::GetInstance().SetPlaylistPath(new_path);
                theApp.m_pMainWnd->SendMessage(WM_CUR_PLAYLIST_RENAMED);
            }
            CPlayer::GetInstance().GetRecentPlaylist().SavePlaylistData();
        }

        ShowPathList();
        m_playlist_modified = true;
    }
}


void CSelectPlaylistDlg::OnDeletePlaylist()
{
    // TODO: 在此添加命令处理程序代码
    int index{ m_row_selected - SPEC_PLAYLIST_NUM };
    int playlists_size{ static_cast<int>(CPlayer::GetInstance().GetRecentPlaylist().m_recent_playlists.size()) };
    if (index >= 0 && index < playlists_size)
    {
        wstring playlist_path = CPlayer::GetInstance().GetRecentPlaylist().m_recent_playlists[index].path;
        if (playlist_path == CPlayer::GetInstance().GetPlaylistPath())      //如果删除的是正在播放的播放列表，则播放默认播放列表
        {
            ::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_PLAYLIST_SELECTED, (WPARAM)this, -2);
        }
        CPlayer::GetInstance().GetRecentPlaylist().DeletePlaylist(playlist_path);
        CCommon::DeleteAFile(this->GetSafeHwnd(), playlist_path);
        ShowPathList();
        m_playlist_modified = true;
    }
    else if (index == playlists_size)           // 删除的是临时播放列表
    {
        if (CPlayer::GetInstance().GetRecentPlaylist().m_cur_playlist_type == PT_TEMP)
        {
            ::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_PLAYLIST_SELECTED, (WPARAM)this, -2);
        }
        CPlayer::GetInstance().GetRecentPlaylist().m_temp_playlist.track_num = 0;
        wstring playlist_path{ CPlayer::GetInstance().GetRecentPlaylist().m_temp_playlist.path };
        CCommon::DeleteAFile(this->GetSafeHwnd(), playlist_path);
        ShowPathList();
        m_playlist_modified = true;
    }
}


void CSelectPlaylistDlg::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    LeftListClicked(pNMItemActivate->iItem);

    *pResult = 0;
}


void CSelectPlaylistDlg::OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    LeftListClicked(pNMItemActivate->iItem);
    m_selected_string = m_playlist_ctrl.GetItemText(pNMItemActivate->iItem, pNMItemActivate->iSubItem);

    //弹出右键菜单
    CMenu* pContextMenu = theApp.m_menu_set.m_media_lib_playlist_menu.GetSubMenu(0);
    m_playlist_ctrl.ShowPopupMenu(pContextMenu, pNMItemActivate->iItem, this);

    *pResult = 0;
}


void CSelectPlaylistDlg::OnInitMenu(CMenu* pMenu)
{
    CMediaLibTabDlg::OnInitMenu(pMenu);

    // TODO: 在此处添加消息处理程序代码
    int playlists_size{ static_cast<int>(CPlayer::GetInstance().GetRecentPlaylist().m_recent_playlists.size()) + SPEC_PLAYLIST_NUM };
    bool is_not_default_playlist{ m_row_selected > 1 && m_row_selected < playlists_size };
    bool is_tmp_playlist{ m_row_selected == playlists_size };
    pMenu->EnableMenuItem(ID_RENAME_PLAYLIST, MF_BYCOMMAND | (is_not_default_playlist ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_DELETE_PLAYLIST, MF_BYCOMMAND | (is_not_default_playlist || is_tmp_playlist ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_PLAY_PLAYLIST, MF_BYCOMMAND | (SelectedCanPlay() ? MF_ENABLED : MF_GRAYED));

    bool is_delete_enable{ !m_left_selected && m_right_selected_item >= 0 && m_right_selected_item < static_cast<int>(m_cur_song_list.size())
        && !m_cur_song_list[m_right_selected_item].is_cue && !COSUPlayerHelper::IsOsuFile(m_cur_song_list[m_right_selected_item].file_path) };
    pMenu->EnableMenuItem(ID_DELETE_FROM_DISK, MF_BYCOMMAND | (is_delete_enable ? MF_ENABLED : MF_GRAYED));
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
    m_searched = (key_word.GetLength() != 0);
    QuickSearch(wstring(key_word));
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
        if (pMsg->wParam == 'F')	//按F键快速查找
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


void CSelectPlaylistDlg::OnNMClickSongList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    SongListClicked(pNMItemActivate->iItem);
    *pResult = 0;
}


void CSelectPlaylistDlg::OnNMRClickSongList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    SongListClicked(pNMItemActivate->iItem);
    m_selected_string = m_song_list_ctrl.GetItemText(pNMItemActivate->iItem, pNMItemActivate->iSubItem);

    if (!m_right_selected_items.empty())
    {
        //弹出右键菜单
        CMenu* pMenu = theApp.m_menu_set.m_media_lib_popup_menu.GetSubMenu(1);
        ASSERT(pMenu != nullptr);
        if (pMenu != nullptr)
        {
            m_song_list_ctrl.ShowPopupMenu(pMenu, pNMItemActivate->iItem, this);
        }
    }

    *pResult = 0;
}


void CSelectPlaylistDlg::OnNMDblclkSongList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码

    if (m_right_selected_item >= 0 && m_right_selected_item < m_song_list_ctrl.GetItemCount())
        OnOK();
    *pResult = 0;
}


void CSelectPlaylistDlg::OnCancel()
{
    // TODO: 在此添加专用代码和/或调用基类

    CTabDlg::OnCancel();

    CWnd* pParent = GetParentWindow();
    if (pParent != nullptr)
        ::SendMessage(pParent->GetSafeHwnd(), WM_COMMAND, IDCANCEL, 0);
}
