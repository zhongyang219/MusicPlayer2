#include "stdafx.h"
#include "RecentFolderAndPlaylist.h"
#include "FilePathHelper.h"
#include "Player.h"
#include "MusicPlayer2.h"
#include "MediaLibPlaylistMgr.h"
#include "RecentFolderMgr.h"

CRecentFolderAndPlaylist CRecentFolderAndPlaylist::m_instance;

CRecentFolderAndPlaylist::CRecentFolderAndPlaylist()
{

}

CRecentFolderAndPlaylist& CRecentFolderAndPlaylist::Instance()
{
    return m_instance;
}

void CRecentFolderAndPlaylist::Init()
{
    const deque<PathInfo>& recent_folder{ CRecentFolderMgr::Instance().GetRecentPath()};
    const CPlaylistMgr& recent_playlist{ CPlaylistMgr::Instance() };
    m_list.clear();
    //添加最近播放播放列表
    auto addPlaylist = [this](const PlaylistInfo* playlist) {
        //只添加播放过的播放列表
        if (playlist->last_played_time > 0)
            m_list.emplace_back(playlist);
        };
    addPlaylist(&recent_playlist.m_default_playlist);
    addPlaylist(&recent_playlist.m_favourite_playlist);
    if (recent_playlist.m_temp_playlist.track_num > 0)          // 忽略没有文件的临时播放列表
        addPlaylist(&recent_playlist.m_temp_playlist);
    for (auto& item : recent_playlist.m_recent_playlists)
        addPlaylist(&item);

    //添加最近播放媒体库项目
    for (const auto& item : CMediaLibPlaylistMgr::Instance().GetAllItems())
        m_list.emplace_back(&item);

    //添加最近播放文件夹
    for (auto& item : recent_folder)
        m_list.emplace_back(&item);

    //按最近播放时间排序
    std::sort(m_list.begin(), m_list.end(), [](const Item& item1, const Item& item2)->bool
        {
            return item1.LastPlayedTime() > item2.LastPlayedTime();
        });
    //向主窗口发送通知
    ::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_RECENT_FOLDER_OR_PLAYLIST_CHANGED, 0, 0);  // 重新初始化快捷菜单
    ::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_INIT_ADD_TO_MENU, 0, 0);   // 重新初始化右键菜单中的“添加到播放列表”子菜单
}

const std::vector<CRecentFolderAndPlaylist::Item>& CRecentFolderAndPlaylist::GetItemList() const
{
    return m_list;
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
bool CRecentFolderAndPlaylist::Item::IsPlaylist() const
{
    return item_type == PLAYLIST;
}

bool CRecentFolderAndPlaylist::Item::IsFolder() const
{
    return item_type == FOLDER;
}

bool CRecentFolderAndPlaylist::Item::IsMedialib() const
{
    return item_type == MEDIA_LIB;
}

CRecentFolderAndPlaylist::Item::Item(const PathInfo* _folder_info)
{
    item_type = Item::FOLDER;
    folder_info = _folder_info;
}

CRecentFolderAndPlaylist::Item::Item(const PlaylistInfo* _playlist_info)
{
    item_type = Item::PLAYLIST;
    playlist_info = _playlist_info;
}

CRecentFolderAndPlaylist::Item::Item(const MediaLibPlaylistInfo* _medialib_info)
{
    item_type = Item::MEDIA_LIB;
    medialib_info = _medialib_info;
}

unsigned __int64 CRecentFolderAndPlaylist::Item::LastPlayedTime() const
{
    if (item_type == Item::PLAYLIST)
    {
        if (playlist_info != nullptr)
            return playlist_info->last_played_time;
    }
    else if (item_type == Item::FOLDER)
    {
        if (folder_info != nullptr)
            return folder_info->last_played_time;
    }
    else if (item_type == Item::MEDIA_LIB)
    {
        if (medialib_info != nullptr)
            return medialib_info->last_played_time;
    }
    return 0;
}

std::wstring CRecentFolderAndPlaylist::Item::GetName() const
{
    if (item_type == Item::PLAYLIST)
    {
        if (playlist_info != nullptr)
        {
            CFilePathHelper path_helper{ playlist_info->path };
            wstring playlist_name = path_helper.GetFileName();
            if (playlist_name == DEFAULT_PLAYLIST_NAME)
                playlist_name = theApp.m_str_table.LoadText(L"TXT_PLAYLIST_NAME_DEFAULT");
            else if (playlist_name == FAVOURITE_PLAYLIST_NAME)
                playlist_name = theApp.m_str_table.LoadText(L"TXT_PLAYLIST_NAME_FAVOURITE");
            else if (playlist_name == TEMP_PLAYLIST_NAME)
                playlist_name = theApp.m_str_table.LoadText(L"TXT_PLAYLIST_NAME_TEMP");
            else
                playlist_name = path_helper.GetFileNameWithoutExtension();
            return playlist_name;
        }
    }
    else if (item_type == Item::FOLDER)
    {
        if (folder_info != nullptr)
        {
            //CFilePathHelper path_helper(folder_info->path);
            //return path_helper.GetFolderName();
            wstring path_name = folder_info->path;
            path_name.pop_back();
            return path_name;
        }
    }
    else if (item_type == Item::MEDIA_LIB)
    {
        if (medialib_info != nullptr)
        {
            return CMediaLibPlaylistMgr::GetMediaLibItemDisplayName(medialib_info->medialib_type, medialib_info->path);
        }
    }
    return wstring();
}

bool CRecentFolderAndPlaylist::Item::IsItemCurrentPlaying() const
{
    if (CPlayer::GetInstance().IsPlaylistMode())
    {
        return item_type == Item::PLAYLIST && playlist_info != nullptr && playlist_info->path == CPlayer::GetInstance().GetPlaylistPath();
    }
    else if (CPlayer::GetInstance().IsFolderMode())
    {
        return item_type == Item::FOLDER && folder_info != nullptr && folder_info->path == CPlayer::GetInstance().GetCurrentDir2();
    }
    else if (CPlayer::GetInstance().IsMediaLibMode())
    {
        return item_type == Item::MEDIA_LIB && medialib_info != nullptr && medialib_info->medialib_type == CPlayer::GetInstance().GetMediaLibPlaylistType()
            && medialib_info->path == CPlayer::GetInstance().GetMedialibItemName();
    }
    return false;
}
