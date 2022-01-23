#include "stdafx.h"
#include "RecentFolderAndPlaylist.h"
#include "FilePathHelper.h"

CRecentFolderAndPlaylist CRecentFolderAndPlaylist::m_instance;

CRecentFolderAndPlaylist::CRecentFolderAndPlaylist()
{

}

CRecentFolderAndPlaylist& CRecentFolderAndPlaylist::Instance()
{
    return m_instance;
}

void CRecentFolderAndPlaylist::Init(const deque<PathInfo>& recent_folder, const CPlaylistMgr& recent_playlist)
{
    m_list.clear();
    //添加最近播放播放列表
    m_list.emplace_back(&recent_playlist.m_default_playlist);
    m_list.emplace_back(&recent_playlist.m_favourite_playlist);
    m_list.emplace_back(&recent_playlist.m_temp_playlist);
    for (auto& item : recent_playlist.m_recent_playlists)
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
    ::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_RECENT_FOLSER_OR_PLAYLIST_CHANGED, 0, 0);
    int a = 0;
}

const std::vector<CRecentFolderAndPlaylist::Item>& CRecentFolderAndPlaylist::GetItemList() const
{
    return m_list;
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
CRecentFolderAndPlaylist::Item::Item(const PathInfo* _folder_info)
{
    is_playlist = false;
    folder_info = _folder_info;
}

CRecentFolderAndPlaylist::Item::Item(const PlaylistInfo* _playlist_info)
{
    is_playlist = true;
    playlist_info = _playlist_info;
}

unsigned __int64 CRecentFolderAndPlaylist::Item::LastPlayedTime() const
{
    if (is_playlist)
    {
        if (playlist_info != nullptr)
            return playlist_info->last_played_time;
    }
    else
    {
        if (folder_info != nullptr)
            return folder_info->last_played_time;
    }
    return 0;
}

std::wstring CRecentFolderAndPlaylist::Item::GetName() const
{
    if (is_playlist)
    {
        if (playlist_info != nullptr)
        {
            CFilePathHelper path_helper{ playlist_info->path };
            wstring playlist_name = path_helper.GetFileName();
            if (playlist_name == DEFAULT_PLAYLIST_NAME)
                playlist_name = CCommon::LoadText(_T("["), IDS_DEFAULT, _T("]"));
            else if (playlist_name == FAVOURITE_PLAYLIST_NAME)
                playlist_name = CCommon::LoadText(_T("["), IDS_MY_FAVURITE, _T("]"));
            else if (playlist_name == TEMP_PLAYLIST_NAME)
                playlist_name = CCommon::LoadText(_T("["), IDS_TEMP_PLAYLIST, _T("]"));
            else
                playlist_name = path_helper.GetFileNameWithoutExtension();
            return playlist_name;
        }
    }
    else
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
    return wstring();
}
