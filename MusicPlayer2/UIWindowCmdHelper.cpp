#include "stdafx.h"
#include "UIWindowCmdHelper.h"
#include "Player.h"
#include "UserUi.h"
#include "UiMediaLibItemMgr.h"
#include "MusicPlayerCmdHelper.h"
#include "PropertyDlg.h"
#include "COSUPlayerHelper.h"

CUIWindowCmdHelper::CUIWindowCmdHelper(IPlayerUI* pUI)
    : m_pUI(pUI)
{
}

void CUIWindowCmdHelper::OnUiCommand(DWORD command)
{
    CUserUi* pUi = dynamic_cast<CUserUi*>(m_pUI);
    if (pUi != nullptr)
    {
        //获取命令的发送者
        UiElement::MediaLibItemList* medialib_item_list = dynamic_cast<UiElement::MediaLibItemList*>(pUi->m_context_menu_sender);
        if (medialib_item_list != nullptr)
        {
            OnMediaLibItemListCommand(medialib_item_list, command);
        }
        UiElement::RecentPlayedList* recent_played_list = dynamic_cast<UiElement::RecentPlayedList*>(pUi->m_context_menu_sender);
        if (recent_played_list != nullptr)
        {
            OnRecentPlayedListCommand(recent_played_list, command);
        }
        UiElement::MediaLibFolder* medialib_folder = dynamic_cast<UiElement::MediaLibFolder*>(pUi->m_context_menu_sender);
        if (medialib_folder != nullptr)
        {
            OnMediaLibFolderCommand(medialib_folder, command);
        }
        UiElement::MediaLibPlaylist* medialib_playlist = dynamic_cast<UiElement::MediaLibPlaylist*>(pUi->m_context_menu_sender);
        if (medialib_playlist != nullptr)
        {
            OnMediaLibPlaylistCommand(medialib_playlist, command);
        }
        UiElement::MyFavouriteList* my_favourite_list = dynamic_cast<UiElement::MyFavouriteList*>(pUi->m_context_menu_sender);
        if (my_favourite_list != nullptr)
        {
            OnMyFavouriteListCommand(my_favourite_list, command);
        }
        UiElement::AllTracksList* all_tracks_list = dynamic_cast<UiElement::AllTracksList*>(pUi->m_context_menu_sender);
        if (all_tracks_list != nullptr)
        {
            OnAllTracksListCommand(all_tracks_list, command);
        }
        UiElement::Playlist* playlist = dynamic_cast<UiElement::Playlist*>(pUi->m_context_menu_sender);
        if (playlist != nullptr)
        {
            OnAddToPlaystCommand(playlist, command);
        }
        pUi->m_context_menu_sender = nullptr;   //命令被响应后清空上次保存的命令发送者
    }
}

void CUIWindowCmdHelper::SetMenuState(CMenu* pMenu)
{
    if (pMenu == theApp.m_menu_mgr.GetMenu(MenuMgr::UiLibLeftMenu))
    {
        SetMediaLibItemListMenuState(pMenu);
    }
    else if (pMenu == theApp.m_menu_mgr.GetMenu(MenuMgr::UiLibSetPathMenu))
    {
        SetMediaLibFolderMenuState(pMenu);
    }
    else if (pMenu == theApp.m_menu_mgr.GetMenu(MenuMgr::LibPlaylistMenu))
    {
        SetMediaLibPlaylistMenuState(pMenu);
    }
    else if (pMenu == theApp.m_menu_mgr.GetMenu(MenuMgr::UiRecentPlayedMenu))
    {
        SetRecentPlayedListMenuState(pMenu);
    }
    else if (pMenu == theApp.m_menu_mgr.GetMenu(MenuMgr::UiMyFavouriteMenu))
    {
        SetMyFavouriteListMenuState(pMenu);
    }
    else if (pMenu == theApp.m_menu_mgr.GetMenu(MenuMgr::LibRightMenu))
    {
        SetAllTracksListMenuState(pMenu);
    }
    else if (pMenu == theApp.m_menu_mgr.GetMenu(MenuMgr::AddToPlaylistMenu))
    {
        SetAddToPlaylistMenuState(pMenu);
    }
}

