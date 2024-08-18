// AllMediaDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "Player.h"
#include "AllMediaDlg.h"
#include "MusicPlayerCmdHelper.h"
#include "PropertyDlg.h"
#include "SongDataManager.h"


// CAllMediaDlg 对话框

IMPLEMENT_DYNAMIC(CAllMediaDlg, CMediaLibTabDlg)

CAllMediaDlg::CAllMediaDlg(CAllMediaDlg::DialogType type, CWnd* pParent /*=nullptr*/)
    : CMediaLibTabDlg(IDD_ALL_MEDIA_DIALOG, pParent), m_type{ type }
{

}

CAllMediaDlg::~CAllMediaDlg()
{
}

void CAllMediaDlg::RefreshData()
{
    InitListData();
    ShowSongList();
    m_initialized = true;
}

void CAllMediaDlg::RefreshSongList()
{
    for (int index : m_selected_items)
    {
        if (index >= 0 && index < static_cast<int>(m_list_data.size()))
        {
            SongInfo tmp = CSongDataManager::GetInstance().GetSongInfo3(m_list_songs[index]);
            std::swap(m_list_songs[index], tmp);
            SetRowData(m_list_data[index], m_list_songs[index]);
        }
    }
    ShowSongList();
}

void CAllMediaDlg::OnTabEntered()
{
    if (m_type == DT_ALL_MEDIA)
    {
        SetButtonsEnable(true);
        SetPlayButtonText(m_song_list_ctrl.GetCurSel() >= 0);
    }
    else
    {
        SetButtonsEnable(m_song_list_ctrl.GetCurSel() >= 0);
    }
    if (!m_initialized)
    {
        CWaitCursor wait_cursor;
        InitListData();
        ShowSongList();
        m_initialized = true;
    }
}

void CAllMediaDlg::OnTabExited()
{
    //切换到其他标签时将按钮恢复为“播放选中”
    if (m_type == DT_ALL_MEDIA)
    {
        CWnd* pParent = GetParentWindow();
        if (pParent != nullptr)
            pParent->SetDlgItemTextW(IDC_PLAY_SELECTED, theApp.m_str_table.LoadText(L"TXT_LIB_PLAY_SEL").c_str());
    }
}

void CAllMediaDlg::InitListData()
{
    m_list_data.clear();
    m_list_songs.clear();

    //获取当前系统时间
    SYSTEMTIME sys_time;
    GetLocalTime(&sys_time);
    __int64 cur_time = CTime(sys_time).GetTime();

    CSongDataManager::GetInstance().GetSongData([&](const CSongDataManager::SongDataMap& song_data_map)
        {
            for (const auto& item : song_data_map)
            {
                if (m_type == DT_RECENT_MEDIA)      //如果显示最近播放曲目，则跳过没有播放过的曲目
                {
                    if (item.second.last_played_time == 0)
                        continue;

                    //计算曲目上一次播放的时间和当前的时间差
                    __int64 time_span = cur_time - item.second.last_played_time;
                    //如果时间差超过了列表显示的范围，则跳过它
                    switch (theApp.m_media_lib_setting_data.recent_played_range)
                    {
                    case RPR_TODAY:
                        if (time_span > 24 * 3600)
                            continue;
                        break;
                    case RPR_THREE_DAYS:
                        if (time_span > 3 * 24 * 3600)
                            continue;
                        break;
                    case RPR_WEAK:
                        if (time_span > 7 * 24 * 3600)
                            continue;
                        break;
                    case RPR_MONTH:
                        if (time_span > 30 * 24 * 3600)
                            continue;
                        break;
                    case RPR_HALF_YEAR:
                        if (time_span > 180 * 24 * 3600)
                            continue;
                        break;
                    case RPR_YEAR:
                        if (time_span > 360 * 24 * 3600)
                            continue;
                        break;
                    default:
                        break;
                    }
                }
                m_list_songs.push_back(item.second);
            }
        });
    std::sort(m_list_songs.begin(), m_list_songs.end(), [&](const SongInfo& a, const SongInfo& b)
    {
        // 显示所有曲目时默认按标题排序，显示最近曲目时默认按最近播放时间排序
        if (m_type == DT_RECENT_MEDIA)
            return a.last_played_time > b.last_played_time;
        else
            return CCommon::StringCompareInLocalLanguage(a.title, b.title) < 0;
    });
    for (const SongInfo& tmp : m_list_songs)
    {
        CListCtrlEx::RowData row_data;
        SetRowData(row_data, tmp);
        m_list_data.push_back(std::move(row_data));
    }
    UpdateListIndex();
}

