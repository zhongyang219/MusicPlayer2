#include "stdafx.h"
#include "Playlist.h"
#include "Common.h"
#include "FilePathHelper.h"


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
            m_playlist.push_back(CCommon::StrToUnicode(current_line, CodeType::UTF8_NO_BOM));
    }

}

void CPlaylist::SaveToFile(const wstring & file_path) const
{
    ofstream stream{ file_path };
    for (const auto& item : m_playlist)
    {
        stream << CCommon::UnicodeToStr(item, CodeType::UTF8_NO_BOM) << std::endl;
    }
}

const vector<wstring>& CPlaylist::GetPlaylist() const
{
    return m_playlist;
}

void CPlaylist::AddFiles(const vector<wstring>& files)
{
    for (const auto& file : files)
        m_playlist.push_back(file);
}

void CPlaylist::FromSongList(const vector<SongInfo>& song_list)
{
    m_playlist.clear();
    for (auto song : song_list)
    {
        if(!song.file_path.empty())
            m_playlist.push_back(song.file_path);
    }
}

void CPlaylist::ToSongList(vector<SongInfo>& song_list)
{
    for (const auto& file : m_playlist)
    {
        SongInfo song_info;
        CFilePathHelper file_path{ file };
        song_info.file_name = file_path.GetFileName();
        song_info.file_path = file;
        song_list.push_back(song_info);
    }
}

bool CPlaylist::IsFileInPlaylist(const wstring& file)
{
    auto iter = std::find(m_playlist.begin(), m_playlist.end(), file);
    return iter != m_playlist.end();
}

void CPlaylist::RemoveFile(const wstring& file)
{
    auto iter = std::find(m_playlist.begin(), m_playlist.end(), file);
    if (iter != m_playlist.end())
    {
        m_playlist.erase(iter);
    }
}
