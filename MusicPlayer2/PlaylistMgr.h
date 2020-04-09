#pragma once

//储存路径信息
struct PlaylistInfo
{
    wstring path;		    //路径
    int track{};		    //最后播放到的曲目序号
    int position{};		    //最后播放到的位置
    int track_num{};		//路径中音频文件的数量
    int total_time{};		//路径中音频文件的总时间
};

enum PlaylistType
{
    PT_USER,
    PT_DEFAULT,
    PT_FAVOURITE,
    PT_TEMP
};

class CPlaylistMgr
{
public:
    CPlaylistMgr();
    ~CPlaylistMgr();

    void EmplacePlaylist(const wstring& path, int track, int pos, int track_num, int total_time);
    void AddNewPlaylist(const wstring& path);
    bool DeletePlaylist(const wstring& path);
    void UpdateCurrentPlaylist(int track, int pos, int track_num, int total_time);

    void SavePlaylistData();
    void LoadPlaylistData();

    PlaylistInfo FindPlaylistInfo(const wstring& str);

public:
    PlaylistInfo m_default_playlist;
    PlaylistInfo m_favourite_playlist;
    PlaylistInfo m_temp_playlist;
    std::deque<PlaylistInfo> m_recent_playlists;
    PlaylistType m_cur_playlist_type{ PT_DEFAULT };
};