void CAllMediaDlg::SetRowData(CListCtrlEx::RowData& row_data, const SongInfo& song)
{
    row_data[COL_TITLE] = song.GetTitle();
    row_data[COL_ARTIST] = song.GetArtist();
    row_data[COL_ALBUM] = song.GetAlbum();
    std::wstring track_str;
    if (song.track != 0)
        track_str = std::to_wstring(song.track);
    row_data[COL_TRACK] = track_str;
    row_data[COL_GENRE] = song.GetGenre();
    row_data[COL_BITRATE] = (song.bitrate == 0 ? L"-" : std::to_wstring(song.bitrate));
    row_data[COL_YEAR] = song.GetYear();
    row_data[COL_PATH] = song.file_path;
    if (song.last_played_time != 0)
    {
        CTime played_time(song.last_played_time);
        wchar_t buff[64];
        swprintf_s(buff, L"%d/%.2d/%.2d %.2d:%.2d:%.2d", played_time.GetYear(), played_time.GetMonth(), played_time.GetDay(),
            played_time.GetHour(), played_time.GetMinute(), played_time.GetSecond());
        row_data[COL_LAST_PLAYED_TIME] = buff;
    }
}

void CAllMediaDlg::UpdateListIndex()
{
    int index{ 1 };
    for (auto& item : m_list_data)
    {
        item[COL_INDEX] = std::to_wstring(index);
        index++;
    }
}

void CAllMediaDlg::ShowSongList()
{
    CWaitCursor wait_cursor;
    if (m_searched)
        m_song_list_ctrl.SetListData(&m_list_data_searched);
    else
        m_song_list_ctrl.SetListData(&m_list_data);
}

void CAllMediaDlg::QuickSearch(const wstring& key_word)
{
    ASSERT(m_list_data.size() == m_list_songs.size());
    m_list_data_searched.clear();
    m_list_songs_searched.clear();

    for (int i{}; i < static_cast<int>(m_list_data.size()); ++i)
    {
        const auto& item{ m_list_data[i] };
        const vector<int> search_col{ COL_TITLE, COL_ARTIST, COL_ALBUM, COL_GENRE, COL_PATH };
        for (int col : search_col)
        {
            if (theApp.m_chinese_pingyin_res.IsStringMatchWithPingyin(key_word, item.at(col)))
            {
                m_list_data_searched.push_back(item);
                m_list_songs_searched.push_back(m_list_songs[i]);
                break;
            }
        }
    }
}

void CAllMediaDlg::SongListClicked(int index)
{
    m_selected_item = index;
    m_song_list_ctrl.GetItemSelected(m_selected_items);
    bool select_valid = !m_selected_items.empty();
    if (m_type == DT_ALL_MEDIA)
        SetPlayButtonText(select_valid);
    else
        SetButtonsEnable(select_valid);
}

void CAllMediaDlg::SetButtonsEnable(bool enable)
{
    CWnd* pParent = GetParentWindow();
    ::SendMessage(pParent->GetSafeHwnd(), WM_PLAY_SELECTED_BTN_ENABLE, WPARAM(enable), 0);
}

void CAllMediaDlg::SetPlayButtonText(bool selected_valid)
{
    CWnd* pParent = GetParentWindow();
    if (pParent != nullptr)
    {
        //选中了曲目时按钮文本为“播放选中”，否则为“播放”
        std::wstring text;
        if (selected_valid)
            text = theApp.m_str_table.LoadText(L"TXT_LIB_PLAY_SEL");
        else
            text = theApp.m_str_table.LoadText(L"UI_TIP_BTN_PLAY");
        pParent->SetDlgItemTextW(IDC_PLAY_SELECTED, text.c_str());
    }
}

const vector<SongInfo>& CAllMediaDlg::GetSongList() const
{
    if (m_searched)
        return m_list_songs_searched;
    else
        return m_list_songs;
}

