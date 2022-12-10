#pragma once
#include "SongInfo.h"
#include <functional>

class CSongDataManager
{
public:
    ~CSongDataManager();

    struct SongDataMapKey
    {
        wstring path;
        int cue_track{};    // 当存储cue时用来保存音轨号，其他情况为0

        SongDataMapKey() {}
        SongDataMapKey(const wstring& path) :path(path)
        {
            ASSERT(!path.empty());
        }
        SongDataMapKey(const wstring& path, const int& cue_track) :path(path), cue_track(cue_track)
        {
            ASSERT(!path.empty());
        }
        SongDataMapKey(const SongInfo& song_info)
        {
            ASSERT(!song_info.file_path.empty());
            path = song_info.file_path;
            if (song_info.is_cue)
                cue_track = song_info.track;
        }

        bool operator==(const SongDataMapKey& b) const
        {
            return path == b.path && cue_track == b.cue_track;
        }
    };

    struct SongDataMapKey_Hash
    {
        size_t operator()(const SongDataMapKey& key) const
        {
            return std::hash<wstring>()(key.path) ^ std::hash<int>()(key.cue_track);
        }
    };

    using SongDataMap = std::unordered_map<SongDataMapKey, SongInfo, SongDataMapKey_Hash>;

    static CSongDataManager& GetInstance();
    void SaveSongData(std::wstring path);       //将所有歌曲信息以序列化的方式保存到文件
    void LoadSongData(std::wstring path);       //从文件中以序列化的方式读取所有歌曲信息

    void SetSongDataModified();
    bool IsSongDataModified() const;

    CString GetDataVersion() const;
    // 将一个歌曲信息保存到m_song_data中
    void SaveSongInfo(const SongInfo& song_info);
    // 从媒体库加载信息更新到播放列表项目
    void LoadSongInfo(SongInfo& song_info);

    SongInfo GetSongInfo(const SongDataMapKey& key) const;
    // 获取一个媒体库歌曲信息（不存在会返回和参数song一致的SongInfo）
    // 至少要保证用于查询的file_path,is_cue,track是正确的
    // 用于修改媒体库的歌曲属性，修改后需使用CSongDataManager::AddItem保存
    SongInfo GetSongInfo3(const SongInfo& song) const;

    const SongDataMap& GetSongData();
    bool IsItemExist(const SongDataMapKey& key) const;
    void AddItem(const SongInfo& song);
    bool RemoveItem(const SongDataMapKey& key);
    int RemoveItemIf(std::function<bool(const SongInfo&)> fun_condition);       //删除符合条件的项目，返回已删除个数

    void ClearPlayTime();       //清除播放时间统计数据
    void ClearLastPlayedTime();     //清除上次播放时间

    void ChangeFilePath(const wstring& file_path, const wstring& new_path);

private:
    CSongDataManager();

    static CSongDataManager m_instance;

private:
    SongDataMap m_song_data;        //储存所有歌曲信息数据的映射容器，键是每一个音频文件的绝对路径，对象是每一个音频文件的信息
    bool m_song_data_modified{};
    CString m_data_version;
    CCriticalSection m_critical;    //线程同步对象
};
