#include "stdafx.h"
#include "CueFile.h"
#include "FilePathHelper.h"


CCueFile::CCueFile(const std::wstring& file_path)
    : m_file_path(file_path)
{
    string file_content;
    if (!CCommon::GetFileContent(m_file_path.c_str(), file_content, 102400))
        return;

    // cue文件较长可以自动检测是否符合UTF8编码
    m_file_content_wcs = CCommon::StrToUnicode(file_content, m_code_type, true);

    DoAnalysis();
}


CCueFile::CCueFile()
{
}

CCueFile::~CCueFile()
{
}

void CCueFile::LoadContentsDirect(const std::wstring& cue_contets)
{
    m_file_content_wcs = cue_contets;
    DoAnalysis();
}

// void CCueFile::SetTotalLength(Time length)
// {
//     if (!m_result.empty())
//     {
//         m_result.back().end_pos = length;
//         m_result.back().lengh = Time(length - m_result.back().start_pos);
//     }
// }

std::vector<SongInfo>& CCueFile::GetAnalysisResult()
{
    return m_result;
}

bool CCueFile::Save(std::wstring file_path)
{
    if (file_path.empty())
        file_path = m_file_path;
    if (m_result.empty())
        return false;
    std::ofstream file_stream(file_path, std::ios::binary | std::ios::out | std::ios::trunc);
    if (file_stream.fail())
        return false;

    const SongInfo& first_track{ m_result.front() };
    //写入流派
    if (!first_track.genre.empty())
        file_stream << "REM GENRE " << CCommon::UnicodeToStr(first_track.genre, CodeType::UTF8_NO_BOM) << "\r\n";
    //写入年份
    if (!first_track.IsYearEmpty())
        file_stream << "REM DATE " << CCommon::UnicodeToStr(first_track.get_year(), CodeType::UTF8_NO_BOM) << "\r\n";
    //写入注释
    if (!first_track.comment.empty())
        file_stream << "REM COMMENT \"" << CCommon::UnicodeToStr(first_track.comment, CodeType::UTF8_NO_BOM) << "\"\r\n";
    //写入唱片集艺术家
    file_stream << "PERFORMER \"" << CCommon::UnicodeToStr(first_track.artist, CodeType::UTF8_NO_BOM) << "\"\r\n";
    //写入唱片集标题
    file_stream << "TITLE \"" << CCommon::UnicodeToStr(first_track.album, CodeType::UTF8_NO_BOM) << "\"\r\n";
    //写入文件名
    std::string file_type;
    std::wstring file_ext = CFilePathHelper(first_track.file_path).GetFileExtension();
    if (file_ext == L"mp3")
        file_type = "MP3";
    else if (file_ext == L"aif" || file_ext == L"aiff")
        file_type = "AIFF";
    else
        file_type = "WAVE";
    file_stream << "FILE \"" << CCommon::UnicodeToStr(m_result.front().GetFileName(), CodeType::UTF8_NO_BOM) << "\" " << file_type << "\r\n";

    //写入每个音轨
    size_t index = 0;
    for (const auto& song : m_result)
    {
        //音轨信息
        file_stream << "  TRACK ";
        if (song.track < 10)
            file_stream << "0";
        file_stream << song.track << " AUDIO\r\n";
        //标题
        file_stream << "    TITLE \"" << CCommon::UnicodeToStr(song.title, CodeType::UTF8_NO_BOM) << "\"\r\n";
        //艺术家
        file_stream << "    PERFORMER \"" << CCommon::UnicodeToStr(song.artist, CodeType::UTF8_NO_BOM) << "\"\r\n";
        //时间
        if (song.track == 1)
        {
            file_stream << "    INDEX 01 00:00:00\r\n";
        }
        else
        {
            if (index > 0)
            {
                Time pre_track_end_pos = m_result[index - 1].end_pos;
                if (pre_track_end_pos != song.start_pos)
                    file_stream << "    INDEX 00 " << TimeToString(pre_track_end_pos) << "\r\n";
                file_stream << "    INDEX 01 " << TimeToString(song.start_pos) << "\r\n";
            }
        }
        index++;
    }

    file_stream.close();
    return true;
}

