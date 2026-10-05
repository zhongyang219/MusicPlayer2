#pragma once
#include "PlayTime.h"

enum eTagType
{
    T_OTHER_TAG = 0,
    T_ID3V1 = 1,
    T_ID3V2 = 1 << 1,
    T_APE = 1 << 2,
    T_RIFF = 1 << 3,
    T_MP4 = 1 << 4,
};

//排序方式
enum SortMode
{
    SM_U_FILE = 0,          // 文件名 升序
    SM_D_FILE,              // 文件名 降序
    SM_U_PATH,              // 路径 升序
    SM_D_PATH,              // 路径 降序
    SM_U_TITLE,             // 标题 升序
    SM_D_TITLE,             // 标题 降序
    SM_U_ARTIST,            // 艺术家 升序
    SM_D_ARTIST,            // 艺术家 降序
    SM_U_ALBUM,             // 专辑 升序
    SM_D_ALBUM,             // 专辑 降序
    SM_U_TRACK,             // 音轨号 升序
    SM_D_TRACK,             // 音轨号 降序
    SM_U_LISTEN,            // 累计播放时间 升序
    SM_D_LISTEN,            // 累计播放时间 降序
    SM_U_TIME,              // 修改日期 升序
    SM_D_TIME,              // 修改日期 降序
    SM_U_GENRE,             // 流派 升序
    SM_D_GENRE,             // 流派 降序
    SM_U_YEAR,              // 年份 升序
    SM_D_YEAR,              // 年份 降序
    SM_U_BITRATE,           // 比特率 升序
    SM_D_BITRATE,           // 比特率 降序

    SM_UNSORT = 100,        // 未排序（进入播放列表模式时总是设置为此排序方式，且不进行持久化）
};

struct SongKey;

//一首歌曲的信息
struct SongInfo
{
    wstring file_path{};                // 歌曲的路径
    wstring lyric_file{};               // 匹配的歌词文件的路径
    wstring title;                      // 标题
    wstring artist;                     // 艺术家
    wstring album;                      // 唱片集
    wstring comment;                    // 注释
    wstring genre;                      // 流派
    wstring cue_file_path;              // cue文件的路径
    wstring album_artist;               // 唱片集艺术家
    unsigned __int64 song_id_netease{}; // 歌曲对应的网易云音乐中的歌曲ID<仅在媒体库内使用>
    char song_id_qq_music[16]{};        // 歌曲对应的QQ音乐中的歌曲ID<仅在媒体库内使用>
    __int64 last_played_time{};         // 上次播放的时间<仅在媒体库内使用>
    unsigned __int64 modified_time{};   // 修改时间
    int track{};                        // 音轨序号
    int listen_time{};                  // 歌曲累计听的时间（单位为秒）<仅在媒体库内使用>
    int freq{};                         // 采样频率
    CPlayTime start_pos{};                   // 音频的起始位置
    CPlayTime end_pos{};                     // 音频的结束位置
    unsigned short year{};              // 年份
    short bitrate{};                    // 比特率
    WORD flags{};                       // 保存一些标志<仅在媒体库内使用>
    BYTE tag_type{};                    // 标签的类型（0：其他；1：ID3v1；2：ID3v2；3：APE）
    BYTE genre_idx{ 255 };              // 以字节表示的流派号
    bool info_acquired{ false };        // 如果已经获取到了信息，则为ture (实际上已完全没有作用，可以被modified_time==0和ChannelInfoAcquired代替，考虑移除)
    bool is_favourite{ false };         // 是否在我喜欢的音乐列表内<仅在播放列表内使用>
    bool is_cue{ false };               // 如果曲目是cue分轨，则为true
    BYTE rating{ 255 };                 // 歌曲分级<仅在媒体库内使用>
    BYTE bits{};                        // 位深度
    BYTE channels{};                    // 声道数
    BYTE total_tracks{};                // 曲目总数
    BYTE disc_num{};                    // CD序号
    BYTE total_discs{};                 // CD总数
    bool is_prefered{};                 // 有同一首曲目的多个版本时，是否为选择的版本 <仅在媒体库内使用>

