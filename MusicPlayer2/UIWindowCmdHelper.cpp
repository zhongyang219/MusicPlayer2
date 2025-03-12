#include "stdafx.h"
#include "UIWindowCmdHelper.h"
#include "Player.h"
#include "UserUi.h"
#include "UiMediaLibItemMgr.h"
#include "MusicPlayerCmdHelper.h"
#include "PropertyDlg.h"
#include "COSUPlayerHelper.h"
#include "PlaylistPropertiesDlg.h"
#include "FolderPropertiesDlg.h"
#include "MediaLibItemPropertiesDlg.h"
#include "CRecentList.h"

CUIWindowCmdHelper::CUIWindowCmdHelper(UiElement::Element* sender)
    : m_context_menu_sender(sender)
{
}

void CUIWindowCmdHelper::OnUiCommand(DWORD command)
{
    //获取命令的发送者
    UiElement::MediaLibItemList* medialib_item_list = dynamic_cast<UiElement::MediaLibItemList*>(m_context_menu_sender);
    if (medialib_item_list != nullptr)
    {
        OnMediaLibItemListCommand(medialib_item_list, command);
    }
    UiElement::RecentPlayedList* recent_played_list = dynamic_cast<UiElement::RecentPlayedList*>(m_context_menu_sender);
    if (recent_played_list != nullptr)
    {
        OnRecentPlayedListCommand(recent_played_list, command);
    }
    UiElement::MediaLibFolder* medialib_folder = dynamic_cast<UiElement::MediaLibFolder*>(m_context_menu_sender);
    if (medialib_folder != nullptr)
    {
        OnMediaLibFolderCommand(medialib_folder, command);
    }
    UiElement::MediaLibPlaylist* medialib_playlist = dynamic_cast<UiElement::MediaLibPlaylist*>(m_context_menu_sender);
    if (medialib_playlist != nullptr)
    {
        OnMediaLibPlaylistCommand(medialib_playlist, command);
    }
    UiElement::MyFavouriteList* my_favourite_list = dynamic_cast<UiElement::MyFavouriteList*>(m_context_menu_sender);
    if (my_favourite_list != nullptr)
    {
        OnMyFavouriteListCommand(my_favourite_list, command);
    }
    UiElement::AllTracksList* all_tracks_list = dynamic_cast<UiElement::AllTracksList*>(m_context_menu_sender);
    if (all_tracks_list != nullptr)
    {
        OnAllTracksListCommand(all_tracks_list, command);
    }
    UiElement::FolderExploreTree* folder_explore = dynamic_cast<UiElement::FolderExploreTree*>(m_context_menu_sender);
    if (folder_explore != nullptr)
    {
        OnMedialibFolderExploreCommand(folder_explore, command);
    }
    UiElement::Playlist* playlist = dynamic_cast<UiElement::Playlist*>(m_context_menu_sender);
    if (playlist != nullptr)
    {
        OnAddToPlaystCommand(playlist, command);
    }
    OnFolderOrPlaylistSortCommand(command);
}

