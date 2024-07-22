#pragma once

#include "MediaLibHelper.h"
#include "PlaylistMgr.h"
#include "CommonData.h"
#include <deque>
#include <map>
#include <set>
#include "IconMgr.h"
#include "AudioCommon.h"

//通过媒体库中艺术家、唱片集、流派、年份等界面创建的播放列表信息
struct MediaLibPlaylistInfo : public PlaylistInfo
{
    //媒体库播放列表的类型。path字段用于储存对应媒体库项目的名称，
    //例如medialib_type为CT_ARTIST，则path字段储存的是艺术家的名称
    CMediaClassifier::ClassificationType medialib_type;
    SortMode sort_mode{};	//排序方式
    bool operator==(const MediaLibPlaylistInfo& other) const;
    bool operator<(const MediaLibPlaylistInfo& other) const;
    bool isValid() const;
};


class CMediaLibPlaylistMgr
{
public:
    ~CMediaLibPlaylistMgr()
    {}

    static CMediaLibPlaylistMgr& Instance();
    static std::vector<SongInfo> GetSongList(CMediaClassifier::ClassificationType type, const std::wstring& name);
    static IconMgr::IconType GetIcon(CMediaClassifier::ClassificationType type);
    static std::wstring GetTypeName(CMediaClassifier::ClassificationType type);
    static SortMode GetDefaultSortMode(CMediaClassifier::ClassificationType type);
    //获取一个媒体库项目的显示名称（仅当媒体库项目为“<未知艺术家>”、“<其他>”等时需要特殊处理）
    static std::wstring GetMediaLibItemDisplayName(CMediaClassifier::ClassificationType type, const std::wstring medialib_item_name);

    void EmplaceMediaLibPlaylist(CMediaClassifier::ClassificationType type, const wstring& name, int track, int pos, int track_num, int total_time, unsigned __int64 last_played_time, SortMode sort_mode);
    MediaLibPlaylistInfo FindItem(CMediaClassifier::ClassificationType type, const wstring& name) const;
    MediaLibPlaylistInfo GetCurrentPlaylistInfo() const;
    bool DeleteItem(const MediaLibPlaylistInfo* item);

    void IterateItems(std::function<void(const MediaLibPlaylistInfo&)> func);

    void SavePlaylistData();
    void LoadPlaylistData();

private:
    CMediaLibPlaylistMgr()
    {}

    static CMediaLibPlaylistMgr m_instance;     //此类唯一的对象
    std::deque<MediaLibPlaylistInfo> m_media_lib_playlist;
    std::set<MediaLibPlaylistInfo> m_empty_items;   //如果一个媒体库库项目下面没有任何曲目，则会被保存到这里
    mutable std::shared_mutex m_shared_mutex;
};

