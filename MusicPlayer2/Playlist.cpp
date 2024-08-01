#include "stdafx.h"
#include "Playlist.h"
#include "Common.h"
#include "FilePathHelper.h"
#include "SongDataManager.h"
#include "PlaylistMgr.h"

const vector<wstring> CPlaylistFile::m_surpported_playlist{ PLAYLIST_EXTENSION_2, L"m3u", L"m3u8" };

/*
播放列表文件格式说明
每行一个曲目，每一行的格式为：
文件路径|是否为cue音轨|cue音轨起始时间|cue音轨结束时间|标题|艺术家|唱片集|曲目序号|比特率|流派|年份|注释|cue文件路径
播放列表至少要保存能够在song_data.dat清空时原样恢复特定歌曲的项目
目前除了cue音轨外，其他曲目只保存文件路径
列表cue条目必要保存的项目有“文件路径”、“音轨号”、“cue文件路径”，但出于向后兼容考虑仍然保留其他项目（实际上不以这些项目为准，外部编辑会被忽略）
*/

CPlaylistFile::CPlaylistFile()
{
}


CPlaylistFile::~CPlaylistFile()
{
}

wstring DeleteInvalidCh(const wstring& str)
{
    wstring result = str;
    CCommon::StringCharacterReplace(result, L'|', L'_');
    return result;
}

void CPlaylistFile::LoadFromFile(const wstring & file_path)
{
    m_path = file_path;
    ifstream stream{ file_path };
    if (stream.fail())
        return;

    //判断文件编码
    bool utf8{};
    wstring file_extension = CFilePathHelper(file_path).GetFileExtension();
    utf8 = (file_extension != L"m3u");

    string current_line;
    while (!stream.eof())
    {
        std::getline(stream, current_line);
        DisposePlaylistFileLine(current_line, utf8);
    }
    stream.close();
}

void CPlaylistFile::SaveToFile(const wstring& file_path, Type type) const
{
    SavePlaylistToFile(m_playlist, file_path, type);
}

void CPlaylistFile::SavePlaylistToFile(const vector<SongInfo>& song_list, const wstring& file_path, Type type)
{
    ofstream stream{ file_path };
    if (!stream.is_open())
        return;
    if (type == PL_PLAYLIST)
    {
        for (const auto& item : song_list)
        {
            if (item.file_path.empty()) continue;   // 不保存没有音频路径的项目
            stream << CCommon::UnicodeToStr(item.file_path, CodeType::UTF8_NO_BOM);
            if (item.is_cue)
            {
                // 出于向后兼容考虑必要这行代码，当song_list来自LoadFromFile加载的不记录cue_file_path的播放列表时item需要从媒体库加载cue_file_path
                SongInfo song = CSongDataManager::GetInstance().GetSongInfo3(item); // 从媒体库载入数据，媒体库不存在的话会原样返回item
                CString buff;
                buff.Format(L"|%d|%d|%d|%s|%s|%s|%d|%d|%s|%s|%s|%s", song.is_cue, song.start_pos.toInt(), song.end_pos.toInt(),
                    DeleteInvalidCh(song.title).c_str(), DeleteInvalidCh(song.artist).c_str(), DeleteInvalidCh(song.album).c_str(),
                    song.track, song.bitrate,
                    DeleteInvalidCh(song.genre).c_str(), DeleteInvalidCh(song.get_year()).c_str(), DeleteInvalidCh(song.comment).c_str(),
                    song.cue_file_path.c_str()
                );
                stream << CCommon::UnicodeToStr(buff.GetString(), CodeType::UTF8_NO_BOM);
            }
            stream << "\n"; // 使用std::endl会触发flush影响效率
        }
    }
    else if (type == PL_M3U || type == PL_M3U8)
    {
        CodeType code_type{ CodeType::ANSI };
        if (type == PL_M3U8)
            code_type = CodeType::UTF8_NO_BOM;

        stream << "#EXTM3U" << '\n';
        std::set<std::wstring> saved_cue_path;      //已经保存过的cue文件的路径
        for (const auto& item : song_list)
        {
            if (item.file_path.empty()) continue;   // 不保存没有音频路径的项目
            // song_list可能来自LoadFromFile含有信息不足，此处先从媒体库载入最新数据，媒体库不存在的话会原样返回item
            SongInfo song = CSongDataManager::GetInstance().GetSongInfo3(item);
            if (song.is_cue)
            {
                //如果播放列表中的项目是cue，且该cue文件没有保存过，则将其保存
                if (!song.cue_file_path.empty() && saved_cue_path.find(song.cue_file_path) == saved_cue_path.end())
                {
                    stream << "#" << '\n';
                    stream << CCommon::UnicodeToStr(song.cue_file_path, code_type) << '\n';
                    saved_cue_path.insert(song.cue_file_path);
                }
            }
            else
            {
                CString buff;
                buff.Format(_T("#EXTINF:%d,%s - %s"), song.length().toInt() / 1000, song.GetArtist().c_str(), song.GetTitle().c_str());
                stream << CCommon::UnicodeToStr(buff.GetString(), code_type) << '\n';
                stream << CCommon::UnicodeToStr(song.file_path, code_type) << '\n';
            }
        }
    }
    stream.close();

    //更新播放列表的曲目数
    CPlaylistMgr::Instance().UpdatePlaylistTrackNum(file_path, song_list.size());
}