void CUIWindowCmdHelper::OnMediaLibItemListCommand(UiElement::MediaLibItemList* medialib_item_list, DWORD command)
{
    std::wstring item_name = CUiMediaLibItemMgr::Instance().GetItemName(medialib_item_list->type, medialib_item_list->GetItemSelected());
    std::wstring display_name = CUiMediaLibItemMgr::Instance().GetItemDisplayName(medialib_item_list->type, medialib_item_list->GetItemSelected());
    auto getSongList = [&](std::vector<SongInfo>& song_list)
        {
            CMediaClassifier classifier(medialib_item_list->type);
            classifier.ClassifyMedia();
            song_list = classifier.GetMeidaList()[item_name];
        };
    CMusicPlayerCmdHelper helper;

    //播放
    if (command == ID_PLAY_ITEM)
    {
        helper.OnMediaLibItemSelected(medialib_item_list->type, item_name, true);
    }
    //添加到新播放列表
    else if (command == ID_ADD_TO_NEW_PLAYLIST)
    {
        wstring playlist_path;
        helper.OnAddToNewPlaylist(getSongList, playlist_path, display_name);
    }

    //添加到新播放列表并播放
    else if (command == ID_ADD_TO_NEW_PLAYLIST_AND_PLAY)
    {
        wstring playlist_path;
        if (helper.OnAddToNewPlaylist(getSongList, playlist_path, display_name))
        {
            if (!CPlayer::GetInstance().SetPlaylist(playlist_path, 0, 0, true))
            {
                const wstring& info = theApp.m_str_table.LoadText(L"MSG_WAIT_AND_RETRY");
                AfxMessageBox(info.c_str(), MB_ICONINFORMATION | MB_OK);
            }
        }
    }
    //复制文本
    else if (command == ID_COPY_TEXT)
    {
        if (!CCommon::CopyStringToClipboard(display_name))
            AfxMessageBox(theApp.m_str_table.LoadText(L"MSG_COPY_CLIPBOARD_FAILED").c_str(), MB_ICONWARNING);

    }
    //在媒体库中查看
    else if (command == ID_VIEW_IN_MEDIA_LIB)
    {
        CMusicPlayerCmdHelper::eMediaLibTab tab{};
        switch (medialib_item_list->type)
        {
        case CMediaClassifier::CT_ARTIST: tab = CMusicPlayerCmdHelper::ML_ARTIST; break;
        case CMediaClassifier::CT_ALBUM: tab = CMusicPlayerCmdHelper::ML_ALBUM; break;
        case CMediaClassifier::CT_GENRE: tab = CMusicPlayerCmdHelper::ML_GENRE; break;
        case CMediaClassifier::CT_YEAR: tab = CMusicPlayerCmdHelper::ML_YEAR; break;
        case CMediaClassifier::CT_TYPE: tab = CMusicPlayerCmdHelper::ML_FILE_TYPE; break;
        case CMediaClassifier::CT_BITRATE: tab = CMusicPlayerCmdHelper::ML_BITRATE; break;
        case CMediaClassifier::CT_RATING: tab = CMusicPlayerCmdHelper::ML_RATING; break;
        }
        helper.OnViewInMediaLib(tab, item_name);
    }
    else
    {
        helper.OnAddToPlaylistCommand(getSongList, command);
    }
}

