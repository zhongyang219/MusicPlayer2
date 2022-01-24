#pragma once
#include "AudioCommon.h"
#include "PlaylistMgr.h"

class CRecentFolderAndPlaylist
{
public:
    static CRecentFolderAndPlaylist& Instance();

    struct Item
    {
        bool is_playlist{};     //是否为播放列表
        const PathInfo* folder_info{};
        const PlaylistInfo* playlist_info{};

        Item(const PathInfo* _folder_info);
        Item(const PlaylistInfo* _playlist_info);
        unsigned __int64 LastPlayedTime() const;
        std::wstring GetName() const;
        bool IsItemCurrentPlaying() const;      //判断是否为当前播放的文件夹/播放列表
    };

    void Init(const deque<PathInfo>& recent_folder, const CPlaylistMgr& recent_playlist);
    const std::vector<Item>& GetItemList() const;

private:
    CRecentFolderAndPlaylist();
    std::vector<Item> m_list;

private:
    static CRecentFolderAndPlaylist m_instance;
};
