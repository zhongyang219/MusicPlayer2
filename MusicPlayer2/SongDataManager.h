#pragma once
#include "SongInfo.h"

class CSongDataManager
{
public:
    ~CSongDataManager();

    using SongDataMap = std::unordered_map<SongKey, SongInfo>;

    static CSongDataManager& GetInstance();
    void SaveSongData(std::wstring path);       //将所有歌曲信息以序列化的方式保存到文件
    void LoadSongData(std::wstring path);       //从文件中以序列化的方式读取所有歌曲信息

    bool IsSongDataModified() const;

    CString GetDataVersion() const;

    // 设置歌曲ID
    bool SetSongID(const SongKey& key, const unsigned __int64 id);
    // 获取歌曲ID
    bool GetSongID(const SongKey& key, unsigned __int64& id) const;

    // CAudioCommon::GetCueTracks用来将获取的信息存入媒体库（专用，其他位置多半不合适）
    void SaveCueSongInfo(const vector<SongInfo>& songs_info);
    // 从媒体库加载信息更新到播放列表项目（仅加载播放列表需要的属性）
    void LoadSongInfo(SongInfo& song_info) const;
    void LoadSongsInfo(vector<SongInfo>& song_info) const;

    // 获取一个媒体库歌曲信息（不存在会返回和参数song一致的SongInfo）
    // 至少要保证用于查询的file_path,is_cue,track是正确的
    // 用于修改媒体库的歌曲属性，修改后需使用CSongDataManager::AddItem保存
    SongInfo GetSongInfo3(const SongInfo& song) const;

    SongInfo GetSongInfo(const SongKey& key) const;

    // 用于外部读取m_song_data，加读锁后以const m_song_data&为参数调用func
    // 请勿在func中试图修改媒体库以避免未定义行为
    void GetSongData(const std::function<void(const CSongDataManager::SongDataMap&)>& func) const;

    bool IsItemExist(const SongKey& key) const;
    void AddItem(const SongInfo& song);
    bool RemoveItem(const SongKey& key);
    int RemoveItemIf(std::function<bool(const SongInfo&)>& fun_condition);       //删除符合条件的项目，返回已删除个数

    void ClearPlayTime();       //清除播放时间统计数据
    void ClearLastPlayedTime();     //清除上次播放时间

    // 创建旧媒体库条目的新路径副本（不能用于cue文件）
    void ChangeFilePath(const wstring& file_path, const wstring& new_path);

private:
    CSongDataManager();

    static CSongDataManager m_instance;

private:
    SongDataMap m_song_data;        //储存所有歌曲信息数据的映射容器，键是每一个音频文件的绝对路径，对象是每一个音频文件的信息
    std::atomic<bool> m_song_data_modified{ false };
    CString m_data_version;
    // 用于保证m_song_data读写的线程安全，遍历/查找加读锁，添加/删除加写锁
    mutable std::shared_mutex m_shared_mutex;   // 线程同步对象
};
