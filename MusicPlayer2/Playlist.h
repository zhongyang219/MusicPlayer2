#pragma once
#include "SongInfo.h"
class CPlaylist
{
public:
    struct PlaylistItem
    {
        wstring file_path;
        bool is_cue{ false };		//如果曲目是cue分轨，则为true
        Time start_pos{};
        Time end_pos{};
        wstring title;
        wstring artist;
        wstring album;
        int track{};
        int bit_rate{};
    };

public:
    CPlaylist();
    ~CPlaylist();
    void LoadFromFile(const wstring& file_path);
    void SaveToFile(const wstring& file_path) const;
    vector<SongInfo> GetPlaylist() const;
    void AddFiles(const vector<wstring>& files);
    void AddFiles(const vector<SongInfo>& files);
    void FromSongList(const vector<SongInfo>& song_list);
    void ToSongList(vector<SongInfo>& song_list);
    bool IsFileInPlaylist(const SongInfo& file);
    void RemoveFile(const wstring& file);

private:
    static SongInfo PlaylistItemToSongInfo(const PlaylistItem& item);
    static PlaylistItem SongInfoToPlaylistItem(const SongInfo& song);

private:
    vector<PlaylistItem> m_playlist;
};