int CAllMediaDlg::GetItemSelected() const
{
    return m_selected_item;
}

const vector<int>& CAllMediaDlg::GetItemsSelected() const
{
    return m_selected_items;
}

void CAllMediaDlg::AfterDeleteFromDisk(const std::vector<SongInfo>& songs)
{
    //删除成功，则刷新列表
    auto isRemoved_song = [&](const SongInfo& data)
    {
        for (const auto& item : songs)
        {
            if (item.IsSameSong(data))
                return true;
        }
        return false;
    };
    auto iter_removed_song = std::remove_if(m_list_songs.begin(), m_list_songs.end(), isRemoved_song);
    m_list_songs.erase(iter_removed_song, m_list_songs.end());
    auto iter_removed_song1 = std::remove_if(m_list_songs_searched.begin(), m_list_songs_searched.end(), isRemoved_song);
    m_list_songs_searched.erase(iter_removed_song1, m_list_songs_searched.end());

    auto isRemoved = [&](const CListCtrlEx::RowData& data)
    {
        for (const auto& item : songs)
        {
            if (item.file_path == data.at(COL_PATH) && item.track == std::stoi(data.at(COL_TRACK)))     // 这里若遇到特殊文件可能有潜在问题（音频内嵌cue）
                return true;
        }
        return false;
    };
    auto iter_removed = std::remove_if(m_list_data.begin(), m_list_data.end(), isRemoved);
    m_list_data.erase(iter_removed, m_list_data.end());
    auto iter_removed1 = std::remove_if(m_list_data_searched.begin(), m_list_data_searched.end(), isRemoved);
    m_list_data_searched.erase(iter_removed1, m_list_data_searched.end());
    ShowSongList();
}

wstring CAllMediaDlg::GetSelectedString() const
{
    return m_selected_string;
}

void CAllMediaDlg::DoDataExchange(CDataExchange* pDX)
{
    CMediaLibTabDlg::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_SONG_LIST, m_song_list_ctrl);
    DDX_Control(pDX, IDC_SEARCH_EDIT, m_search_edit);
}