SongInfo& CCueFile::GetTrackInfo(int track)
{
    static SongInfo empty_song_info;
    for (auto& song : m_result)
        if (song.track == track)
            return song;
    return empty_song_info;
}

const std::map<std::wstring, std::wstring>& CCueFile::GetCuePropertyMap() const
{
    return m_cue_property_map;
}

const std::map<std::wstring, std::wstring>& CCueFile::GetTrackPropertyMap(int track)
{
    return m_track_property_maps[track];
}

void CCueFile::DoAnalysis()
{
    CFilePathHelper cue_file_path{ m_file_path };

    SongInfo song_info{};
    // 获取一次标签作为默认值，可以取得FILE之前的标签
    song_info.album = GetCommand(L"TITLE");
    song_info.genre = GetCommand(L"REM GENRE");
    song_info.SetYear(GetCommand(L"REM DATE").c_str());
    song_info.comment = GetCommand(L"REM COMMENT");
    song_info.artist = GetCommand(L"PERFORMER ");
    song_info.is_cue = true;
    song_info.info_acquired = true;

    //获取cue属性
    m_cue_property_map[L"ALBUM"] = song_info.album;
    m_cue_property_map[L"ALBUMARTIST"] = song_info.artist;
    //查找所有REM
    size_t index_rem{ std::wstring::npos };
    while (true)
    {
        index_rem = m_file_content_wcs.find(L"REM", index_rem + 1);
        if (index_rem == std::wstring::npos)
            break;
        size_t index1{}, index2{};
        index2 = m_file_content_wcs.find_first_of(L"\r\n", index_rem + 4);
        index1 = m_file_content_wcs.find(L' ', index_rem + 4);
        if (index1 != std::wstring::npos && index1 < index2)
        {
            std::wstring key = m_file_content_wcs.substr(index_rem + 4, index1 - index_rem - 4);
            std::wstring value = m_file_content_wcs.substr(index1 + 1, index2 - index1 - 1);
            if (!key.empty())
            {
                if (!value.empty() && value.front() == L'\"')
                    value = value.substr(1);
                if (!value.empty() && value.back() == L'\"')
                    value.pop_back();
                m_cue_property_map[key] = value;
            }
        }
    }

    CCommon::StringNormalize(song_info.album);
    CCommon::StringNormalize(song_info.genre);
    CCommon::StringNormalize(song_info.comment);

    size_t index_file{};
    size_t index_track{};
    size_t index_title{};
    size_t index_artist{};
    index_file = m_file_content_wcs.find(L"FILE ", index_file);
    while (true)
    {
        index_track = index_file;   // 恢复内层break时index_track的值，使其正常查找第一个TRACK
        // 获取此FILE标签对应path
        song_info.file_path = cue_file_path.GetDir() + GetCommand(L"FILE ", index_file);
        size_t next_file_index = m_file_content_wcs.find(L"FILE ", index_file + 6);
        while (true)
        {
            // 查找曲目序号
            index_track = m_file_content_wcs.find(L"TRACK ", index_track + 6);
            // 限制TRACK在此FILE范围
            if (index_track >= next_file_index)
                break;
            wstring track_str = m_file_content_wcs.substr(index_track + 6, 3);
            song_info.track = _wtoi(track_str.c_str());

            auto& track_property_map = m_track_property_maps[song_info.track];
            track_property_map[L"TRACK"] = track_str;

            size_t next_track_index = m_file_content_wcs.find(L"TRACK ", index_track + 6);
            // 查找曲目标题
            size_t index2, index3;
            index_title = m_file_content_wcs.find(L"TITLE ", index_track + 6);
            if (index_title < next_track_index)
            {
                index2 = m_file_content_wcs.find(L'\"', index_title);
                index3 = m_file_content_wcs.find(L'\"', index2 + 1);
                song_info.title = m_file_content_wcs.substr(index2 + 1, index3 - index2 - 1);
                track_property_map[L"TITLE"] = song_info.title;
            }

            // 查找曲目艺术家
            index_artist = m_file_content_wcs.find(L"PERFORMER ", index_track + 6);
            if (index_artist < next_track_index)
            {
                index2 = m_file_content_wcs.find(L'\"', index_artist);
                index3 = m_file_content_wcs.find(L'\"', index2 + 1);
                song_info.artist = m_file_content_wcs.substr(index2 + 1, index3 - index2 - 1);
                track_property_map[L"ARTIST"] = song_info.artist;
            }

            // 查找曲目位置
            Time time_index00{}, time_index01{};
            size_t index00_pos{}, index01_pos{};
            index00_pos = m_file_content_wcs.find(L"INDEX 00", index_track + 6);
            index01_pos = m_file_content_wcs.find(L"INDEX 01", index_track + 6);
            if (index00_pos < next_track_index)
                time_index00 = PhaseIndex(index00_pos);
            if (index01_pos < next_track_index)
                time_index01 = PhaseIndex(index01_pos);

            song_info.start_pos = time_index01;

            // 每个FILE的第一个TRACK不能执行这个来补充上个TRACK的信息，上个TRACK的结束时间应当从文件获取
            if (!m_result.empty() && m_file_content_wcs.find(L"TRACK ", index_file + 6) != index_track)
            {
                if(!time_index00.isZero())
                    m_result.back().end_pos = time_index00;
                else
                    m_result.back().end_pos = time_index01;
                if(!m_result.back().end_pos.isZero())
                    m_result.back().lengh = Time(m_result.back().end_pos - m_result.back().start_pos);
            }

            CCommon::StringNormalize(song_info.title);
            CCommon::StringNormalize(song_info.artist);

            m_result.push_back(song_info);
        }
        // 如果没有下一个FILE标签则退出
        if (next_file_index == wstring::npos)
            break;
        else
            index_file = next_file_index;
    }

}