void CUIWindowCmdHelper::SetMenuState(CMenu* pMenu)
{
    if (pMenu == theApp.m_menu_mgr.GetMenu(MenuMgr::UiLibLeftMenu))
    {
        SetMediaLibItemListMenuState(pMenu);
    }
    else if (pMenu == theApp.m_menu_mgr.GetMenu(MenuMgr::LibSetPathMenu))
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
    else if (pMenu == theApp.m_menu_mgr.GetMenu(MenuMgr::LibFolderSortMenu))
    {
        SetFolderSortMenuState(pMenu);
    }
    else if (pMenu == theApp.m_menu_mgr.GetMenu(MenuMgr::LibPlaylistSortMenu))
    {
        SetPlaylistSortMenuState(pMenu);
    }
    else
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
    ListItem list_item{ LT_MEDIA_LIB, item_name, medialib_item_list->type };

    //播放
    if (command == ID_PLAY_ITEM)
    {
        helper.OnListItemSelected(list_item, true);
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
    ListItem list_item = medialib_item_list->m_list_cache.GetItem(item_selected);
    if (list_item.empty())
        return;

    //播放
    if (command == ID_PLAY_ITEM)
    {
        CMusicPlayerCmdHelper helper;
        helper.OnListItemSelected(list_item, true);
    }
    //移除
    else if (command == ID_RECENT_PLAYED_REMOVE)
    {
        wstring info = theApp.m_str_table.LoadTextFormat(L"MSG_DELETE_RECENTPLAYED_ITEM_INQUIRY", { list_item.GetTypeDisplayName(), list_item.GetDisplayName() });
        if (AfxMessageBox(info.c_str(), MB_ICONQUESTION | MB_YESNO) == IDYES)
            CRecentList::Instance().ResetLastPlayedTime(list_item);
    }
    //复制文本
    else if (command == ID_COPY_TEXT)
    {
        if (!CCommon::CopyStringToClipboard(list_item.GetDisplayName()))
            AfxMessageBox(theApp.m_str_table.LoadText(L"MSG_COPY_CLIPBOARD_FAILED").c_str(), MB_ICONWARNING);
    }
    //在媒体库中查看
    else if (command == ID_VIEW_IN_MEDIA_LIB)
    {
        CMusicPlayerCmdHelper helper;
        CMusicPlayerCmdHelper::eMediaLibTab tab{};
        switch (list_item.type)
        {
        case LT_FOLDER: tab = CMusicPlayerCmdHelper::ML_FOLDER; break;
        case LT_PLAYLIST: tab = CMusicPlayerCmdHelper::ML_PLAYLIST; break;
        case LT_MEDIA_LIB:
            switch (list_item.medialib_type)
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
        }
        helper.OnViewInMediaLib(tab, list_item.path);
    }
    //属性
    else if (command == ID_LIB_RECENT_PLAYED_ITEM_PROPERTIES)
    {
        if (list_item.type == LT_FOLDER)
        {
            CFolderPropertiesDlg dlg(list_item);
            dlg.DoModal();
        }
        else if (list_item.type == LT_PLAYLIST)
        {
            CPlaylistPropertiesDlg dlg(list_item);
            dlg.DoModal();
        }
        else if (list_item.type == LT_MEDIA_LIB)
        {
            CMediaLibItemPropertiesDlg dlg(list_item);
            dlg.DoModal();
        }
    }
}

void CUIWindowCmdHelper::OnMediaLibFolderCommand(UiElement::MediaLibFolder* medialib_folder, DWORD command)
{
    int item_selected{ medialib_folder->GetItemSelected() };

    ListItem list_item = UiElement::MediaLibFolder::m_list_cache.GetItem(item_selected);
    CMusicPlayerCmdHelper helper;

    auto getSongList = [&](std::vector<SongInfo>& song_list) {
        CAudioCommon::GetAudioFiles(list_item.path, song_list, MAX_SONG_NUM, list_item.contain_sub_folder);
        int cnt{};
        bool flag{};
        CAudioCommon::GetCueTracks(song_list, cnt, flag, MR_MIN_REQUIRED);
    };

    if (command == ID_PLAY_PATH)
    {
        helper.OnListItemSelected(list_item, true);
    }
    else if (command == ID_DELETE_PATH)
    {
        helper.OnDeleteRecentListItem(list_item);
    }
    else if (command == ID_BROWSE_PATH)
    {
        ShellExecute(NULL, _T("open"), _T("explorer"), list_item.path.c_str(), NULL, SW_SHOWNORMAL);
    }
    else if (command == ID_CONTAIN_SUB_FOLDER)
    {
        // 如果是当前播放则使用CPlayer成员方法更改（会启动播放列表初始化）
        if (CRecentList::Instance().IsCurrentList(list_item))
        {
            if (!CPlayer::GetInstance().SetContainSubFolder())
            {
                const wstring& info = theApp.m_str_table.LoadText(L"MSG_WAIT_AND_RETRY");
                AfxMessageBox(info.c_str(), MB_ICONINFORMATION | MB_OK);
            }
        }
        else
        {
            CRecentList::Instance().SetContainSubFolder(list_item);
        }

    }
    else if (command == ID_CLEAR_INVALID_PATH)
    {
        const wstring& inquiry_info = theApp.m_str_table.LoadText(L"MSG_LIB_PATH_CLEAR_INQUIRY");
        if (AfxMessageBox(inquiry_info.c_str(), MB_ICONQUESTION | MB_OKCANCEL) == IDCANCEL)
            return;
        int cleard_cnt = CRecentList::Instance().RemoveItemIf([](const ListItem& list_item)
            { return list_item.type == LT_FOLDER && !CAudioCommon::IsPathContainsAudioFile(list_item.path, list_item.contain_sub_folder); });
        CRecentList::Instance().SaveData();
        wstring complete_info = theApp.m_str_table.LoadTextFormat(L"MSG_LIB_PATH_CLEAR_COMPLETE", { cleard_cnt });
        AfxMessageBox(complete_info.c_str(), MB_ICONINFORMATION | MB_OK);
    }
    else if (command == ID_FILE_OPEN_FOLDER)
    {
        helper.OnOpenFolder();
    }
    else if (command == ID_LIB_FOLDER_PROPERTIES)
    {
        CFolderPropertiesDlg dlg(list_item);
        dlg.DoModal();
    }
    //添加到新播放列表
    else if (command == ID_ADD_TO_NEW_PLAYLIST)
    {
        wstring playlist_path;
        helper.OnAddToNewPlaylist(getSongList, playlist_path, CFilePathHelper(list_item.path).GetFolderName());
    }
    //添加到播放列表
    else
    {
        helper.OnAddToPlaylistCommand(getSongList, command);
    }
}

void CUIWindowCmdHelper::OnMediaLibPlaylistCommand(UiElement::MediaLibPlaylist* medialib_folder, DWORD command)
{
    int item_selected{ medialib_folder->GetItemSelected() };
    ListItem list_item = medialib_folder->m_list_cache.GetItem(item_selected);
    CMusicPlayerCmdHelper helper;

    if (command == ID_PLAY_PLAYLIST)
    {
        helper.OnListItemSelected(list_item, true);
    }
    else if (command == ID_RENAME_PLAYLIST)
    {
        helper.OnRenamePlaylist(list_item);
    }
    else if (command == ID_DELETE_PLAYLIST)
    {
        helper.OnDeleteRecentListItem(list_item);
    }
    else if (command == ID_SAVE_AS_NEW_PLAYLIST)
    {
        helper.OnNewPlaylist(list_item.path);
    }
    else if (command == ID_PLAYLIST_SAVE_AS)
    {
        helper.OnPlaylistSaveAs(list_item.path);

    }
    else if (command == ID_PLAYLIST_BROWSE_FILE)
    {
        if (!list_item.path.empty())
        {
            CString str;
            str.Format(_T("/select,\"%s\""), list_item.path.c_str());
            ShellExecute(NULL, _T("open"), _T("explorer"), str, NULL, SW_SHOWNORMAL);
        }
    }
    else if (command == ID_PLAYLIST_FIX_PATH_ERROR)
    {
        helper.OnPlaylistFixPathError(list_item.path);
    }
    else if (command == ID_NEW_PLAYLIST)
    {
        helper.OnNewPlaylist();
    }
    else if (command == ID_LIB_PLAYLIST_PROPERTIES)
    {
        CPlaylistPropertiesDlg dlg(list_item);
        dlg.DoModal();
    }
}

bool CUIWindowCmdHelper::OnSongListCommand(const std::vector<SongInfo>& songs, DWORD command)
{
    if (songs.empty())
        return false;

    auto getSongList = [&](std::vector<SongInfo>& song_list) {
        song_list = songs;
    };

    CMusicPlayerCmdHelper helper;
    //下一首播放
    if (command == ID_PLAY_AS_NEXT)
    {
        CPlayer::GetInstance().PlayAfterCurrentTrack(songs);
    }
    //在文件夹模式中播放
    else if (command == ID_PLAY_ITEM_IN_FOLDER_MODE)
    {
        if (!CPlayer::GetInstance().OpenASongInFolderMode(songs.front(), true))
        {
            const wstring& info = theApp.m_str_table.LoadText(L"MSG_WAIT_AND_RETRY");
            AfxMessageBox(info.c_str(), MB_ICONINFORMATION | MB_OK);
        }
    }
    //在线查看
    else if (command == ID_EXPLORE_ONLINE)
    {
        static SongInfo song_view_online;
        song_view_online = songs.front();
        AfxBeginThread(CMusicPlayerCmdHelper::ViewOnlineThreadFunc, (LPVOID)&song_view_online);
    }
    //格式转换
    else if (command == ID_FORMAT_CONVERT)
    {
        helper.FormatConvert(songs);
    }
    //打开文件位置
    else if (command == ID_EXPLORE_TRACK)
    {
        CString str;
        str.Format(_T("/select,\"%s\""), songs.front().file_path.c_str());
        ShellExecute(NULL, _T("open"), _T("explorer"), str, NULL, SW_SHOWNORMAL);
    }
    //从磁盘删除
    else if (command == ID_DELETE_FROM_DISK)
    {
        helper.DeleteSongsFromDisk(songs);
    }
    //分级
    else if ((command >= ID_RATING_1 && command <= ID_RATING_5) || command == ID_RATING_NONE)    //如果命令是歌曲分级（应确保分级命令的ID是连续的）
    {
        helper.OnRating(songs.front(), command);
    }
    //添加到新播放列表
    else if (command == ID_ADD_TO_NEW_PLAYLIST)
    {
        wstring playlist_path;
        helper.OnAddToNewPlaylist(getSongList, playlist_path);
    }
    //添加到播放列表
    else
    {
        return helper.OnAddToPlaylistCommand(getSongList, command);
    }
    return true;
}

void CUIWindowCmdHelper::OnMyFavouriteListCommand(UiElement::MyFavouriteList* my_favourite_list, DWORD command)
{
    std::vector<int> indexes;
    my_favourite_list->GetItemsSelected(indexes);
    vector<SongInfo> songs;
    for (int i : indexes)
    {
        if (i >= 0 && i < CUiMyFavouriteItemMgr::Instance().GetSongCount())
            songs.push_back(CUiMyFavouriteItemMgr::Instance().GetSongInfo(i));
    }

    if (songs.empty())
        return;

    int item_selected{ my_favourite_list->GetItemSelected() };
    SongInfo song_info{ CUiMyFavouriteItemMgr::Instance().GetSongInfo(item_selected) };

    CMusicPlayerCmdHelper helper;

    if (!OnSongListCommand(songs, command))
    {
        //播放
        if (command == ID_PLAY_ITEM)
        {
            helper.OnPlayMyFavourite(song_info);
        }
        //从列表中删除
        else if (command == ID_REMOVE_FROM_PLAYLIST)
        {
            helper.OnRemoveFromPlaylist(CRecentList::Instance().GetSpecPlaylist(CRecentList::PT_FAVOURITE), songs);
        }
        //属性
        else if (command == ID_ITEM_PROPERTY)
        {
            if (my_favourite_list->IsMultipleSelected())
            {
                CPropertyDlg dlg(songs);
                dlg.DoModal();
            }
            else
            {
                std::vector<SongInfo> song_list;
                CUiMyFavouriteItemMgr::Instance().GetSongList(song_list);
                CPropertyDlg dlg(song_list, item_selected, false);
                dlg.DoModal();
            }
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
    std::vector<int> indexes;
    all_tracks_list->GetItemsSelected(indexes);
    vector<SongInfo> songs;
    for (int i : indexes)
    {
        if (i >= 0 && i < CUiAllTracksMgr::Instance().GetSongCount())
            songs.push_back(CUiAllTracksMgr::Instance().GetSongInfo(i));
    }

    if (songs.empty())
        return;

    int item_selected{ all_tracks_list->GetItemSelected() };

    CMusicPlayerCmdHelper helper;

    if (!OnSongListCommand(songs, command))
    {
        //播放
        if (command == ID_PLAY_ITEM)
        {
            const SongInfo& song_info = CUiAllTracksMgr::Instance().GetSongInfo(item_selected);
            helper.OnPlayAllTrack(song_info);
        }
        //属性
        else if (command == ID_ITEM_PROPERTY)
        {
            if (all_tracks_list->IsMultipleSelected())
            {
                CPropertyDlg dlg(songs);
                dlg.DoModal();
            }
            else
            {
                std::vector<SongInfo> song_list;
                CUiAllTracksMgr::Instance().GetSongList(song_list);
                CPropertyDlg dlg(song_list, item_selected, false);
                dlg.DoModal();
            }
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

void CUIWindowCmdHelper::OnMedialibFolderExploreCommand(UiElement::FolderExploreTree* folder_explore, DWORD command)
{
    std::wstring folder_path = folder_explore->GetSelectedPath();
    if (folder_path.empty())
        return;

    std::wstring folder_name = folder_explore->GetItemText(folder_explore->GetItemSelected(), UiElement::FolderExploreTree::COL_NAME);

    CMusicPlayerCmdHelper helper;

    auto getSongList = [&](std::vector<SongInfo>& song_list) {
        if (COSUPlayerHelper::IsOsuFolder(folder_path))
            COSUPlayerHelper::GetOSUAudioFiles(folder_path, song_list);
        else
            CAudioCommon::GetAudioFiles(folder_path, song_list, MAX_SONG_NUM, true);
    };

    //播放
    if (command == ID_PLAY_ITEM)
    {
        helper.OnOpenFolder(folder_path, true, true);
    }
    //添加到新播放列表并播放
    else if (command == ID_ADD_TO_NEW_PLAYLIST_AND_PLAY)
    {
        wstring playlist_path;
        if (helper.OnAddToNewPlaylist(getSongList, playlist_path, folder_name))
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
        if (!CCommon::CopyStringToClipboard(folder_name))
            AfxMessageBox(theApp.m_str_table.LoadText(L"MSG_COPY_CLIPBOARD_FAILED").c_str(), MB_ICONWARNING);
    }
    //打开文件夹位置
    else if (command == ID_BROWSE_PATH)
    {
        ShellExecute(NULL, _T("open"), _T("explorer"), folder_path.c_str(), NULL, SW_SHOWNORMAL);
    }
    //添加到新播放列表
    else if (command == ID_ADD_TO_NEW_PLAYLIST)
    {
        wstring playlist_path;
        helper.OnAddToNewPlaylist(getSongList, playlist_path, folder_name);
    }
    //添加到播放列表
    else
    {
        helper.OnAddToPlaylistCommand(getSongList, command);
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

void CUIWindowCmdHelper::OnFolderOrPlaylistSortCommand(DWORD command)
{
    //文件夹-最近播放
    if (command == ID_LIB_FOLDER_SORT_RECENT_PLAYED)
    {
        CRecentList::Instance().SetSortMode(LT_FOLDER, CRecentList::listSortMode::SM_RECENT_PLAYED);
    }
    //文件夹-最近添加
    else if (command == ID_LIB_FOLDER_SORT_RECENT_ADDED)
    {
        CRecentList::Instance().SetSortMode(LT_FOLDER, CRecentList::listSortMode::SM_RECENT_CREATED);
    }
    //文件夹-路径
    else if (command == ID_LIB_FOLDER_SORT_PATH)
    {
        CRecentList::Instance().SetSortMode(LT_FOLDER, CRecentList::listSortMode::SM_PATH);
    }
    //播放列表-最近播放
    else if (command == ID_LIB_PLAYLIST_SORT_RECENT_PLAYED)
    {
        CRecentList::Instance().SetSortMode(LT_PLAYLIST, CRecentList::listSortMode::SM_RECENT_PLAYED);
        theApp.m_pMainWnd->SendMessage(WM_INIT_ADD_TO_MENU);
    }
    //播放列表-最近创建
    else if (command == ID_LIB_PLAYLIST_SORT_RECENT_CREATED)
    {
        CRecentList::Instance().SetSortMode(LT_PLAYLIST, CRecentList::listSortMode::SM_RECENT_CREATED);
        theApp.m_pMainWnd->SendMessage(WM_INIT_ADD_TO_MENU);
    }
    //播放列表-名称
    else if (command == ID_LIB_PLAYLIST_SORT_NAME)
    {
        CRecentList::Instance().SetSortMode(LT_PLAYLIST, CRecentList::listSortMode::SM_PATH);
        theApp.m_pMainWnd->SendMessage(WM_INIT_ADD_TO_MENU);
    }
}

void CUIWindowCmdHelper::SetRecentPlayedListMenuState(CMenu* pMenu)
{
    UiElement::RecentPlayedList* recent_played = dynamic_cast<UiElement::RecentPlayedList*>(m_context_menu_sender);
    if (recent_played != nullptr)
    {
        int item_selected{ recent_played->GetItemSelected() };
        //最近播放中排在第一个的项目为正在播放的项目，不允许移除
        pMenu->EnableMenuItem(ID_RECENT_PLAYED_REMOVE, MF_BYCOMMAND | (item_selected > 0 ? MF_ENABLED : MF_GRAYED));
    }
}

void CUIWindowCmdHelper::SetMediaLibItemListMenuState(CMenu* pMenu)
{
    //设置“添加到播放列表”子菜单项的可用状态
    SetAddToPlaylistMenuState(pMenu);
}

void CUIWindowCmdHelper::SetMediaLibFolderMenuState(CMenu* pMenu)
{
    bool select_valid{};
    bool contain_sub_folder{};
    UiElement::MediaLibFolder* medialib_folder{ dynamic_cast<UiElement::MediaLibFolder*>(m_context_menu_sender) };
    if (medialib_folder != nullptr)
    {
        int item_selected{ medialib_folder->GetItemSelected() };
        ListItem list_item = UiElement::MediaLibFolder::m_list_cache.GetItem(item_selected);
        select_valid = !list_item.empty();
        contain_sub_folder = list_item.contain_sub_folder;
    }

    pMenu->EnableMenuItem(ID_PLAY_PATH, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_DELETE_PATH, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_BROWSE_PATH, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_CONTAIN_SUB_FOLDER, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
    pMenu->CheckMenuItem(ID_CONTAIN_SUB_FOLDER, MF_BYCOMMAND | (select_valid && contain_sub_folder ? MF_CHECKED : MF_UNCHECKED));
    pMenu->EnableMenuItem(ID_LIB_FOLDER_PROPERTIES, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));

    SetAddToPlaylistMenuState(pMenu);
}

void CUIWindowCmdHelper::SetMediaLibPlaylistMenuState(CMenu* pMenu)
{
    bool select_valid{};
    ListItem selected_playlist;
    UiElement::MediaLibPlaylist* medialib_playlist = dynamic_cast<UiElement::MediaLibPlaylist*>(m_context_menu_sender);
    if (medialib_playlist != nullptr)
    {
        int item_selected{ medialib_playlist->GetItemSelected() };
        selected_playlist = medialib_playlist->m_list_cache.GetItem(item_selected);
        select_valid = !selected_playlist.empty();
    }

    bool selected_can_play{ select_valid && !CRecentList::Instance().IsCurrentList(selected_playlist) };

    bool is_spec_playlist = CRecentList::IsSpecPlaylist(selected_playlist);
    bool is_cant_del = CRecentList::IsSpecPlaylist(selected_playlist, CRecentList::PT_DEFAULT) || CRecentList::IsSpecPlaylist(selected_playlist, CRecentList::PT_FAVOURITE);
    pMenu->EnableMenuItem(ID_RENAME_PLAYLIST, MF_BYCOMMAND | (select_valid && !is_spec_playlist ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_DELETE_PLAYLIST, MF_BYCOMMAND | (select_valid && !is_cant_del ? MF_ENABLED : MF_GRAYED));
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
    UiElement::MyFavouriteList* my_favourite_list = dynamic_cast<UiElement::MyFavouriteList*>(m_context_menu_sender);
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

    pMenu->EnableMenuItem(ID_PLAY_AS_NEXT, MF_BYCOMMAND | (selected_in_current_playing_list ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_DELETE_FROM_DISK, MF_BYCOMMAND | (can_del ? MF_ENABLED : MF_GRAYED));

    SetAddToPlaylistMenuState(pMenu);
}

void CUIWindowCmdHelper::SetAllTracksListMenuState(CMenu* pMenu)
{
    bool selected_in_current_playing_list{ false }; //选中是否正在播放
    bool can_del{ false };
    UiElement::AllTracksList* all_tracks_list = dynamic_cast<UiElement::AllTracksList*>(m_context_menu_sender);
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

    pMenu->EnableMenuItem(ID_PLAY_AS_NEXT, MF_BYCOMMAND | (selected_in_current_playing_list ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_DELETE_FROM_DISK, MF_BYCOMMAND | (can_del ? MF_ENABLED : MF_GRAYED));
    SetAddToPlaylistMenuState(pMenu);
}

void CUIWindowCmdHelper::SetAddToPlaylistMenuState(CMenu* pMenu)
{
    //判断菜单的发送者
    UiElement::Playlist* playlist{ dynamic_cast<UiElement::Playlist*>(m_context_menu_sender) };
    UiElement::MyFavouriteList* my_favourite_list{ dynamic_cast<UiElement::MyFavouriteList*>(m_context_menu_sender) };
    UiElement::ListElement* list_element{ dynamic_cast<UiElement::ListElement*>(m_context_menu_sender) };

    //设置默认状态
    if (list_element != nullptr)
    {
        bool select_valid{ list_element->GetItemSelected() >= 0 };
        for (UINT id = ID_ADD_TO_DEFAULT_PLAYLIST; id < ID_ADD_TO_MY_FAVOURITE + ADD_TO_PLAYLIST_MAX_SIZE + 1; id++)
        {
            pMenu->EnableMenuItem(id, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
        }
        pMenu->EnableMenuItem(ID_ADD_TO_NEW_PLAYLIST, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
        pMenu->EnableMenuItem(ID_ADD_TO_OTHER_PLAYLIST, MF_BYCOMMAND | (select_valid ? MF_ENABLED : MF_GRAYED));
    }
    else
    {
        for (UINT id = ID_ADD_TO_DEFAULT_PLAYLIST; id < ID_ADD_TO_MY_FAVOURITE + ADD_TO_PLAYLIST_MAX_SIZE + 1; id++)
        {
            pMenu->EnableMenuItem(id, MF_BYCOMMAND | MF_ENABLED);
        }
        pMenu->EnableMenuItem(ID_ADD_TO_NEW_PLAYLIST, MF_BYCOMMAND | MF_ENABLED);
        pMenu->EnableMenuItem(ID_ADD_TO_OTHER_PLAYLIST, MF_BYCOMMAND | MF_ENABLED);
    }

    //设置播放列表和我喜欢的音乐“添加到”子菜单的状态
    wstring disable_item_name;
    if (my_favourite_list)
    {
        disable_item_name = theApp.m_str_table.LoadMenuText(L"ADD_TO_PLAYLIST", L"ID_ADD_TO_MY_FAVOURITE");
    }
    else if (playlist)
    {
        ListItem list_item = CRecentList::Instance().GetCurrentList();
        if (CRecentList::IsSpecPlaylist(list_item, CRecentList::PT_DEFAULT))
            disable_item_name = theApp.m_str_table.LoadMenuText(L"ADD_TO_PLAYLIST", L"ID_ADD_TO_DEFAULT_PLAYLIST");
        else if (CRecentList::IsSpecPlaylist(list_item, CRecentList::PT_FAVOURITE))
            disable_item_name = theApp.m_str_table.LoadMenuText(L"ADD_TO_PLAYLIST", L"ID_ADD_TO_MY_FAVOURITE");
        else if (!CRecentList::IsSpecPlaylist(list_item, CRecentList::PT_TEMP)) // 临时播放列表不会出现在菜单中，所以不用禁用
            disable_item_name = CFilePathHelper(list_item.path).GetFileNameWithoutExtension();
    }
    if (!disable_item_name.empty())
    {
        for (UINT id = ID_ADD_TO_DEFAULT_PLAYLIST; id < ID_ADD_TO_MY_FAVOURITE + ADD_TO_PLAYLIST_MAX_SIZE + 1; id++)
        {
            CString menu_string;
            pMenu->GetMenuString(id, menu_string, 0);
            pMenu->EnableMenuItem(id, MF_BYCOMMAND | (disable_item_name != menu_string.GetString() ? MF_ENABLED : MF_GRAYED));
        }
    }
}

void CUIWindowCmdHelper::SetFolderSortMenuState(CMenu* pMenu)
{
    switch (CRecentList::Instance().GetSortMode(LT_FOLDER))
    {
    case CRecentList::listSortMode::SM_RECENT_PLAYED: pMenu->CheckMenuRadioItem(ID_LIB_FOLDER_SORT_RECENT_PLAYED, ID_LIB_FOLDER_SORT_PATH, ID_LIB_FOLDER_SORT_RECENT_PLAYED, MF_BYCOMMAND | MF_CHECKED); break;
    case CRecentList::listSortMode::SM_RECENT_CREATED: pMenu->CheckMenuRadioItem(ID_LIB_FOLDER_SORT_RECENT_PLAYED, ID_LIB_FOLDER_SORT_PATH, ID_LIB_FOLDER_SORT_RECENT_ADDED, MF_BYCOMMAND | MF_CHECKED); break;
    case CRecentList::listSortMode::SM_PATH: pMenu->CheckMenuRadioItem(ID_LIB_FOLDER_SORT_RECENT_PLAYED, ID_LIB_FOLDER_SORT_PATH, ID_LIB_FOLDER_SORT_PATH, MF_BYCOMMAND | MF_CHECKED); break;
    }
}

void CUIWindowCmdHelper::SetPlaylistSortMenuState(CMenu* pMenu)
{
    switch (CRecentList::Instance().GetSortMode(LT_PLAYLIST))
    {
    case CRecentList::listSortMode::SM_RECENT_PLAYED: pMenu->CheckMenuRadioItem(ID_LIB_PLAYLIST_SORT_RECENT_PLAYED, ID_LIB_PLAYLIST_SORT_NAME, ID_LIB_PLAYLIST_SORT_RECENT_PLAYED, MF_BYCOMMAND | MF_CHECKED); break;
    case CRecentList::listSortMode::SM_RECENT_CREATED: pMenu->CheckMenuRadioItem(ID_LIB_PLAYLIST_SORT_RECENT_PLAYED, ID_LIB_PLAYLIST_SORT_NAME, ID_LIB_PLAYLIST_SORT_RECENT_CREATED, MF_BYCOMMAND | MF_CHECKED); break;
    case CRecentList::listSortMode::SM_PATH: pMenu->CheckMenuRadioItem(ID_LIB_PLAYLIST_SORT_RECENT_PLAYED, ID_LIB_PLAYLIST_SORT_NAME, ID_LIB_PLAYLIST_SORT_NAME, MF_BYCOMMAND | MF_CHECKED); break;
    }
}
