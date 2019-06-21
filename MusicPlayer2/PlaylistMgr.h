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


class CPlaylistMgr
{
public:
    CPlaylistMgr();
    ~CPlaylistMgr();

    void EmplacePlaylist(const wstring& path, int track, int pos, int track_num, int total_time);
    void AddNewPlaylist(const wstring& path);

    void SavePlaylistData();
    void LoadPlaylistData();

public:
    PlaylistInfo m_default_playlist;
    std::deque<PlaylistInfo> m_recent_playlists;
    bool m_use_default_playlist{ true };
};

