#include "stdafx.h"
#include "MediaLibTabDlg.h"
#include "MusicPlayer2.h"
#include "Player.h"
#include "MusicPlayerCmdHelper.h"
#include "PropertyDlg.h"
#include "SongDataManager.h"
#include "COSUPlayerHelper.h"


IMPLEMENT_DYNAMIC(CMediaLibTabDlg, CTabDlg)

CMediaLibTabDlg::CMediaLibTabDlg(UINT nIDTemplate, CWnd *pParent)
    : CTabDlg(nIDTemplate, pParent)
{
}


CMediaLibTabDlg::~CMediaLibTabDlg()
{
}

BEGIN_MESSAGE_MAP(CMediaLibTabDlg, CTabDlg)
    ON_WM_INITMENU()
    ON_COMMAND(ID_PLAY_ITEM, &CMediaLibTabDlg::OnPlayItem)
    ON_COMMAND(ID_PLAY_ITEM_IN_FOLDER_MODE, &CMediaLibTabDlg::OnPlayItemInFolderMode)
    ON_COMMAND(ID_ADD_TO_NEW_PLAYLIST, &CMediaLibTabDlg::OnAddToNewPlaylist)
    ON_COMMAND(ID_ADD_TO_NEW_PLAYLIST_AND_PLAY, &CMediaLibTabDlg::OnAddToNewPlaylistAndPlay)
    ON_COMMAND(ID_EXPLORE_ONLINE, &CMediaLibTabDlg::OnExploreOnline)
    ON_COMMAND(ID_EXPLORE_TRACK, &CMediaLibTabDlg::OnExploreTrack)
    ON_COMMAND(ID_FORMAT_CONVERT, &CMediaLibTabDlg::OnFormatConvert)
    ON_COMMAND(ID_DELETE_FROM_DISK, &CMediaLibTabDlg::OnDeleteFromDisk)
    ON_COMMAND(ID_ITEM_PROPERTY, &CMediaLibTabDlg::OnItemProperty)
    ON_COMMAND(ID_COPY_TEXT, &CMediaLibTabDlg::OnCopyText)
    ON_COMMAND(ID_PLAY_AS_NEXT, &CMediaLibTabDlg::OnPlayAsNext)
END_MESSAGE_MAP()


void CMediaLibTabDlg::GetSongsSelected(std::vector<SongInfo>& song_list) const
{
    song_list.clear();
    const vector<SongInfo>& o_song_list{ GetSongList() };
    for (int index : GetItemsSelected())
    {
        if (index < 0 || index > static_cast<int>(o_song_list.size()))
            continue;
        song_list.push_back(o_song_list[index]);
    }
}

void CMediaLibTabDlg::OnTabEntered()
{
    //默认将“播放选中”禁用
    CWnd* pParent = GetParentWindow();
    ::SendMessage(pParent->GetSafeHwnd(), WM_PLAY_SELECTED_BTN_ENABLE, 0, 0);
}

bool CMediaLibTabDlg::_OnAddToNewPlaylist(std::wstring& playlist_path)
{
    auto getSongList = [&](std::vector<SongInfo>& song_list)
    {
        GetSongsSelected(song_list);
    };
    CMusicPlayerCmdHelper cmd_helper(this);
    return cmd_helper.OnAddToNewPlaylist(getSongList, playlist_path, GetNewPlaylistName());
}

UINT CMediaLibTabDlg::ViewOnlineThreadFunc(LPVOID lpParam)
{
    CMediaLibTabDlg* pThis = (CMediaLibTabDlg*)(lpParam);
    if (pThis == nullptr)
        return 0;
    CCommon::SetThreadLanguageList(theApp.m_str_table.GetLanguageTag());
    //此命令用于跳转到歌曲对应的网易云音乐的在线页面
    if (pThis->GetItemSelected() >= 0)
    {
        SongInfo sel_song = pThis->GetSongList()[pThis->GetItemSelected()];
        if (CCommon::FileExist(sel_song.file_path))
        {
            CMusicPlayerCmdHelper cmd_helper(pThis);
            cmd_helper.VeiwOnline(sel_song);
        }
    }
    return 0;
}

void CMediaLibTabDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类

    vector<SongInfo> songs;
    GetSongsSelected(songs);
    if (!songs.empty())
    {
        bool ok{};
        CMediaClassifier::ClassificationType type = GetClassificationType();
        wstring item_name = GetClassificationItemName();
        if (type != CMediaClassifier::CT_NONE && item_name != STR_OTHER_CLASSIFY_TYPE)
        {
            int item_selected = GetItemSelected();
            //右侧选中了曲目，则播放选中的曲目
            if (item_selected >= 0)
                ok = CPlayer::GetInstance().SetMediaLibPlaylist(type, item_name, -1, songs.front(), true, true);
            //右侧没有选中曲目，则不指定播放曲目（继续播放上次播放的曲目）
            else
                ok = CPlayer::GetInstance().SetMediaLibPlaylist(type, item_name, -1, SongInfo(), true, true);
        }
        else
        {
            if (songs.size() > 1 || CFilePathHelper(songs[0].file_path).GetFileExtension() == L"cue")   // 为兼容可存在.cue文件的旧媒体库保留
            {
                ok = CPlayer::GetInstance().OpenSongsInTempPlaylist(songs);
            }
            else
            {
                ok = CPlayer::GetInstance().OpenSongsInTempPlaylist(GetSongList(), GetItemSelected());
            }
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


void CMediaLibTabDlg::OnCancel()
{
    // TODO: 在此添加专用代码和/或调用基类

    CTabDlg::OnCancel();

    CWnd* pParent = GetParentWindow();
    if (pParent != nullptr)
        ::PostMessage(pParent->GetSafeHwnd(), WM_COMMAND, IDCANCEL, 0);
}


BOOL CMediaLibTabDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
    // TODO: 在此添加专用代码和/或调用基类

    WORD command = LOWORD(wParam);
    auto getSelectedItems = [&](std::vector<SongInfo>& item_list)
    {
        GetSongsSelected(item_list);
    };
    //响应播放列表右键菜单中的“添加到播放列表”
    CMusicPlayerCmdHelper cmd_helper;
    cmd_helper.OnAddToPlaylistCommand(getSelectedItems, command);

    //响应右键菜单中的分级
    if ((command >= ID_RATING_1 && command <= ID_RATING_5) || command == ID_RATING_NONE)    //如果命令是歌曲分级（应确保分级命令的ID是连续的）
    {
        std::vector<SongInfo> songs_selected;
        GetSongsSelected(songs_selected);
        for (const auto& song : songs_selected)
        {
            cmd_helper.OnRating(song, command);
        }
    }

    return CTabDlg::OnCommand(wParam, lParam);
}

void CMediaLibTabDlg::OnInitMenu(CMenu* pMenu)
{
    CTabDlg::OnInitMenu(pMenu);

    // TODO: 在此处添加消息处理程序代码
    // 此处设置m_media_lib_popup_menu左侧菜单和右侧菜单的状态
    vector<SongInfo> songs;
    GetSongsSelected(songs);
    bool select_all_in_playing_list = CPlayer::GetInstance().IsSongsInPlayList(songs);
    // 选中歌曲全部为cue或osu!文件时禁用从磁盘删除菜单项
    bool can_del = !theApp.m_media_lib_setting_data.disable_delete_from_disk &&
        std::find_if(songs.begin(), songs.end(), [&](const SongInfo& song_info) { return song_info.is_cue || COSUPlayerHelper::IsOsuFile(song_info.file_path); }) != songs.end();

    pMenu->EnableMenuItem(ID_PLAY_AS_NEXT, MF_BYCOMMAND | (select_all_in_playing_list ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_DELETE_FROM_DISK, MF_BYCOMMAND | (can_del ? MF_ENABLED : MF_GRAYED));
}


void CMediaLibTabDlg::OnPlayItem()
{
    // TODO: 在此添加命令处理程序代码
    OnOK();
}


void CMediaLibTabDlg::OnPlayAsNext()
{
    // TODO: 在此添加命令处理程序代码
    vector<SongInfo> songs;
    GetSongsSelected(songs);
    CPlayer::GetInstance().PlayAfterCurrentTrack(songs);
    ::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_SET_UI_FORCE_FRESH_FLAG, 0, 0);
}


void CMediaLibTabDlg::OnPlayItemInFolderMode()
{
    // TODO: 在此添加命令处理程序代码
    int sel_item{ GetItemSelected() };
    if (sel_item >= 0 && sel_item < static_cast<int>(GetSongList().size()))
    {
        if (!CPlayer::GetInstance().OpenASongInFolderMode(GetSongList()[sel_item], true))
        {
            const wstring& info = theApp.m_str_table.LoadText(L"MSG_WAIT_AND_RETRY");
            MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
        }
        else
            OnCancel();
    }
}


void CMediaLibTabDlg::OnAddToNewPlaylist()
{
    // TODO: 在此添加命令处理程序代码
    wstring playlist_path;
    _OnAddToNewPlaylist(playlist_path);
}


void CMediaLibTabDlg::OnAddToNewPlaylistAndPlay()
{
    // TODO: 在此添加命令处理程序代码
    wstring playlist_path;
    if (_OnAddToNewPlaylist(playlist_path))
    {
        if (!CPlayer::GetInstance().SetPlaylist(playlist_path, 0, 0, true))
        {
            const wstring& info = theApp.m_str_table.LoadText(L"MSG_WAIT_AND_RETRY");
            MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
        }
        else
            OnCancel();
    }
}


void CMediaLibTabDlg::OnExploreOnline()
{
    // TODO: 在此添加命令处理程序代码
    AfxBeginThread(ViewOnlineThreadFunc, (void*)this);
}


void CMediaLibTabDlg::OnExploreTrack()
{
    // TODO: 在此添加命令处理程序代码
    int sel_item{ GetItemSelected() };
    if (sel_item >= 0 && sel_item < static_cast<int>(GetSongList().size()) && CCommon::FileExist(GetSongList()[sel_item].file_path))
    {
        CString str;
        str.Format(_T("/select,\"%s\""), GetSongList()[sel_item].file_path.c_str());
        ShellExecute(NULL, _T("open"), _T("explorer"), str, NULL, SW_SHOWNORMAL);
    }
}


void CMediaLibTabDlg::OnFormatConvert()
{
    // TODO: 在此添加命令处理程序代码
    std::vector<SongInfo> songs;
    GetSongsSelected(songs);
    CMusicPlayerCmdHelper cmd_helper(this);
    cmd_helper.FormatConvert(songs);
}


void CMediaLibTabDlg::OnDeleteFromDisk()
{
    // TODO: 在此添加命令处理程序代码
    vector<SongInfo> songs_selected;
    GetSongsSelected(songs_selected);
    CMusicPlayerCmdHelper helper;
    if (helper.DeleteSongsFromDisk(songs_selected))
    {
        AfterDeleteFromDisk(songs_selected);
    }
}


void CMediaLibTabDlg::OnItemProperty()
{
    // TODO: 在此添加命令处理程序代码
    if (GetItemSelected() < 0)
        return;
    CPropertyDlg* pDlg;
    std::vector<SongInfo> songs;
    if (GetItemsSelected().size() > 1)        // 选中项多于一个时批量编辑选中项
    {
        GetSongsSelected(songs);
        pDlg = new CPropertyDlg(songs);
    }
    else                                      // 只有一个选中项时打开选中歌曲属性窗口，songs是用来支持翻页的数据
    {
        songs = GetSongList();
        pDlg = new CPropertyDlg(songs, GetItemSelected(), false);
    }
    pDlg->DoModal();
    if (pDlg->GetModified())
        RefreshSongList();
    SAFE_DELETE(pDlg);
}


void CMediaLibTabDlg::OnCopyText()
{
    // TODO: 在此添加命令处理程序代码
    if (!CCommon::CopyStringToClipboard(GetSelectedString()))
        MessageBox(theApp.m_str_table.LoadText(L"MSG_COPY_CLIPBOARD_FAILED").c_str(), NULL, MB_ICONWARNING);
}