void CAllMediaDlg::OnOK()
{
    //这里重写了基类CMediaLibTabDlg的OnOK函数
    //在“所有曲目”和“最近播放”标签中双击一项，会在“默认”播放列表中打开选中的曲目
    // 多选时OnOK会将选中项添加到临时播放列表内

    vector<SongInfo> songs;
    GetSongsSelected(songs);
    if (!songs.empty() || m_type == DT_ALL_MEDIA)
    {
        bool ok{};
        //所有曲目使用媒体库模式播放
        if (m_type == DT_ALL_MEDIA)
        {
            if (songs.empty())
                ok = CPlayer::GetInstance().SetMediaLibPlaylist(CMediaClassifier::CT_NONE, std::wstring());
            else
                ok = CPlayer::GetInstance().SetMediaLibPlaylist(CMediaClassifier::CT_NONE, std::wstring(), -1, songs.front(), true, true);
        }
        else
        {
            if (songs.size() == 1)
                ok = CPlayer::GetInstance().OpenSongsInDefaultPlaylist(songs);
            else
                ok = CPlayer::GetInstance().OpenSongsInTempPlaylist(songs);
        }
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

BEGIN_MESSAGE_MAP(CAllMediaDlg, CMediaLibTabDlg)
    ON_NOTIFY(HDN_ITEMCLICK, 0, &CAllMediaDlg::OnHdnItemclickSongList)
    ON_EN_CHANGE(IDC_SEARCH_EDIT, &CAllMediaDlg::OnEnChangeSearchEdit)
    ON_NOTIFY(NM_CLICK, IDC_SONG_LIST, &CAllMediaDlg::OnNMClickSongList)
    ON_NOTIFY(NM_RCLICK, IDC_SONG_LIST, &CAllMediaDlg::OnNMRClickSongList)
    ON_NOTIFY(NM_DBLCLK, IDC_SONG_LIST, &CAllMediaDlg::OnNMDblclkSongList)
    ON_MESSAGE(WM_SEARCH_EDIT_BTN_CLICKED, &CAllMediaDlg::OnSearchEditBtnClicked)
    ON_WM_INITMENU()
END_MESSAGE_MAP()


// CAllMediaDlg 消息处理程序


BOOL CAllMediaDlg::OnInitDialog()
{
    CMediaLibTabDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

    //初始化歌曲列表
    m_song_list_ctrl.SetExtendedStyle(m_song_list_ctrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_LABELTIP);
    m_song_list_ctrl.InsertColumn(0, theApp.m_str_table.LoadText(L"TXT_SERIAL_NUMBER").c_str(), LVCFMT_LEFT, theApp.DPI(40));
    m_song_list_ctrl.InsertColumn(1, theApp.m_str_table.LoadText(L"TXT_TITLE").c_str(), LVCFMT_LEFT, theApp.DPI(150));
    m_song_list_ctrl.InsertColumn(2, theApp.m_str_table.LoadText(L"TXT_ARTIST").c_str(), LVCFMT_LEFT, theApp.DPI(100));
    m_song_list_ctrl.InsertColumn(3, theApp.m_str_table.LoadText(L"TXT_ALBUM").c_str(), LVCFMT_LEFT, theApp.DPI(150));
    m_song_list_ctrl.InsertColumn(4, theApp.m_str_table.LoadText(L"TXT_TRACK_NUM").c_str(), LVCFMT_LEFT, theApp.DPI(60));
    m_song_list_ctrl.InsertColumn(5, theApp.m_str_table.LoadText(L"TXT_GENRE").c_str(), LVCFMT_LEFT, theApp.DPI(100));
    m_song_list_ctrl.InsertColumn(6, theApp.m_str_table.LoadText(L"TXT_BITRATE").c_str(), LVCFMT_LEFT, theApp.DPI(60));
    m_song_list_ctrl.InsertColumn(7, theApp.m_str_table.LoadText(L"TXT_YEAR").c_str(), LVCFMT_LEFT, theApp.DPI(60));
    m_song_list_ctrl.InsertColumn(8, theApp.m_str_table.LoadText(L"TXT_FILE_PATH").c_str(), LVCFMT_LEFT, theApp.DPI(600));
    m_song_list_ctrl.InsertColumn(9, theApp.m_str_table.LoadText(L"TXT_LAST_PLAYED_TIME").c_str(), LVCFMT_LEFT, theApp.DPI(140));
    m_song_list_ctrl.SetCtrlAEnable(true);

    m_search_edit.SetCueBanner(theApp.m_str_table.LoadText(L"TXT_SEARCH_PROMPT").c_str(), TRUE);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CAllMediaDlg::OnHdnItemclickSongList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    if (phdr->hdr.hwndFrom == m_song_list_ctrl.GetHeaderCtrl()->GetSafeHwnd())
    {
        static bool ascending = false;
        ascending = !ascending;

        static int last_item = -1;
        if (last_item != phdr->iItem)
        {
            last_item = phdr->iItem;
            ascending = true;
        }

        //对列表排序
        auto& list_songs{ m_searched ? m_list_songs_searched : m_list_songs };
        auto& list_data{ m_searched ? m_list_data_searched : m_list_data };

        if (phdr->iItem > COL_INDEX && phdr->iItem < COL_MAX)
        {
            auto so = CCommon::sort_permutation(list_songs, [&](const SongInfo& a, const SongInfo& b)
                {
                    if (ascending)
                    {
                        switch (phdr->iItem)
                        {
                        case COL_TITLE:
                            return SongInfo::ByTitle(a, b); break;
                        case COL_ARTIST:
                            return SongInfo::ByArtist(a, b); break;
                        case COL_ALBUM:
                            return SongInfo::ByAlbum(a, b); break;
                        case COL_TRACK:
                            return SongInfo::ByTrack(a, b); break;
                        case COL_GENRE:
                            return SongInfo::ByGenre(a, b); break;
                        case COL_BITRATE:
                            return SongInfo::ByBitrate(a, b); break;
                        case COL_YEAR:
                            return SongInfo::ByYear(a, b); break;
                        case COL_PATH:
                            return SongInfo::ByPath(a, b); break;
                        case COL_LAST_PLAYED_TIME:
                            return SongInfo::ByLastPlay(a, b); break;
                        default: 
                            return SongInfo::ByTitle(a, b); break;    // 默认按标题排序
                        }
                    }
                    else
                    {
                        switch (phdr->iItem)
                        {
                        case COL_TITLE:
                            return SongInfo::ByTitleDecending(a, b); break;
                        case COL_ARTIST:
                            return SongInfo::ByArtistDecending(a, b); break;
                        case COL_ALBUM:
                            return SongInfo::ByAlbumDecending(a, b); break;
                        case COL_TRACK:
                            return SongInfo::ByTrackDecending(a, b); break;
                        case COL_GENRE:
                            return SongInfo::ByGenreDecending(a, b); break;
                        case COL_BITRATE:
                            return SongInfo::ByBitrateDecending(a, b); break;
                        case COL_YEAR:
                            return SongInfo::ByYearDecending(a, b); break;
                        case COL_PATH:
                            return SongInfo::ByPathDecending(a, b); break;
                        case COL_LAST_PLAYED_TIME:
                            return SongInfo::ByLastPlayDecending(a, b); break;
                        default:
                            return SongInfo::ByTitleDecending(a, b); break;   // 默认按标题排序
                        }
                    }
                });
            list_data = CCommon::apply_permutation(list_data, so);
            list_songs = CCommon::apply_permutation(list_songs, so);
            if (!m_searched)
                UpdateListIndex();
            ShowSongList();
        }
    }

    *pResult = 0;
}


void CAllMediaDlg::OnEnChangeSearchEdit()
{
    // TODO:  如果该控件是 RICHEDIT 控件，它将不
    // 发送此通知，除非重写 CMediaLibTabDlg::OnInitDialog()
    // 函数并调用 CRichEditCtrl().SetEventMask()，
    // 同时将 ENM_CHANGE 标志“或”运算到掩码中。

    // TODO:  在此添加控件通知处理程序代码
    CString str;
    m_search_edit.GetWindowText(str);
    QuickSearch(wstring(str));
    m_searched = !str.IsEmpty();
    ShowSongList();
}


void CAllMediaDlg::OnNMClickSongList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    SongListClicked(pNMItemActivate->iItem);
    *pResult = 0;
}


void CAllMediaDlg::OnNMRClickSongList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    SongListClicked(pNMItemActivate->iItem);
    m_selected_string = m_song_list_ctrl.GetItemText(pNMItemActivate->iItem, pNMItemActivate->iSubItem);

    if (pNMItemActivate->iItem >= 0)
    {
        //弹出右键菜单
        CMenu* pMenu = theApp.m_menu_mgr.GetMenu(MenuMgr::LibRightMenu);
        ASSERT(pMenu != nullptr);
        if (pMenu != nullptr)
        {
            m_song_list_ctrl.ShowPopupMenu(pMenu, pNMItemActivate->iItem, this);
        }
    }
    *pResult = 0;
}


