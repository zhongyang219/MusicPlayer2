#pragma once

//储存路径信息
enum PlaylistType
{
    PT_USER,
    PT_DEFAULT,
    PT_FAVOURITE,
    PT_TEMP
};

struct PlaylistInfo
{
    wstring path;		    //路径
    int track{};		    //最后播放到的曲目序号
    int position{};		    //最后播放到的位置
    int track_num{};		//路径中音频文件的数量
    int total_time{};		//路径中音频文件的总时间
    unsigned __int64 last_played_time{};    //上次播放的时间
    unsigned __int64 create_time{};         //创建时间
};

class CPlaylistMgr
{
public:
    ~CPlaylistMgr();
    static CPlaylistMgr& Instance();

    enum
    {
        SPEC_PLAYLIST_NUM = 2       //特殊播放列表的个数（这里是2，默认播放列表和我喜欢的播放列表）
    };

    //播放列表排序方式
    enum PlaylistSortMode
    {
        SM_UNSORTED,        //未排序
        SM_RECENT_PLAYED,   //最近播放
        SM_RECENT_CREATED,  //最近创建
        SM_NAME             //名称
    };

    void Init();

    void EmplacePlaylist(const wstring& path, int track, int pos, int track_num, int total_time, unsigned __int64 last_played_time);
    void AddNewPlaylist(const wstring& path);
    bool DeletePlaylist(const wstring& path);
    void UpdateCurrentPlaylistType(const wstring& path);
    void UpdatePlaylistInfo(PlaylistInfo playlist_info);

    void SavePlaylistData();
    void LoadPlaylistData();

    PlaylistInfo FindPlaylistInfo(const wstring& str) const;
    PlaylistInfo GetCurrentPlaylistInfo() const;
    PlaylistType GetPlaylistType(const wstring& path) const;
    PlaylistType GetCurPlaylistType() const { return m_cur_playlist_type; }

    // 重命名播放列表后以此更新m_recent_playlists
    void RenamePlaylist(const wstring& old_path, const wstring& new_path);
    // CSelectPlaylistDlg使用此方法获取/更新显示数据
    void GetAllPlaylistInfo(vector<PlaylistInfo>& playlists_info);

    void IterateItems(std::function<void(PlaylistInfo&)> func);

    //遍历所有播放列表，不包括特殊播放列表
    //max_num：最多遍历播放列表的个数，0表示不限制
    void IterateItemsWithoutSpecialPlaylist(std::function<void(PlaylistInfo&)> func, int max_num = 0);

    //获取播放列表总数，包括特殊播放列表
    int GetPlaylistNum() const;

    //根据指定index获取一个播放列表信息，包括特殊播放列表，获取到的播放列表信息通过func的参数传递
    void GetPlaylistInfo(int index, std::function<void(const PlaylistInfo&)> func);

    //根据指定index获取一个播放列表信息，不包括特殊播放列表，获取到的播放列表信息通过func的参数传递
    void GetPlaylistInfoWithoutSpecialPlaylist(int index, std::function<void(const PlaylistInfo&)> func);

    int GetCurrentPlaylistIndex() const;

    const PlaylistInfo& GetDefaultPlaylist() const { return m_default_playlist; }
    const PlaylistInfo& GetFavouritePlaylist() const { return m_favourite_playlist; }
    const PlaylistInfo& GetTempPlaylist() const { return m_temp_playlist; }

    static std::wstring GetPlaylistDisplayName(const std::wstring path);

    bool ResetLastPlayedTime(const wstring& path);     //将上次播放时间清空，使它从“最近播放”中移除
    bool UpdatePlaylistTrackNum(const wstring& path, int track_num);  //更新一个播放列表的曲目总数

    bool SetSortMode(PlaylistSortMode sort_mode);
    PlaylistSortMode GetSortMode() const;

private:
    CPlaylistMgr();
    const PlaylistInfo& GetPlaylistInfo(int index);
    PlaylistInfo& GetPlaylistInfo(const wstring& path, bool& ok);
    void SortPlaylist();

    static CPlaylistMgr m_instance;     //CPlaylistMgr类唯一的对象
    PlaylistInfo m_default_playlist;
    PlaylistInfo m_favourite_playlist;
    PlaylistInfo m_temp_playlist;
    std::deque<PlaylistInfo> m_recent_playlists;
    PlaylistType m_cur_playlist_type{ PT_DEFAULT };
    PlaylistSortMode m_sort_mode{ SM_UNSORTED };
    mutable std::shared_mutex m_shared_mutex;
};
