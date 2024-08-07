#pragma once
#include "AudioCommon.h"

//文件夹模式下一个文件夹的信息
struct PathInfo
{
    wstring path;		//路径
    int track{};			//最后播放到的曲目号
    int position{};		//最后播放到的位置
    SortMode sort_mode{};	//路径中文件的排序方式
    int track_num{};		//路径中音频文件的数量
    int total_time{};		//路径中音频文件的总时间
    bool contain_sub_folder{};  //是否包含子文件夹
    unsigned __int64 last_played_time{};    //上次播放的时间
    unsigned __int64 add_time{};            //添加时间

    bool IsEmpty() const;
};

class CRecentFolderMgr
{
public:
    ~CRecentFolderMgr();
    static CRecentFolderMgr& Instance();

    //文件夹排序方式
    enum FolderSortMode
    {
        SM_UNSORTED,        //未排序
        SM_RECENT_PLAYED,   //最近播放
        SM_RECENT_ADDED,    //最近添加
        SM_PATH             //路径
    };

    //deque<PathInfo>& GetRecentPath() { return m_recent_path; }    //返回最近播放路径列表的引用
    bool IsEmpty() { return m_recent_path.empty(); }
    void EmplaceRecentFolder(const std::wstring& path, int track, int position, SortMode sort_mode, int track_num, int totla_time, bool contain_sub_folder);
    PathInfo& FindItem(const std::wstring& path);
    bool FindItem(const std::wstring& path, std::function<void(PathInfo&)> func);   //查找一个PathInfo对象，找到后会调用func，并通过参数传递找到的对象，func仅调用一次
    const PathInfo& GetCurrentItem();
    int GetItemSize() const;
    void IteratePathInfo(std::function<void(const PathInfo&)> func);
    PathInfo& GetItem(int index);
    void GetItem(int index, std::function<void(const PathInfo&)> func);
    bool DeleteItem(const std::wstring& path);
    int DeleteInvalidItems();
    bool ResetLastPlayedTime(const std::wstring& path);     //将上次播放时间清空，使它从“最近播放”中移除

    int GetCurrentPlaylistIndex() const;

    bool SetSortMode(FolderSortMode sort_mode);
    FolderSortMode GetSortMode() const;

    //从文件读取数据
    bool LoadData();
    //将数据写入文件
    void SaveData() const;

    static void GetFolderAudioFiles(const PathInfo& path_info, std::vector<SongInfo>& song_list);

private:
    CRecentFolderMgr();
    void SortPath();

    static CRecentFolderMgr m_instance;     //CRecentFolderMgr类唯一的对象
    deque<PathInfo> m_recent_path;      //最近打开过的路径
    FolderSortMode m_sort_mode{ SM_UNSORTED };
    mutable std::shared_mutex m_shared_mutex;

};