void CUIWindowCmdHelper::OnRecentPlayedListCommand(UiElement::RecentPlayedList* medialib_item_list, DWORD command)
{
    int item_selected{ medialib_item_list->GetItemSelected() };
    if (item_selected < 0 || item_selected >= static_cast<int>(CRecentFolderAndPlaylist::Instance().GetItemList().size()))
        return;

    const CRecentFolderAndPlaylist::Item& item{ CRecentFolderAndPlaylist::Instance().GetItemList()[item_selected] };

    //播放
    if (command == ID_PLAY_ITEM)
    {
        CMusicPlayerCmdHelper helper;
        helper.OnRecentItemSelected(item_selected, true);
    }
    //移除
    else if (command == ID_RECENT_PLAYED_REMOVE)
    {
        CString item_str;
        std::wstring type_name;
        if (item.IsMedialib())
            type_name = CMediaLibPlaylistMgr::GetTypeName(item.medialib_info->medialib_type);
        else if (item.IsFolder())
            type_name = theApp.m_str_table.LoadText(L"TXT_FOLDER");
        else if (item.IsPlaylist())
            type_name = theApp.m_str_table.LoadText(L"TXT_PLAYLIST");
        item_str.Format(_T("%s: %s"), type_name.c_str(), item.GetName().c_str());
        std::wstring messagebox_info = theApp.m_str_table.LoadTextFormat(L"MSG_DELETE_RECENTPLAYED_ITEM_INQUIRY", { item_str });
        if (AfxMessageBox(messagebox_info.c_str(), MB_ICONQUESTION | MB_YESNO) == IDYES)
        {
            CRecentFolderAndPlaylist::Instance().RemoveItem(item);
        }
    }
    //复制文本
    else if (command == ID_COPY_TEXT)
    {
        if (!CCommon::CopyStringToClipboard(item.GetName()))
            AfxMessageBox(theApp.m_str_table.LoadText(L"MSG_COPY_CLIPBOARD_FAILED").c_str(), MB_ICONWARNING);
    }
    //在媒体库中查看
    else if (command == ID_VIEW_IN_MEDIA_LIB)
    {
        CMusicPlayerCmdHelper helper;
        if (item.IsFolder())
        {
            wstring folder_path{ item.folder_info->path };
            helper.OnViewInMediaLib(CMusicPlayerCmdHelper::ML_FOLDER, folder_path);
        }
        else if (item.IsPlaylist())
        {
            wstring playlist_path{ item.playlist_info->path };
            helper.OnViewInMediaLib(CMusicPlayerCmdHelper::ML_PLAYLIST, playlist_path);
        }
        else if (item.IsMedialib())
        {
            CMusicPlayerCmdHelper::eMediaLibTab tab{};
            switch (item.medialib_info->medialib_type)
            {
            case CMediaClassifier::CT_ARTIST: tab = CMusicPlayerCmdHelper::ML_ARTIST; break;
            case CMediaClassifier::CT_ALBUM: tab = CMusicPlayerCmdHelper::ML_ALBUM; break;
            case CMediaClassifier::CT_GENRE: tab = CMusicPlayerCmdHelper::ML_GENRE; break;
            case CMediaClassifier::CT_YEAR: tab = CMusicPlayerCmdHelper::ML_YEAR; break;
            case CMediaClassifier::CT_TYPE: tab = CMusicPlayerCmdHelper::ML_FILE_TYPE; break;
            case CMediaClassifier::CT_BITRATE: tab = CMusicPlayerCmdHelper::ML_BITRATE; break;
            case CMediaClassifier::CT_RATING: tab = CMusicPlayerCmdHelper::ML_RATING; break;
            case CMediaClassifier::CT_NONE: tab = CMusicPlayerCmdHelper::ML_ALL; break;
            }
            helper.OnViewInMediaLib(tab, item.medialib_info->path);
        }
    }
}

