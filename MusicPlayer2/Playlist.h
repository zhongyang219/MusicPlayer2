#pragma once
#include "SongInfo.h"
class CPlaylistFile
{
public:
    enum Type           //播放列表格式
    {
        PL_PLAYLIST,    //MusicPlayer2播放列表
        PL_M3U,         //m3u播放列表
        PL_M3U8         //m3u8播放列表
    };

    CPlaylistFile();
    ~CPlaylistFile();
    void LoadFromFile(const wstring& file_path);
    void SaveToFile(const wstring& file_path, Type type = PL_PLAYLIST) const;
    const vector<SongInfo>& GetPlaylist() const;
    int AddSongsToPlaylist(const vector<SongInfo>& songs, bool insert_begin = false);
    // 使用移动语义覆盖参数song_list，调用后此CPlaylistFile对象不再可用
    void MoveToSongList(vector<SongInfo>& song_list);
    bool IsSongInPlaylist(const SongInfo& song);
    int GetSongIndexInPlaylist(const SongInfo& song);
    void RemoveSong(const SongInfo& song);

    static bool IsPlaylistFile(const wstring& file_path);
    static bool IsPlaylistExt(wstring ext);

    static void SavePlaylistToFile(const vector<SongInfo>& song_list, const wstring& file_path, Type type = PL_PLAYLIST);

public:
    const static vector<wstring> m_surpported_playlist;     //支持的播放列表文件的扩展名列表

protected:
    //解析MusicPlayer2播放列表
    void ParsePlaylistFile(const std::wstring& file_contents);
    //解析m3u/m3u8格式播放列表
    void ParseM3uFile(const std::wstring& file_contents);
    //解析Windows Media Player播放列表
    void ParseWplFile(const std::string& file_contents);
    //解析千千静听播放列表
    void ParseTtplFile(const std::string& file_contents);

private:
    vector<SongInfo> m_playlist;
    wstring m_path;
};

