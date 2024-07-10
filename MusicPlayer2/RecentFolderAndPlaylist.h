#pragma once
#include "AudioCommon.h"
#include "PlaylistMgr.h"
#include "MediaLibPlaylistMgr.h"

class CRecentFolderAndPlaylist
{
public:
    static CRecentFolderAndPlaylist& Instance();

    struct Item
    {
        enum Type
        {
            FOLDER,
            PLAYLIST,
            MEDIA_LIB
        };
        Type item_type{};
        const PathInfo* folder_info{};
        const PlaylistInfo* playlist_info{};
        const MediaLibPlaylistInfo* medialib_info{};
        bool IsPlaylist() const;
        bool IsFolder() const;
        bool IsMedialib() const;

        Item(const PathInfo* _folder_info);
        Item(const PlaylistInfo* _playlist_info);
        Item(const MediaLibPlaylistInfo* _medialib_info);
        unsigned __int64 LastPlayedTime() const;
        std::wstring GetName() const;
        bool IsItemCurrentPlaying() const;      //判断是否为当前播放的文件夹/播放列表
    };

    // 此方法会在更新数据后向主窗口SendMessage以更新“列表快捷切换”菜单和“添加到播放列表”菜单
    void Init();
    const std::vector<Item>& GetItemList() const;

private:
    CRecentFolderAndPlaylist();
    std::vector<Item> m_list;

private:
    static CRecentFolderAndPlaylist m_instance;
};