void CUIWindowCmdHelper::OnMediaLibFolderCommand(UiElement::MediaLibFolder* medialib_folder, DWORD command)
{
    int item_selected{ medialib_folder->GetItemSelected() };

    PathInfo& path_info{ CRecentFolderMgr::Instance().GetItem(item_selected) };
    CMusicPlayerCmdHelper helper;

    if (command == ID_PLAY_PATH)
    {
        helper.OnFolderSelected(path_info, true);
    }
    else if (command == ID_DELETE_PATH)
    {
        helper.OnDeleteRecentFolder(path_info.path);
    }
    else if (command == ID_BROWSE_PATH)
    {
        ShellExecute(NULL, _T("open"), _T("explorer"), path_info.path.c_str(), NULL, SW_SHOWNORMAL);
    }
    else if (command == ID_CONTAIN_SUB_FOLDER)
    {
        // 如果是当前播放则使用CPlayer成员方法更改（会启动播放列表初始化）不需要操作CPlayer::GetInstance().GetRecentPath()
        if (CPlayer::GetInstance().IsFolderMode() && CPlayer::GetInstance().GetCurrentDir2() == path_info.path)
        {
            if (!CPlayer::GetInstance().SetContainSubFolder())
            {
                const wstring& info = theApp.m_str_table.LoadText(L"MSG_WAIT_AND_RETRY");
                AfxMessageBox(info.c_str(), MB_ICONINFORMATION | MB_OK);
            }
        }
        else
        {
            path_info.contain_sub_folder = !path_info.contain_sub_folder;
        }

    }
    else if (command == ID_CLEAR_INVALID_PATH)
    {
        const wstring& inquiry_info = theApp.m_str_table.LoadText(L"MSG_LIB_PATH_CLEAR_INQUIRY");
        if (AfxMessageBox(inquiry_info.c_str(), MB_ICONQUESTION | MB_OKCANCEL) == IDCANCEL)
            return;
        int cleard_cnt = CRecentFolderMgr::Instance().DeleteInvalidItems();
        CRecentFolderAndPlaylist::Instance().Init();
        wstring complete_info = theApp.m_str_table.LoadTextFormat(L"MSG_LIB_PATH_CLEAR_COMPLETE", { cleard_cnt });
        AfxMessageBox(complete_info.c_str(), MB_ICONINFORMATION | MB_OK);
    }
    else if (command == ID_FILE_OPEN_FOLDER)
    {
        helper.OnOpenFolder();
    }
}

void CUIWindowCmdHelper::OnMediaLibPlaylistCommand(UiElement::MediaLibPlaylist* medialib_folder, DWORD command)
{
    int item_selected{ medialib_folder->GetItemSelected() };

    PlaylistInfo playlist;
    CPlaylistMgr::Instance().GetPlaylistInfo(item_selected, [&](const PlaylistInfo& playlist_info) {
        playlist = playlist_info;
    });
    CMusicPlayerCmdHelper helper;

    if (command == ID_PLAY_PLAYLIST)
    {
        helper.OnPlaylistSelected(playlist, true);
    }
    else if (command == ID_RENAME_PLAYLIST)
    {
        helper.OnRenamePlaylist(playlist.path);
    }
    else if (command == ID_DELETE_PLAYLIST)
    {
        helper.OnDeletePlaylist(playlist.path);
    }
    else if (command == ID_SAVE_AS_NEW_PLAYLIST)
    {
        wstring new_playlist_path = helper.OnNewPlaylist();
        if (!new_playlist_path.empty())
        {
            PlaylistInfo playlist_info{ playlist };
            CopyFile(playlist_info.path.c_str(), new_playlist_path.c_str(), FALSE);
            playlist_info.path = new_playlist_path;
            playlist_info.last_played_time = 0;
            CPlaylistMgr::Instance().UpdatePlaylistInfo(playlist_info);
        }
    }
    else if (command == ID_PLAYLIST_SAVE_AS)
    {
        helper.OnPlaylistSaveAs(playlist.path);

    }
    else if (command == ID_PLAYLIST_BROWSE_FILE)
    {
        if (!playlist.path.empty())
        {
            CString str;
            str.Format(_T("/select,\"%s\""), playlist.path.c_str());
            ShellExecute(NULL, _T("open"), _T("explorer"), str, NULL, SW_SHOWNORMAL);
        }
    }
    else if (command == ID_PLAYLIST_FIX_PATH_ERROR)
    {
        helper.OnPlaylistFixPathError(playlist.path);
    }
    else if (command == ID_NEW_PLAYLIST)
    {
        helper.OnNewPlaylist();
    }
}

