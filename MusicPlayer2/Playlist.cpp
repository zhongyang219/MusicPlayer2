#include "stdafx.h"
#include "Playlist.h"
#include "Common.h"
#include "FilePathHelper.h"

/*
播放列表文件格式说明
每行一个曲目，每一行的格式为：
文件路径|是否为cue音轨|cue音轨起始时间|cue音轨结束时间|标题|艺术家|唱片集|曲目序号
目前除了cue音轨外，其他曲目只保存文件路径
*/

CPlaylist::CPlaylist()
{
}


CPlaylist::~CPlaylist()
{
}

void CPlaylist::LoadFromFile(const wstring & file_path)
{
    ifstream stream{ file_path };
    if (stream.fail())
        return;
    string current_line;
    while (!stream.eof())
    {
        std::getline(stream, current_line);
        CCommon::DeleteStringBom(current_line);
        if (!current_line.empty() && current_line.front() == '\"')
            current_line = current_line.substr(1);
        if (!current_line.empty() && current_line.back() == '\"')
            current_line.pop_back();

        if(current_line.size()>3)
        {
            PlaylistItem item;
            wstring current_line_wcs = CCommon::StrToUnicode(current_line, CodeType::UTF8_NO_BOM);
            size_t index = current_line_wcs.find(L'|');
            item.file_path = current_line_wcs.substr(0, index);
            if (index < current_line_wcs.size() - 1)
            {
                vector<wstring> result;
                CCommon::StringSplit(current_line_wcs, L'|', result, false);
                if (result.size() >= 8)
                {
                    item.is_cue = (_wtoi(result[1].c_str()) != 0);
                    item.start_pos.fromInt(_wtoi(result[2].c_str()));
                    item.end_pos.fromInt(_wtoi(result[3].c_str()));
                    item.title = result[4];
                    item.artist = result[5];
                    item.album = result[6];
                    item.track = _wtoi(result[7].c_str());
                }
            }
            m_playlist.push_back(item);
        }
    }

}

void CPlaylist::SaveToFile(const wstring & file_path) const
{
    ofstream stream{ file_path };
    for (const auto& item : m_playlist)
    {
        stream << CCommon::UnicodeToStr(item.file_path, CodeType::UTF8_NO_BOM);
        if (item.is_cue)
        {
            CString buff;
            buff.Format(L"|%d|%d|%d|%s|%s|%s|%d", item.is_cue, item.start_pos.toInt(), item.end_pos.toInt(),
                item.title.c_str(), item.artist.c_str(), item.album.c_str(), item.track); 
            stream << CCommon::UnicodeToStr(buff.GetString(), CodeType::UTF8_NO_BOM);
        }
        stream << std::endl;
    }
}

vector<SongInfo> CPlaylist::GetPlaylist() const
{
    vector<SongInfo> playlist;
    for (const auto& item : m_playlist)
        playlist.push_back(PlaylistItemToSongInfo(item));
    return playlist;
}

void CPlaylist::AddFiles(const vector<wstring>& files)
{
    for (const auto& file : files)
    {
        PlaylistItem item;
        item.file_path = file;
        m_playlist.push_back(item);
    }
}

void CPlaylist::AddFiles(const vector<SongInfo>& files)
{
    for (const auto& file : files)
    {
        m_playlist.push_back(SongInfoToPlaylistItem(file));
    }

}

void CPlaylist::FromSongList(const vector<SongInfo>& song_list)
{
    m_playlist.clear();
    for (auto song : song_list)
    {
        if(!song.file_path.empty())
        {
            m_playlist.push_back(SongInfoToPlaylistItem(song));
        }
    }
}

void CPlaylist::ToSongList(vector<SongInfo>& song_list)
{
    for (const auto& item : m_playlist)
    {
        song_list.push_back(PlaylistItemToSongInfo(item));
    }
}

bool CPlaylist::IsFileInPlaylist(const wstring& file)
{
    auto iter = std::find_if(m_playlist.begin(), m_playlist.end(), [&file](const PlaylistItem& item)
    {
        return file == item.file_path;
    });
    return iter != m_playlist.end();
}

void CPlaylist::RemoveFile(const wstring& file)
{
    auto iter = std::find_if(m_playlist.begin(), m_playlist.end(), [&file](const PlaylistItem& item)
    {
        return file == item.file_path;
    });
    if (iter != m_playlist.end())
    {
        m_playlist.erase(iter);
    }
}

SongInfo CPlaylist::PlaylistItemToSongInfo(const PlaylistItem & item)
{
    SongInfo song_info;
    CFilePathHelper file_path{ item.file_path };
    song_info.file_name = file_path.GetFileName();
    song_info.file_path = item.file_path;
    song_info.is_cue = item.is_cue;
    song_info.start_pos = item.start_pos;
    song_info.end_pos = item.end_pos;
    song_info.lengh = Time(item.end_pos - item.start_pos);
    song_info.title = item.title;
    song_info.artist = item.artist;
    song_info.album = item.album;
    song_info.track = item.track;
    return song_info;
}

CPlaylist::PlaylistItem CPlaylist::SongInfoToPlaylistItem(const SongInfo & song)
{
    PlaylistItem item;
    item.file_path = song.file_path;
    item.is_cue = song.is_cue;
    item.start_pos = song.start_pos;
    item.end_pos = song.end_pos;
    item.title = song.title;
    item.artist = song.artist;
    item.album = song.album;
    item.track = song.track;
    return item;
}