const vector<SongInfo>& CPlaylistFile::GetPlaylist() const
{
    return m_playlist;
}

int CPlaylistFile::AddSongsToPlaylist(const vector<SongInfo>& songs, bool insert_begin)
{
    int added{};
    for (const auto& file : songs)
    {
        if (CCommon::IsItemInVector(m_playlist, [&](const SongInfo& song) { return song.IsSameSong(file); }))
            continue;
        m_playlist.push_back(file);
        ++added;
    }
    if (insert_begin)   // 使用循环旋转将新增条目移动到开头而不是直接插入到开头，可对参数songs去重
        std::rotate(m_playlist.rbegin(), m_playlist.rbegin() + added, m_playlist.rend());
    return added;
}

void CPlaylistFile::MoveToSongList(vector<SongInfo>& song_list)
{
    song_list = std::move(m_playlist);
}

bool CPlaylistFile::IsSongInPlaylist(const SongInfo& song)
{
    return GetSongIndexInPlaylist(song) != -1;
}

int CPlaylistFile::GetSongIndexInPlaylist(const SongInfo& song)
{
    auto iter = std::find_if(m_playlist.begin(), m_playlist.end(), [&song](const SongInfo& item)
    {
        return song.IsSameSong(item);
    });
    if (iter != m_playlist.end())
        return iter - m_playlist.begin();
    else
        return -1;
}

void CPlaylistFile::RemoveSong(const SongInfo& song)
{
    while (true)
    {
        auto iter = std::find_if(m_playlist.begin(), m_playlist.end(), [&song](const SongInfo& item)
        {
            return song.IsSameSong(item);
        });
        if (iter != m_playlist.end())
            m_playlist.erase(iter);
        else
            break;
    }
}

bool CPlaylistFile::IsPlaylistFile(const wstring& file_path)
{
    wstring file_extension = CFilePathHelper(file_path).GetFileExtension();
    return CCommon::IsItemInVector(m_surpported_playlist, file_extension);
}

bool CPlaylistFile::IsPlaylistExt(wstring ext)
{
    if (ext.empty())
        return false;
    if (ext.front() == L'.')
        ext =  ext.substr(1);
    return CCommon::IsItemInVector(m_surpported_playlist, ext);
}

void CPlaylistFile::DisposePlaylistFileLine(const string& str_current_line, bool utf8)
{
    if (str_current_line.substr(0, 7) == "#EXTM3U" || str_current_line.substr(0, 7) == "#EXTINF")
        return;

    string current_line = str_current_line;
    CCommon::DeleteStringBom(current_line);
    if (!current_line.empty() && current_line.front() == '\"')
        current_line = current_line.substr(1);
    if (!current_line.empty() && current_line.back() == '\"')
        current_line.pop_back();

    if (current_line.size() > 3)
    {
        SongInfo item;
        wstring current_line_wcs = CCommon::StrToUnicode(current_line, utf8 ? CodeType::UTF8 : CodeType::ANSI);
        size_t index = current_line_wcs.find(L'|');
        item.file_path = current_line_wcs.substr(0, index);

        //如果是URL，则直接添加
        if (CCommon::IsURL(item.file_path))
        {
            m_playlist.push_back(item);
        }
        else
        {
            //如果是相对路径，则转换成绝对路径
            item.file_path = CCommon::RelativePathToAbsolutePath(item.file_path, CFilePathHelper(m_path).GetDir());

            if (index < current_line_wcs.size() - 1)
            {
                vector<wstring> result;
                CCommon::StringSplit(current_line_wcs, L'|', result, false);
                if (result.size() >= 2)
                    item.is_cue = (_wtoi(result[1].c_str()) != 0);
                if (result.size() >= 3)
                    item.start_pos.fromInt(_wtoi(result[2].c_str()));
                if (result.size() >= 4)
                    item.end_pos.fromInt(_wtoi(result[3].c_str()));
                //item.lengh = item.end_pos - item.start_pos;
                if (result.size() >= 5)
                    item.title = result[4];
                if (result.size() >= 6)
                    item.artist = result[5];
                if (result.size() >= 7)
                    item.album = result[6];
                if (result.size() >= 8)
                    item.track = _wtoi(result[7].c_str());
                if (result.size() >= 9)
                    item.bitrate = _wtoi(result[8].c_str());
                if (result.size() >= 10)
                    item.genre = result[9];
                if (result.size() >= 11)
                    item.SetYear(result[10].c_str());
                if (result.size() >= 12)
                    item.comment = result[11];
                if (result.size() >= 13)
                    item.cue_file_path = result[12];
            }
            if (CCommon::IsPath(item.file_path)) // 绝对路径的语法检查
            {
                m_playlist.push_back(item);
            }
        }
    }
}