bool CUIWindowCmdHelper::OnSongListCommand(const SongInfo& song_info, DWORD command)
{
    CMusicPlayerCmdHelper helper;
    //下一首播放
    if (command == ID_PLAY_AS_NEXT)
    {
        vector<SongInfo> songs{ song_info };
        CPlayer::GetInstance().PlayAfterCurrentTrack(songs);
    }
    //在文件夹模式中播放
    else if (command == ID_PLAY_ITEM_IN_FOLDER_MODE)
    {
        if (!CPlayer::GetInstance().OpenASongInFolderMode(song_info, true))
        {
            const wstring& info = theApp.m_str_table.LoadText(L"MSG_WAIT_AND_RETRY");
            AfxMessageBox(info.c_str(), MB_ICONINFORMATION | MB_OK);
        }
    }
    //在线查看
    else if (command == ID_EXPLORE_ONLINE)
    {
        AfxBeginThread(CMusicPlayerCmdHelper::ViewOnlineThreadFunc, (LPVOID)&song_info);
    }
    //格式转换
    else if (command == ID_FORMAT_CONVERT)
    {
        vector<SongInfo> songs{ song_info };
        helper.FormatConvert(songs);
    }
    //打开文件位置
    else if (command == ID_EXPLORE_TRACK)
    {
        CString str;
        str.Format(_T("/select,\"%s\""), song_info.file_path.c_str());
        ShellExecute(NULL, _T("open"), _T("explorer"), str, NULL, SW_SHOWNORMAL);
    }
    //从磁盘删除
    else if (command == ID_DELETE_FROM_DISK)
    {
        vector<SongInfo> songs{ song_info };
        helper.DeleteSongsFromDisk(songs);
    }
    //添加到播放列表
    else
    {
        auto getSongList = [&](std::vector<SongInfo>& song_list) {
            song_list.clear();
            song_list.push_back(song_info);
        };
        return helper.OnAddToPlaylistCommand(getSongList, command);
    }
    return true;
}

void CUIWindowCmdHelper::OnMyFavouriteListCommand(UiElement::MyFavouriteList* my_favourite_list, DWORD command)
{
    int item_selected{ my_favourite_list->GetItemSelected() };
    if (item_selected < 0 || item_selected >= CUiMyFavouriteItemMgr::Instance().GetSongCount())
        return;

    const SongInfo& song_info = CUiMyFavouriteItemMgr::Instance().GetSongInfo(item_selected);
    CMusicPlayerCmdHelper helper;

    if (!OnSongListCommand(song_info, command))
    {
        //播放
        if (command == ID_PLAY_ITEM)
        {
            helper.OnPlayMyFavourite(item_selected);
        }
        //从列表中删除
        else if (command == ID_REMOVE_FROM_PLAYLIST)
        {
            vector<SongInfo> songs{ song_info };
            helper.OnRemoveFromPlaylist(songs, theApp.m_playlist_dir + FAVOURITE_PLAYLIST_NAME);
        }
        //属性
        else if (command == ID_ITEM_PROPERTY)
        {
            std::vector<SongInfo> songs;
            CUiMyFavouriteItemMgr::Instance().GetSongList(songs);
            CPropertyDlg dlg(songs, item_selected, false);
            dlg.DoModal();
        }
        //复制文本
        else if (command == ID_COPY_TEXT)
        {
            std::wstring text = my_favourite_list->GetItemText(item_selected, UiElement::MyFavouriteList::COL_TRACK);
            if (!CCommon::CopyStringToClipboard(text))
                AfxMessageBox(theApp.m_str_table.LoadText(L"MSG_COPY_CLIPBOARD_FAILED").c_str(), MB_ICONWARNING);
        }
    }
}

void CUIWindowCmdHelper::OnAllTracksListCommand(UiElement::AllTracksList* all_tracks_list, DWORD command)
{
    int item_selected{ all_tracks_list->GetItemSelected() };
    if (item_selected < 0 || item_selected >= CUiAllTracksMgr::Instance().GetSongCount())
        return;

    const SongInfo& song_info = CUiAllTracksMgr::Instance().GetSongInfo(item_selected);
    CMusicPlayerCmdHelper helper;

    if (!OnSongListCommand(song_info, command))
    {
        //播放
        if (command == ID_PLAY_ITEM)
        {
            helper.OnPlayAllTrack(song_info);
        }
        //属性
        else if (command == ID_ITEM_PROPERTY)
        {
            std::vector<SongInfo> songs;
            CUiAllTracksMgr::Instance().GetSongList(songs);
            CPropertyDlg dlg(songs, item_selected, false);
            dlg.DoModal();
        }
        //复制文本
        else if (command == ID_COPY_TEXT)
        {
            std::wstring text = all_tracks_list->GetItemText(item_selected, UiElement::AllTracksList::COL_TRACK);
            if (!CCommon::CopyStringToClipboard(text))
                AfxMessageBox(theApp.m_str_table.LoadText(L"MSG_COPY_CLIPBOARD_FAILED").c_str(), MB_ICONWARNING);
        }

    }
}

