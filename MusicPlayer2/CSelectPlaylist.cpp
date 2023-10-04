// CSelectPlaylist.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "CSelectPlaylist.h"
#include "afxdialogex.h"
#include "InputDlg.h"
#include "Playlist.h"
#include "SongDataManager.h"
#include "RecentFolderAndPlaylist.h"
#include "MusicPlayerCmdHelper.h"

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
    for (size_t i{ SPEC_PLAYLIST_NUM }; i < m_playlist_ctrl_data.size(); ++i)   // 跳过前两个特殊播放列表
    {
        // m_search_result存储m_playlist_ctrl_data的索引
        CFilePathHelper file_path{ m_playlist_ctrl_data[i].path };
        wstring playlist_name = file_path.GetFileNameWithoutExtension();
        if (CCommon::StringFindNoCase(playlist_name, key_words) != wstring::npos && file_path.GetFileName() != TEMP_PLAYLIST_NAME)
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

void CSelectPlaylistDlg::OnTabEntered()
{
    if (m_playlist_ctrl.GetCurSel() != -1)
        m_left_selected_item = m_playlist_ctrl.GetCurSel();     // m_left_selected_item直接存储m_playlist_ctrl的索引
    SetButtonsEnable();
}

void CSelectPlaylistDlg::ShowSongList()
{
    CWaitCursor wait_cursor;
    m_list_data.clear();
    int hight_item{ -1 };
    if (LeftSelectValid())
    {
        PlaylistInfo playlist_info = GetSelectedPlaylist();
        CPlaylistFile playlist_file;
        playlist_file.LoadFromFile(playlist_info.path);
        // 如果是当前播放列表则设置当前播放高亮
        if (CPlayer::GetInstance().IsPlaylistMode() && GetSelectedPlaylist().path == CPlayer::GetInstance().GetPlaylistPath())
            hight_item = playlist_file.GetSongIndexInPlaylist(CPlayer::GetInstance().GetCurrentSongInfo());
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
    m_song_list_ctrl.SetHightItem(hight_item);
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

    // ShowPathList初始化m_playlist_ctrl_data，需要在其他方法之前
    ShowPathList();
    SetLeftListSelected(GetPlayingItem());  // 初始化时选中正在播放的播放列表
    ShowSongList();
    m_search_edit.SetFocus();       //初始时将焦点设置到搜索框
    m_search_edit.SetCueBanner(CCommon::LoadText(IDS_SEARCH_HERE), TRUE);

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
            m_playlist_ctrl.SetItemText(0, 1, CCommon::LoadText(IDS_NO_RESULT_TO_SHOW));
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
            MessageBox(CCommon::LoadText(IDS_WAIT_AND_RETRY), NULL, MB_ICONINFORMATION | MB_OK);
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
    CInputDlg imput_dlg(this);
    imput_dlg.SetTitle(CCommon::LoadText(IDS_NEW_PLAYLIST));
    imput_dlg.SetInfoText(CCommon::LoadText(IDS_INPUT_PLAYLIST_NAME));
    if (imput_dlg.DoModal() == IDOK)
    {
        CString playlist_name = imput_dlg.GetEditText();
        if (playlist_name.IsEmpty())
        {
            MessageBox(CCommon::LoadText(IDS_PLAYLIST_NAME_EMPTY_WARNING), NULL, MB_ICONWARNING | MB_OK);
            return wstring();
        }
        if (!CCommon::IsFileNameValid(wstring(playlist_name.GetString())))
        {
            MessageBox(CCommon::LoadText(IDS_FILE_NAME_INVALID_WARNING), NULL, MB_ICONWARNING | MB_OK);
            return wstring();
        }
        wstring playlist_path = theApp.m_playlist_dir + playlist_name.GetString() + PLAYLIST_EXTENSION;
        if (CCommon::FileExist(playlist_path))
        {
            MessageBox(CCommon::LoadTextFormat(IDS_PLAYLIST_EXIST_WARNING, { playlist_name }), NULL, MB_ICONWARNING | MB_OK);
            return wstring();
        }

        CPlaylistMgr::Instance().AddNewPlaylist(playlist_path);
        ShowPathList();
        SetLeftListSelected(SPEC_PLAYLIST_NUM);     //选中新增的播放列表。添加新的播放列表后，新增的播放会排到前面，在特殊的播放列表的后一个位置
        CRecentFolderAndPlaylist::Instance().Init();
        return playlist_path;
    }
    return wstring();
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
    if (sel_playlist_path.empty()) return;
    CFilePathHelper old_path{ sel_playlist_path };
    wstring old_playlist_name{ old_path.GetFileName() };        // 确保不是特殊播放列表
    if (old_playlist_name == DEFAULT_PLAYLIST_NAME || old_playlist_name == FAVOURITE_PLAYLIST_NAME || old_playlist_name == TEMP_PLAYLIST_NAME) return;
    old_playlist_name = old_path.GetFileNameWithoutExtension(); // 获取播放列表名

    CInputDlg imput_dlg;
    imput_dlg.SetTitle(CCommon::LoadText(IDS_RENAME_PLAYLIST));
    imput_dlg.SetInfoText(CCommon::LoadText(IDS_INPUT_PLAYLIST_NAME));
    imput_dlg.SetEditText(old_playlist_name.c_str());

    if (imput_dlg.DoModal() == IDOK)
    {
        wstring new_playlist_name{ imput_dlg.GetEditText() };
        if (new_playlist_name.empty())
        {
            MessageBox(CCommon::LoadText(IDS_PLAYLIST_NAME_EMPTY_WARNING), NULL, MB_ICONWARNING | MB_OK);
            return;
        }
        if (!CCommon::IsFileNameValid(new_playlist_name))
        {
            MessageBox(CCommon::LoadText(IDS_FILE_NAME_INVALID_WARNING), NULL, MB_ICONWARNING | MB_OK);
            return;
        }
        if (CCommon::FileExist(theApp.m_playlist_dir + new_playlist_name + PLAYLIST_EXTENSION))
        {
            MessageBox(CCommon::LoadTextFormat(IDS_PLAYLIST_EXIST_WARNING, { new_playlist_name }), NULL, MB_ICONWARNING | MB_OK);
            return;
        }

        wstring new_path = CCommon::FileRename(sel_playlist_path, new_playlist_name);   //播放列表后命名后的路径
        if (new_path.empty())
        {
            MessageBox(CCommon::LoadText(IDS_REMANE_FAILED), NULL, MB_ICONWARNING | MB_OK);
            return;
        }
        if (sel_playlist_path == new_path)
            return;

        if (sel_playlist_path == CPlayer::GetInstance().GetPlaylistPath())          // 如果重命名的播放是当前播放的播放列表，就重新设置当前播放列表的路径
        {
            CPlayer::GetInstance().SetPlaylistPath(new_path);                       // 更新m_playlist_path变量
            theApp.m_pMainWnd->SendMessage(WM_CUR_PLAYLIST_RENAMED);                // 更新主窗口m_path_edit控件文字
        }
        CPlaylistMgr::Instance().RenamePlaylist(sel_playlist_path, new_path);
        CPlaylistMgr::Instance().SavePlaylistData();

        ShowPathList();
        CRecentFolderAndPlaylist::Instance().Init();
    }
}


void CSelectPlaylistDlg::OnDeletePlaylist()
{
    // TODO: 在此添加命令处理程序代码
    wstring del_path = GetSelectedPlaylist().path;
    if (del_path.empty())
        return;

    // 如果是当前播放那么使用CPlayer成员方法处理
    if (CPlayer::GetInstance().IsPlaylistMode() && CPlayer::GetInstance().GetPlaylistPath() == del_path)
    {
        if (!CPlayer::GetInstance().RemoveCurPlaylistOrFolder())
            MessageBox(CCommon::LoadText(IDS_WAIT_AND_RETRY), NULL, MB_ICONINFORMATION | MB_OK);
    }
    else
    {
        CPlaylistMgr::Instance().DeletePlaylist(del_path);
        CCommon::DeleteAFile(this->GetSafeHwnd(), del_path);
        CRecentFolderAndPlaylist::Instance().Init();
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
    CMenu* pContextMenu = theApp.m_menu_set.m_media_lib_playlist_menu.GetSubMenu(0);
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
    pMenu->EnableMenuItem(ID_RENAME_PLAYLIST, MF_BYCOMMAND | (!is_spec_playlist && !is_temp_playlist ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_DELETE_PLAYLIST, MF_BYCOMMAND | (!is_spec_playlist ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_PLAY_PLAYLIST, MF_BYCOMMAND | (SelectedCanPlay() ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_SAVE_AS_NEW_PLAYLIST, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_PLAYLIST_SAVE_AS, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_PLAYLIST_FIX_PATH_ERROR, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_PLAYLIST_BROWSE_FILE, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
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
        CMenu* pMenu = theApp.m_menu_set.m_media_lib_popup_menu.GetSubMenu(1);
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
    CFileDialog fileDlg(FALSE, _T("m3u"), CFilePathHelper(playlist_info.path).GetFileNameWithoutExtension().c_str(), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, CCommon::LoadText(IDS_SAVE_PLAYLIST_FILTER), this);
    if (IDOK == fileDlg.DoModal())
    {
        CPlaylistFile playlist;
        playlist.LoadFromFile(playlist_info.path);
        //将播放列表保存到文件
        wstring file_path{ fileDlg.GetPathName() };
        wstring file_extension{ fileDlg.GetFileExt() };
        file_extension = L'.' + file_extension;
        CPlaylistFile::Type file_type{};
        if (file_extension == PLAYLIST_EXTENSION)
            file_type = CPlaylistFile::PL_PLAYLIST;
        else if (file_extension == L".m3u")
            file_type = CPlaylistFile::PL_M3U;
        else if (file_extension == L".m3u8")
            file_type = CPlaylistFile::PL_M3U8;
        playlist.SaveToFile(file_path, file_type);
    }

}


void CSelectPlaylistDlg::OnPlaylistFixPathError()
{
    if (LeftSelectValid())
    {
        if (MessageBox(CCommon::LoadText(IDS_PLAYLIST_FIX_PATH_ERROR_INFO), NULL, MB_ICONQUESTION | MB_YESNO) == IDYES)
        {
            PlaylistInfo playlist_info{ GetSelectedPlaylist() };
            CMusicPlayerCmdHelper helper;
            int fixed_count = helper.FixPlaylistPathError(playlist_info.path);
            ShowSongList();
            MessageBox(CCommon::LoadTextFormat(IDS_PLAYLIST_FIX_PATH_ERROR_COMPLETE, { fixed_count }), NULL, MB_ICONINFORMATION | MB_OK);
        }
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
