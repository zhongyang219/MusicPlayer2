#include "stdafx.h"
#include "RecentFolderAndPlaylist.h"
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
    std::shared_lock<std::shared_mutex> lock(m_shared_mutex);
    m_list.clear();
    //添加最近播放播放列表
    CPlaylistMgr::Instance().IterateItems([&](const PlaylistInfo& playlist_info) {
        //只添加播放过的播放列表
        if (playlist_info.last_played_time > 0)
            m_list.emplace_back(&playlist_info);
    });

    //添加最近播放媒体库项目
    CMediaLibPlaylistMgr::Instance().IterateItems([&](const MediaLibPlaylistInfo& medialib_item_info) {
        if (medialib_item_info.last_played_time > 0)
            m_list.emplace_back(&medialib_item_info);
    });

    //添加最近播放文件夹
    CRecentFolderMgr::Instance().IteratePathInfo([&](const PathInfo& path_info) {
        if (path_info.last_played_time > 0)
            m_list.emplace_back(&path_info);
    });

    //按最近播放时间排序
    std::sort(m_list.begin(), m_list.end(), [](const Item& item1, const Item& item2)->bool
        {
            return item1.LastPlayedTime() > item2.LastPlayedTime();
        });
    //向主窗口发送通知
    ::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_RECENT_FOLDER_OR_PLAYLIST_CHANGED, 0, 0);  // 重新初始化快捷菜单
    ::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_INIT_ADD_TO_MENU, 0, 0);   // 重新初始化右键菜单中的“添加到播放列表”子菜单
    theApp.m_pMainWnd->PostMessage(WM_CLEAR_UI_SERCH_BOX, UI_LIST_TYPE_RECENT_PLAYED);   //清除UI中的搜索框
}

const std::vector<CRecentFolderAndPlaylist::Item>& CRecentFolderAndPlaylist::GetItemList() const
{
    return m_list;
}

int CRecentFolderAndPlaylist::GetSize() const
{
    std::shared_lock<std::shared_mutex> lock(m_shared_mutex);
    return m_list.size();
}

bool CRecentFolderAndPlaylist::GetItem(int index, std::function<void(const Item&)> func)
{
    std::shared_lock<std::shared_mutex> lock(m_shared_mutex);
    if (index >= 0 && index < static_cast<int>(m_list.size()))
    {
        func(m_list[index]);
        return true;
    }
    return false;
}

bool CRecentFolderAndPlaylist::RemoveItem(const Item& item)
{
    bool is_removed{};
    if (item.IsMedialib())
    {
        is_removed = CMediaLibPlaylistMgr::Instance().DeleteItem(item.medialib_info);
        CMediaLibPlaylistMgr::Instance().SavePlaylistData();
    }
    else if (item.IsFolder() && item.folder_info != nullptr)
    {
        is_removed = CRecentFolderMgr::Instance().ResetLastPlayedTime(item.folder_info->path);
        CRecentFolderMgr::Instance().SaveData();
    }
    else if (item.IsPlaylist() && item.playlist_info != nullptr)
    {
        is_removed = CPlaylistMgr::Instance().ResetLastPlayedTime(item.playlist_info->path);
        CPlaylistMgr::Instance().SavePlaylistData();
    }
    if (is_removed)
        Init();
    return is_removed;
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
            wstring playlist_name = CPlaylistMgr::GetPlaylistDisplayName(playlist_info->path);
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

int CRecentFolderAndPlaylist::Item::GetTrackNum() const
{
    if (item_type == Item::PLAYLIST)
    {
        if (playlist_info != nullptr)
            return playlist_info->track_num;
    }
    else if (item_type == Item::FOLDER)
    {
        if (folder_info != nullptr)
            return folder_info->track_num;
    }
    else if (item_type == Item::MEDIA_LIB)
    {
        if (medialib_info != nullptr)
            return medialib_info->track_num;
    }
    return 0;
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

IconMgr::IconType CRecentFolderAndPlaylist::Item::GetIcon() const
{
    if (IsPlaylist())
    {
        bool is_favourite{ playlist_info->path == CPlaylistMgr::Instance().GetFavouritePlaylist().path};
        return is_favourite ? IconMgr::IconType::IT_Favorite_On : IconMgr::IconType::IT_Playlist;
    }
    else if (IsFolder())
    {
        return IconMgr::IconType::IT_Folder;
    }
    else if (IsMedialib())
    {
        return CMediaLibPlaylistMgr::GetIcon(medialib_info->medialib_type);
    }
    return IconMgr::IT_NO_ICON;
}
