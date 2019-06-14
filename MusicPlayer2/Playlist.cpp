#include "stdafx.h"
#include "Playlist.h"
#include "Common.h"


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
        m_playlist.push_back(CCommon::StrToUnicode(current_line, CodeType::UTF8));
    }

}

void CPlaylist::SaveToFile(const wstring & file_path) const
{
    ofstream stream{ file_path };
    for (const auto& item : m_playlist)
    {
        stream << CCommon::UnicodeToStr(item, CodeType::UTF8) << std::endl;
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
