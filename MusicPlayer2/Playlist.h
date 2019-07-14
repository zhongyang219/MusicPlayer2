#pragma once
#include "SongInfo.h"
class CPlaylist
{
public:
    CPlaylist();
    ~CPlaylist();
    void LoadFromFile(const wstring& file_path);
    void SaveToFile(const wstring& file_path) const;
    const vector<wstring>& GetPlaylist() const;
    void AddFiles(const vector<wstring>& files);
    void FromSongList(const vector<SongInfo>& song_list);
    void ToSongList(vector<SongInfo>& song_list);
    bool IsFileInPlaylist(const wstring& file);
    void RemoveFile(const wstring& file);

private:
    vector<wstring> m_playlist;
};