void CUIWindowCmdHelper::OnAddToPlaystCommand(UiElement::Playlist* playlist, DWORD command)
{
    int item_selected{ playlist->GetItemSelected() };
    if (item_selected < 0 || item_selected >= CPlayer::GetInstance().GetSongNum())
        return;
    const SongInfo& song_info = CPlayer::GetInstance().GetPlayList()[item_selected];
    CMusicPlayerCmdHelper helper;
    auto getSongList = [&](std::vector<SongInfo>& song_list) {
        song_list.clear();
        song_list.push_back(song_info);
        };
    //添加到新播放列表
    if (command == ID_ADD_TO_NEW_PLAYLIST)
    {
        wstring playlist_path;
        helper.OnAddToNewPlaylist(getSongList, playlist_path);
    }
    else
    {
        helper.OnAddToPlaylistCommand(getSongList, command);
    }
}

void CUIWindowCmdHelper::SetRecentPlayedListMenuState(CMenu* pMenu)
{
    CUserUi* pUi = dynamic_cast<CUserUi*>(m_pUI);
    if (pUi != nullptr)
    {
        UiElement::RecentPlayedList* recent_played = dynamic_cast<UiElement::RecentPlayedList*>(pUi->m_context_menu_sender);
        if (recent_played != nullptr)
        {
            int item_selected{ recent_played->GetItemSelected() };
            //最近播放中排在第一个的项目为正在播放的项目，不允许移除
            pMenu->EnableMenuItem(ID_RECENT_PLAYED_REMOVE, MF_BYCOMMAND | (item_selected > 0 ? MF_ENABLED : MF_GRAYED));
        }
    }
}

void CUIWindowCmdHelper::SetMediaLibItemListMenuState(CMenu* pMenu)
{
    //设置“添加到播放列表”子菜单项的可用状态
    for (UINT id = ID_ADD_TO_MY_FAVOURITE + 1; id < ID_ADD_TO_MY_FAVOURITE + ADD_TO_PLAYLIST_MAX_SIZE + 1; id++)
    {
        CString menu_string;
        pMenu->GetMenuString(id, menu_string, 0);
        pMenu->EnableMenuItem(id, MF_BYCOMMAND | MF_ENABLED);
    }
    pMenu->EnableMenuItem(ID_ADD_TO_NEW_PLAYLIST, MF_BYCOMMAND | MF_ENABLED);
    pMenu->EnableMenuItem(ID_ADD_TO_OTHER_PLAYLIST, MF_BYCOMMAND | MF_ENABLED);
}