    // 如果为true，则不在线下载歌词<flags bit0>
    bool NoOnlineLyric() const;
    // 如果为true，则不在线下载歌词<flags bit0>
    void SetNoOnlineLyric(bool val);
    // 如果为true，则不在线下载专辑封面<flags bit1>
    bool NoOnlineAlbumCover() const;
    // 如果为true，则不在线下载专辑封面<flags bit1>
    void SetNoOnlineAlbumCover(bool val);
    // 如果为true，则总是使用外部封面<flags bit2>
    bool AlwaysUseExternalAlbumCover() const;
    // 如果为true，则总是使用外部封面<flags bit2>
    void SetAlwaysUseExternalAlbumCover(bool val);
    // 采样率、位深度、声道数信息是否已获取<flags bit3>
    bool ChannelInfoAcquired() const;
    // 采样率、位深度、声道数信息是否已获取<flags bit3>
    void SetChannelInfoAcquired(bool val);

    //从另一个SongInfo对象复制标签信息
    void CopyAudioTag(const SongInfo& song_info);
    bool IsTagEmpty() const;

    bool IsTitleEmpty() const;
    bool IsArtistEmpty() const;
    bool IsAlbumEmpty() const;
    bool IsYearEmpty() const;
    bool IsGenreEmpty() const;

    // 获取艺术家列表（可能存在多个艺术家）
    void GetArtistList(vector<wstring>& artist_list) const;

    // 获取首个艺术家
    wstring GetFirstArtist() const;

    wstring GetTitle() const;
    wstring GetArtist() const;
    wstring GetAlbum() const;
    wstring GetYear() const;
    wstring get_year() const;
    wstring GetGenre() const;
    wstring GetFileName() const;
    wstring GetSongId() const;

    void SetYear(const wchar_t* str_year);
    void SetSongId(const wstring& id);

    bool IsEmpty() const;
    CPlayTime length() const;
    // 清除歌曲信息中的<>内的默认字符串
    void Normalize();

    // 获取SongInfo的排序谓词方法
    static std::function<bool(const SongInfo& a, const SongInfo& b)> GetSortFunc(SortMode sort_mode);
    // 获取排序方式的显示名称
    static wstring GetSortModeDisplayName(SortMode sort_mode);

    bool operator==(const SongKey& other) const;
    // 判断是否为同一歌曲，原比较代码有漏洞，请迁移所有比较到使用此方法
    bool operator==(const SongInfo& other) const;
};

struct SongKey
{
    wstring path;
    int cue_track{};    // 当存储cue时用来保存音轨号，其他情况为0

    SongKey() {}
    SongKey(const wstring& path) : path(path)
    {
        ASSERT(!path.empty());
    }
    SongKey(const wstring& path, const int& cue_track) : path(path), cue_track(cue_track)
    {
        ASSERT(!path.empty());
    }
    SongKey(const SongInfo& song_info)
    {
        ASSERT(!song_info.file_path.empty());
        path = song_info.file_path;
        if (song_info.is_cue)
            cue_track = song_info.track;
    }
    bool operator<(const SongKey& key) const
    {
        if (int pathComparison = path.compare(key.path))
            return pathComparison < 0;
        return cue_track < key.cue_track;
    }
    bool operator==(const SongKey& other) const
    {
        return cue_track == other.cue_track && path == other.path;
    }
    bool operator==(const SongInfo& other) const
    {
        if ((cue_track > 0) != other.is_cue)                // is_cue不同
            return false;
        if (path != other.file_path)                        // 路径不同
            return false;
        if ((cue_track > 0) && cue_track != other.track)    // is_cue为true时音轨号不同
            return false;
        return true;
    }
};

namespace std {
    template <>
    struct hash<SongKey> {
        std::size_t operator()(const SongKey& key) const {
            return std::hash<wstring>()(key.path) ^ std::hash<int>()(key.cue_track);
        }
    };
    template <>
    struct equal_to<SongKey> {
        bool operator()(const SongKey& lhs, const SongKey& rhs) const {
            return lhs.path == rhs.path && lhs.cue_track == rhs.cue_track;
        }
    };
}
