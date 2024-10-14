#pragma once
#include "SongInfo.h"
#include "IconMgr.h"

enum ListType
{
    LT_FOLDER,
    LT_PLAYLIST,
    LT_MEDIA_LIB,
    LT_MAX,
};

// ListItem描述一个“作为列表的可播放对象”的全部信息
struct ListItem
{
    ListType type{};                            // 此列表的类型
    wstring path;                               // 路径 (媒体库模式存储具体项目名)
    SortMode sort_mode{ SM_UNSORT };            // 此列表中音频的排序方式
    SongKey last_track{};                       // 最后播放到的曲目
    int last_position{};                        // 最后播放到的位置
    int total_time{};                           // 此列表中音频文件的总时长
    int total_num{};                            // 此列表中音频文件的数量
    uint64_t last_played_time{};                // 上次播放的时间 (time_t)
    uint64_t create_time{};                     // 创建/添加时间 (time_t)

    enum class ClassificationType
    {
        CT_ARTIST,
        CT_ALBUM,
        CT_GENRE,
        CT_YEAR,
        CT_TYPE,
        CT_BITRATE,
        CT_RATING,
        CT_NONE,
        CT_MAX
    };
    // type为LT_MEDIA_LIB时path为此字符串表示<其他>分类
    static inline const wstring STR_OTHER_CLASSIFY_TYPE = L"eRk0Q6ov";
    // 媒体库列表时存储列表种类 否则应保持默认
    // 例如medialib_type为CT_ARTIST，则path字段储存的是艺术家的名称
    // 特别的，CT_NONE表示所有曲目，此时path应留空
    ClassificationType medialib_type{};
    // 文件夹列表时存储是否包含子文件夹 否则应保持默认
    bool contain_sub_folder{};

    ListItem() {}
    ListItem(ListType _type, wstring _path, ClassificationType _medialib_type = {});

    bool operator==(const ListItem& other) const
    {
        return type == other.type && path == other.path && medialib_type == other.medialib_type;
    }
    bool operator<(const ListItem& other) const
    {
        return last_played_time > other.last_played_time;
    }
    bool empty() const
    {
        return type == 0 && path.empty();
    }

    // 为此ListItem设置打开时要播放的曲目，覆盖掉最后播放到的曲目
    void SetPlayTrack(const SongKey& song_key, int position = 0)
    {
        last_track = song_key;
        last_position = position;
    }

    // 当sort_mode为默认初始值SM_UNSORT时返回项目类型应有的的默认排序方式代替SM_UNSORT
    SortMode GetDefaultSortMode() const;

    // 获取项目的类型名，例如：“文件夹”、“播放列表”、“艺术家”、“唱片集”等
    wstring GetTypeDisplayName() const;
    // 获取项目的类型对应的图标类型
    IconMgr::IconType GetTypeIcon() const;
    // 获取项目具体显示名称，对LT_FOLDER返回去掉结尾'\'的path
    // LT_PLAYLIST返回文件名并处理特殊播放列表的名称转换“[默认]”、“[我喜欢的音乐]”
    // LT_MEDIA_LIB返回项目名“path”并在需要时转换特殊情况“<未知艺术家>”、“<其他>”等
    wstring GetDisplayName() const;
    // 在媒体库查询此项目上次播放曲目的SongKey，返回其用于显示的名称
    // 效率不太好，如需用于自绘UI需要在CListCache加一层缓存
    wstring GetLastTrackDisplayName() const;
};

