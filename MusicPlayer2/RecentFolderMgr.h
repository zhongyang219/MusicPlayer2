#pragma once
#include "SongInfo.h"

//文件夹模式下一个文件夹的信息
struct PathInfo
{
    wstring path;                               // 路径
    int track{};                                // 最后播放到的曲目号
    int position{};                             // 最后播放到的位置
    SortMode sort_mode{};                       // 路径中文件的排序方式
    int track_num{};                            // 路径中音频文件的数量
    int total_time{};                           // 路径中音频文件的总时间
    bool contain_sub_folder{};                  // 是否包含子文件夹
    unsigned __int64 last_played_time{};        // 上次播放的时间
    unsigned __int64 add_time{};                // 添加时间
};

class CRecentFolderMgr
{
public:
    ~CRecentFolderMgr();
    static CRecentFolderMgr& Instance();

    //文件夹排序方式
    enum class FolderSortMode
    {
        SM_UNSORTED,        //未排序
        SM_RECENT_PLAYED,   //最近播放
        SM_RECENT_ADDED,    //最近添加
        SM_PATH             //路径
    };

    // 设置排序方式
    bool SetSortMode(FolderSortMode sort_mode);
    // 获取排序方式
    FolderSortMode GetSortMode() const;
    // 创建/更新 一个文件夹项目
    void EmplaceRecentFolder(const std::wstring& path, int track, int position, SortMode sort_mode, int track_num, int totla_time, bool contain_sub_folder);
    // 获取最近播放的文件夹项目
    PathInfo GetCurrentListInfo() const;
    // 删除指定项目
    bool RemoveItem(const std::wstring& path);
    // 删除符合条件的项目，返回已删除个数
    int RemoveItemIf(std::function<bool(const PathInfo&)> fun_condition);
    // 翻转指定PathInfo的contain_sub_folder成员
    bool SetContainSubFolder(const wstring& path);
    // 将上次播放时间清空，使它从“最近播放”中移除
    bool ResetLastPlayedTime(const std::wstring& path);

    // 遍历所有PathInfo (仍有线程安全问题，外部不应自行保存参数对象指针，接下来会解决这个)
    void IteratePathInfo(std::function<void(const PathInfo&)> func) const;

    // 获取PathInfo数量
    int GetItemSize() const;
    // 获取path指定PathInfo的副本
    PathInfo GetItem(const wstring& path) const;
    // 获取index指定PathInfo的副本
    PathInfo GetItem(int index) const;
    // 在func中读取index指定PathInfo
    void GetItem(int index, std::function<void(const PathInfo&)> func) const;

    //从文件读取数据
    bool LoadData();
    //将数据写入文件
    void SaveData() const;

private:
    CRecentFolderMgr();
    void SortPath();

    static CRecentFolderMgr m_instance;             // CRecentFolderMgr类唯一的对象
    vector<PathInfo> m_recent_path;                 // 最近打开过的路径
    FolderSortMode m_sort_mode{};
    mutable std::shared_mutex m_shared_mutex;       // 保护此类数据对象 m_recent_path & m_sort_mode 的读写
};

