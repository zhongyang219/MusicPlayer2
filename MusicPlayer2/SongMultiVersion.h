#pragma once
#include "SongInfo.h"
#include "WildcardMap.h"

//用于管理当前播放列表中同一首歌曲的不同版本
class CSongMultiVersion
{
public:
    /**
     * @brief   查找一个曲目列表中同一个曲目的不同版本，并将它们合并，（歌曲的标题、艺术家、唱片集相同则认为是同一个曲目的不同版本）
     * @param[in]   songs 要合并的曲目列表
     */
    void MergeSongsMultiVersion(std::vector<SongInfo>& songs, int& percent);

    /**
     * @brief   当正在播放队列的一个曲目有多个版本时，选择曲目的一个版本
     * @param   index 曲目版本序号
     * @param   cur_song 正在播放队列中的曲目（将被替换为选中的版本）
     */
    void SelectSongsMultiVersion(int index, SongInfo& cur_song);

    /**
     * @brief   当正在播放队列的一个曲目有多个版本时，获取当前选中的版本
     * @param   cur_song 正在播放队列中的曲目
     * @return 
     */
    int GetSongMultiVersionIndex(const SongInfo& cur_song);

    /**
     * @brief   获取播放队列的一个曲目的多个版本
     * @param   song 正在播放队列中的曲目
     * @return 
     */
    const std::vector<SongInfo>& GetSongsMultiVersion(const SongInfo& song);

    /**
     * @brief   是否为空（正在播放队列中没有曲目有多个版本）
     * @return 
     */
    bool IsEmpty() const;

    //清除曲目的多个版本
    void Clear();

protected:
    std::wstring MakeKey(const SongInfo& song_info);

    //从SongDataMap中查询是否为选中的曲目
    bool IsSongPrefered(const SongInfo& song_info);

    //将是否选中标志写入到SongDataMap
    void SetSongPrefered(const SongInfo& song_info, bool is_prefered);

    CWildcardMap<std::vector<SongInfo>> m_duplicate_songs;    //保存同一曲目的不同版本（key 为 SongInfo 中的 "title|artist|album"）
};

///////////////////////////////////////////////////////////////////////////////////////////
class CSongMultiVersionManager
{
public:
    static CSongMultiVersion& PlaylistMultiVersionSongs();

private:
    static CSongMultiVersion m_playlist_multi_version_songs;
};