void CUIWindowCmdHelper::SetMediaLibFolderMenuState(CMenu* pMenu)
{
    bool select_valid{};
    bool contain_sub_folder{};
    UiElement::MediaLibFolder* medialib_folder{};
    CUserUi* pUi = dynamic_cast<CUserUi*>(m_pUI);
    if (pUi != nullptr)
    {
        medialib_folder = dynamic_cast<UiElement::MediaLibFolder*>(pUi->m_context_menu_sender);
    }
    if (medialib_folder != nullptr)
    {
        int item_selected{ medialib_folder->GetItemSelected() };
        if (item_selected >= 0 && item_selected < CRecentFolderMgr::Instance().GetItemSize())
            select_valid = true;

        const PathInfo& path_info{ CRecentFolderMgr::Instance().GetItem(item_selected) };
        contain_sub_folder = path_info.contain_sub_folder;
    }

    pMenu->EnableMenuItem(ID_PLAY_PATH, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_DELETE_PATH, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_BROWSE_PATH, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_CONTAIN_SUB_FOLDER, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->CheckMenuItem(ID_CONTAIN_SUB_FOLDER, MF_BYCOMMAND | (select_valid && contain_sub_folder ? MF_CHECKED : MF_UNCHECKED));
}

void CUIWindowCmdHelper::SetMediaLibPlaylistMenuState(CMenu* pMenu)
{
    bool select_valid{};
    PlaylistInfo selected_playlist;
    CUserUi* pUi = dynamic_cast<CUserUi*>(m_pUI);
    if (pUi != nullptr)
    {
        UiElement::MediaLibPlaylist* medialib_playlist = dynamic_cast<UiElement::MediaLibPlaylist*>(pUi->m_context_menu_sender);
        if (medialib_playlist != nullptr)
        {
            int item_selected{ medialib_playlist->GetItemSelected() };
            if (item_selected >= 0 && item_selected < static_cast<int>(CPlaylistMgr::Instance().GetPlaylistNum()))
            {
                select_valid = true;
                CPlaylistMgr::Instance().GetPlaylistInfo(item_selected, [&](const PlaylistInfo& playlist_info) {
                    selected_playlist = playlist_info;
                });
            }
        }
    }

    bool selected_can_play{ select_valid &&
        (
            !CPlayer::GetInstance().IsPlaylistMode() ||
            selected_playlist.path != CPlayer::GetInstance().GetPlaylistPath()
        )};

    wstring sel_playlist_name = CFilePathHelper(selected_playlist.path).GetFileName();
    bool is_spec_playlist{ sel_playlist_name == DEFAULT_PLAYLIST_NAME || sel_playlist_name == FAVOURITE_PLAYLIST_NAME };
    bool is_temp_playlist{ sel_playlist_name == TEMP_PLAYLIST_NAME };
    pMenu->EnableMenuItem(ID_RENAME_PLAYLIST, MF_BYCOMMAND | (select_valid && !is_spec_playlist && !is_temp_playlist ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_DELETE_PLAYLIST, MF_BYCOMMAND | (select_valid && !is_spec_playlist ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_PLAY_PLAYLIST, MF_BYCOMMAND | (selected_can_play ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_SAVE_AS_NEW_PLAYLIST, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_PLAYLIST_SAVE_AS, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_PLAYLIST_FIX_PATH_ERROR, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_PLAYLIST_BROWSE_FILE, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
}

void CUIWindowCmdHelper::SetMyFavouriteListMenuState(CMenu* pMenu)
{
    bool selected_in_current_playing_list{ false }; //选中是否正在播放
    bool can_del{ false };
    CUserUi* pUi = dynamic_cast<CUserUi*>(m_pUI);
    if (pUi != nullptr)
    {
        UiElement::MyFavouriteList* my_favourite_list = dynamic_cast<UiElement::MyFavouriteList*>(pUi->m_context_menu_sender);
        if (my_favourite_list != nullptr)
        {
            int item_selected{ my_favourite_list->GetItemSelected() };
            if (item_selected >= 0 && item_selected < static_cast<int>(CUiMyFavouriteItemMgr::Instance().GetSongCount()))
            {
                //获取选中曲目
                const SongInfo& selected_song{ CUiMyFavouriteItemMgr::Instance().GetSongInfo(item_selected) };
                std::vector<SongInfo> selected_songs{ selected_song };
                //判断是否可以下一首播放
                selected_in_current_playing_list = CPlayer::GetInstance().IsSongsInPlayList(selected_songs);
                //判断是否可以从磁盘删除
                can_del = !theApp.m_media_lib_setting_data.disable_delete_from_disk &&
                    !selected_song.is_cue &&
                    !COSUPlayerHelper::IsOsuFile(selected_song.file_path);
            }
        }
    }

    pMenu->EnableMenuItem(ID_PLAY_AS_NEXT, MF_BYCOMMAND | (selected_in_current_playing_list ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_DELETE_FROM_DISK, MF_BYCOMMAND | (can_del ? MF_ENABLED : MF_GRAYED));
}

void CUIWindowCmdHelper::SetAllTracksListMenuState(CMenu* pMenu)
{
    bool selected_in_current_playing_list{ false }; //选中是否正在播放
    bool can_del{ false };
    CUserUi* pUi = dynamic_cast<CUserUi*>(m_pUI);
    if (pUi != nullptr)
    {
        UiElement::AllTracksList* all_tracks_list = dynamic_cast<UiElement::AllTracksList*>(pUi->m_context_menu_sender);
        if (all_tracks_list != nullptr)
        {
            int item_selected{ all_tracks_list->GetItemSelected() };
            if (item_selected >= 0 && item_selected < static_cast<int>(CUiAllTracksMgr::Instance().GetSongCount()))
            {
                //获取选中曲目
                const SongInfo& selected_song{ CUiAllTracksMgr::Instance().GetSongInfo(item_selected) };
                std::vector<SongInfo> selected_songs{ selected_song };
                //判断是否可以下一首播放
                selected_in_current_playing_list = CPlayer::GetInstance().IsSongsInPlayList(selected_songs);
                //判断是否可以从磁盘删除
                can_del = !theApp.m_media_lib_setting_data.disable_delete_from_disk &&
                    !selected_song.is_cue &&
                    !COSUPlayerHelper::IsOsuFile(selected_song.file_path);
            }
        }
    }

    pMenu->EnableMenuItem(ID_PLAY_AS_NEXT, MF_BYCOMMAND | (selected_in_current_playing_list ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_DELETE_FROM_DISK, MF_BYCOMMAND | (can_del ? MF_ENABLED : MF_GRAYED));
}

void CUIWindowCmdHelper::SetAddToPlaylistMenuState(CMenu* pMenu)
{
    //判断菜单的发送者
    UiElement::Playlist* playlist{};
    UiElement::MyFavouriteList* my_favourite_list{};
    CUserUi* pUi = dynamic_cast<CUserUi*>(m_pUI);
    if (pUi != nullptr)
    {
        playlist = dynamic_cast<UiElement::Playlist*>(pUi->m_context_menu_sender);
        my_favourite_list = dynamic_cast<UiElement::MyFavouriteList*>(pUi->m_context_menu_sender);
    }
    //我喜欢的音乐菜单的名称
    wstring str_my_favourite{ theApp.m_str_table.LoadMenuText(L"ADD_TO_PLAYLIST", L"ID_ADD_TO_MY_FAVOURITE")};
    //正在播放的播放列表在菜单中的名称
    wstring current_playlist{ CPlayer::GetInstance().GetCurrentFolderOrPlaylistName() };
    if (CPlayer::GetInstance().IsPlaylistMode())
    {
        switch (CPlaylistMgr::Instance().GetCurPlaylistType())
        {
        case PT_DEFAULT: current_playlist = theApp.m_str_table.LoadMenuText(L"ADD_TO_PLAYLIST", L"ID_ADD_TO_DEFAULT_PLAYLIST"); break;
        case PT_FAVOURITE: current_playlist = str_my_favourite; break;
        }
    }
    for (UINT id = ID_ADD_TO_DEFAULT_PLAYLIST; id < ID_ADD_TO_MY_FAVOURITE + ADD_TO_PLAYLIST_MAX_SIZE + 1; id++)
    {
        CString menu_string;
        pMenu->GetMenuString(id, menu_string, 0);
        //发送者是播放列表，则将当前播放列表禁用
        if (playlist != nullptr)
            pMenu->EnableMenuItem(id, MF_BYCOMMAND | (current_playlist != menu_string.GetString() ? MF_ENABLED : MF_GRAYED));
        //发送者是我喜欢的音乐列表，则将当前我喜欢的音乐禁用
        else if (my_favourite_list != nullptr)
            pMenu->EnableMenuItem(id, MF_BYCOMMAND | (str_my_favourite != menu_string.GetString() ? MF_ENABLED : MF_GRAYED));
        else
            pMenu->EnableMenuItem(id, MF_BYCOMMAND | MF_ENABLED);

    }
}