Time CCueFile::PhaseIndex(size_t pos)
{
    if (pos == wstring::npos)
        return Time();

    size_t index1 = m_file_content_wcs.find(L":", pos);
    size_t index2 = m_file_content_wcs.rfind(L" ", index1);
    wstring tmp;
    Time time;
    //获取分钟
    tmp = m_file_content_wcs.substr(index2 + 1, index1 - index2 - 1);
    time.min = _wtoi(tmp.c_str());
    //获取秒钟
    tmp = m_file_content_wcs.substr(index1 + 1, 2);
    time.sec = _wtoi(tmp.c_str());
    //获取毫秒
    tmp = m_file_content_wcs.substr(index1 + 4, 2);
    time.msec = _wtoi(tmp.c_str()) * 10;

    return time;
}

std::string CCueFile::TimeToString(const Time& pos)
{
    char buff[64];
    sprintf_s(buff, "%.2d:%.2d:%.2d", pos.min, pos.sec, pos.msec / 10);
    return std::string(buff);
}

wstring CCueFile::GetCommand(const wstring& str, size_t pos)
{
    if (pos == wstring::npos)
        return wstring();

    wstring command;
    size_t index1 = m_file_content_wcs.find(str, pos);
    if (index1 == wstring::npos)
        return wstring();
    size_t index2 = m_file_content_wcs.find(L'\"', index1 + str.size());
    size_t index3 = m_file_content_wcs.find(L'\"', index2 + 1);
    size_t index_rtn = m_file_content_wcs.find(L'\n', index1);
    if (index2 < index_rtn)     //当前行找到了引号，则获取引号之间的字符串
    {
        command = m_file_content_wcs.substr(index2 + 1, index3 - index2 - 1);
    }
    else        //当前行没有找到引号
    {
        index2 = m_file_content_wcs.find(L' ', index1 + str.size());
        index3 = index_rtn;
        size_t count = index3 - index2 - 1;
        if (count > 0)
            command = m_file_content_wcs.substr(index2 + 1, count);
    }

    return command;
}
