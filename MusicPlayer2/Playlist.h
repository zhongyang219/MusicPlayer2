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
    void FromSongList(const vector<SongInfo>& song_list);
    void ToSongList(vector<SongInfo>& song_list);
    bool IsSongInPlaylist(const SongInfo& song);
    int GetSongIndexInPlaylist(const SongInfo& song);
    void RemoveSong(const SongInfo& song);

    static bool IsPlaylistFile(const wstring& file_path);
    static bool IsPlaylistExt(wstring ext);

public:
    const static vector<wstring> m_surpported_playlist;     //支持的播放列表文件的扩展名列表

private:
    void DisposePlaylistFileLine(const string& str_current_line, bool utf8);

private:
    vector<SongInfo> m_playlist;
    wstring m_path;
};