void CAllMediaDlg::OnNMDblclkSongList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    OnOK();
    *pResult = 0;
}


afx_msg LRESULT CAllMediaDlg::OnSearchEditBtnClicked(WPARAM wParam, LPARAM lParam)
{
    //点击搜索框中的叉按钮时清除搜索结果
    if (m_searched)
    {
        //清除搜索结果
        m_searched = false;
        m_search_edit.SetWindowText(_T(""));
        ShowSongList();
    }
    return 0;
}



void CAllMediaDlg::OnInitMenu(CMenu* pMenu)
{
    CMediaLibTabDlg::OnInitMenu(pMenu);

    //设置“添加到播放列表”子菜单状态
    //未选中状态不会弹出右键菜单，因此“添加到播放列表”子菜单全部设置为可用状态
    for (UINT id = ID_ADD_TO_DEFAULT_PLAYLIST; id < ID_ADD_TO_MY_FAVOURITE + ADD_TO_PLAYLIST_MAX_SIZE + 1; id++)
    {
        pMenu->EnableMenuItem(id, MF_BYCOMMAND | MF_ENABLED);
    }
    pMenu->EnableMenuItem(ID_ADD_TO_NEW_PLAYLIST, MF_BYCOMMAND | MF_ENABLED);
    pMenu->EnableMenuItem(ID_ADD_TO_OTHER_PLAYLIST, MF_BYCOMMAND | MF_ENABLED);
}
