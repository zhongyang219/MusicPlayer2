#pragma once
#include "SongInfo.h"
class CPlaylist
{
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
    vector<SongInfo> m_playlist;
};

