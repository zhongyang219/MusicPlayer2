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

    SongInfo first_track{ m_result.front() };
    //写入流派
    if (!first_track.genre.empty())
        file_stream << "REM GENRE " << CCommon::UnicodeToStr(first_track.genre, CodeType::UTF8_NO_BOM) << "\r\n";
    //写入年份
    if (!first_track.IsYearEmpty())
        file_stream << "REM DATE " << CCommon::UnicodeToStr(first_track.get_year(), CodeType::UTF8_NO_BOM) << "\r\n";
    //写入注释
    if (!first_track.comment.empty())
        file_stream << "REM COMMENT \"" << CCommon::UnicodeToStr(first_track.comment, CodeType::UTF8_NO_BOM) << "\"\r\n";
    //写入唱片集标题
    file_stream << "TITLE \"" << CCommon::UnicodeToStr(first_track.album, CodeType::UTF8_NO_BOM) << "\"\r\n";

    //写入其他属性
    for (const auto& property_item : m_cue_property_map)
    {
        if (property_item.first != L"REM GENRE" && property_item.first != L"REM DATE" && property_item.first != L"REM COMMENT" && property_item.first != L"TITLE")
            file_stream << CCommon::UnicodeToStr(property_item.first, CodeType::UTF8_NO_BOM) << " " << CCommon::UnicodeToStr(property_item.second, CodeType::UTF8_NO_BOM) << "\r\n";
    }
    //写入文件名
    auto getCueAudioFileType = [](const std::wstring& file_path) -> std::string
    {
        std::wstring file_ext = CFilePathHelper(file_path).GetFileExtension();
        if (file_ext == L"mp3")
            return "MP3";
        else if (file_ext == L"aif" || file_ext == L"aiff")
            return "AIFF";
        else
            return "WAVE";
    };
    std::string file_type = getCueAudioFileType(first_track.file_path);
    file_stream << "FILE \"" << CCommon::UnicodeToStr(first_track.GetFileName(), CodeType::UTF8_NO_BOM) << "\" " << file_type << "\r\n";

    //写入每个音轨
    size_t index = 0;
    for (const auto& song : m_result)
    {
        //有多个音频文件时写入新的FILE标签
        if (song.file_path != first_track.file_path)
        {
            first_track = song;
            std::string file_type = getCueAudioFileType(first_track.file_path);
            file_stream << "FILE \"" << CCommon::UnicodeToStr(first_track.GetFileName(), CodeType::UTF8_NO_BOM) << "\" " << file_type << "\r\n";
        }

        //音轨信息
        file_stream << "  TRACK ";
        if (song.track < 10)
            file_stream << "0";
        file_stream << song.track << " AUDIO\r\n";
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
        //写入曲目标题
        if (!song.title.empty())
            file_stream << "    TITLE \"" << CCommon::UnicodeToStr(song.title, CodeType::UTF8_NO_BOM) << "\"\r\n";
        //写入艺术家
        if (!song.artist.empty())
            file_stream << "    PERFORMER \"" << CCommon::UnicodeToStr(song.artist, CodeType::UTF8_NO_BOM) << "\"\r\n";
        //写入其他属性
        auto& track_property_map = m_track_property_maps[song.file_path][song.track];
        for (const auto& property_item : track_property_map)
        {
            if (property_item.first != L"TRACK" && property_item.first != L"PERFORMER" && property_item.first != L"TITLE")
                file_stream << "    " << CCommon::UnicodeToStr(property_item.first, CodeType::UTF8_NO_BOM) << " " << CCommon::UnicodeToStr(property_item.second, CodeType::UTF8_NO_BOM) << "\r\n";
        }

        index++;
    }

    file_stream.close();
    return true;
}

SongInfo& CCueFile::GetTrackInfo(const std::wstring& audio_path, int track)
{
    static SongInfo empty_song_info;
    if (m_track_property_maps.size() == 1)
    {
        for (auto& song : m_result)
            if (song.track == track)
                return song;
    }
    else
    {
        for (auto& song : m_result)
            if (song.track == track && song.file_path == audio_path)
                return song;
    }
    return empty_song_info;
}

const std::map<std::wstring, std::wstring>& CCueFile::GetCuePropertyMap() const
{
    return m_cue_property_map;
}

const std::map<std::wstring, std::wstring>& CCueFile::GetTrackPropertyMap(const std::wstring& audio_path, int track)
{
    if (m_track_property_maps.size() == 1)
        return m_track_property_maps.begin()->second[track];
    else
        return m_track_property_maps[audio_path][track];
}

void CCueFile::DoAnalysis()
{
    CFilePathHelper cue_file_path{ m_file_path };

    size_t index_file{};
    index_file = m_file_content_wcs.find(L"FILE ");
    std::wstring cue_head_contents{ m_file_content_wcs.substr(0, index_file) };     //cue的头部

    SongInfo song_info_common{};
    // 获取一次标签作为默认值，可以取得FILE之前的标签
    song_info_common.album = GetCommand(cue_head_contents, L"TITLE");
    song_info_common.genre = GetCommand(cue_head_contents, L"REM GENRE");
    song_info_common.SetYear(GetCommand(cue_head_contents, L"REM DATE").c_str());
    song_info_common.comment = GetCommand(cue_head_contents, L"REM COMMENT");
    song_info_common.album_artist = GetCommand(cue_head_contents, L"PERFORMER ");
    song_info_common.artist = song_info_common.album_artist;
    song_info_common.disc_num = static_cast<BYTE>(_wtoi(GetCommand(cue_head_contents, L"REM DISCNUMBER").c_str()));
    song_info_common.total_discs = static_cast<BYTE>(_wtoi(GetCommand(cue_head_contents, L"REM TOTALDISCS").c_str()));
    song_info_common.is_cue = true;

    //查找所有属性
    FindAllProperty(cue_head_contents, m_cue_property_map);

    CCommon::StringNormalize(song_info_common.album);
    CCommon::StringNormalize(song_info_common.genre);
    CCommon::StringNormalize(song_info_common.comment);

    size_t index_track{};
    size_t index_title{};
    size_t index_artist{};
    while (true)
    {
        index_track = index_file;   // 恢复内层break时index_track的值，使其正常查找第一个TRACK
        // 获取此FILE标签对应path
        song_info_common.file_path = cue_file_path.GetDir() + GetCommand(m_file_content_wcs, L"FILE ", index_file);
        size_t next_file_index = m_file_content_wcs.find(L"FILE ", index_file + 6);
        while (true)
        {
            SongInfo song_info{ song_info_common };
            song_info.cue_file_path = m_file_path;
            // 查找曲目序号
            index_track = m_file_content_wcs.find(L"TRACK ", index_track + 6);
            // 限制TRACK在此FILE范围
            if (index_track >= next_file_index)
                break;
            wstring track_str = m_file_content_wcs.substr(index_track + 6, 3);
            song_info.track = _wtoi(track_str.c_str());

            auto& track_property_map = m_track_property_maps[song_info.file_path][song_info.track];
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
            }

            // 查找曲目艺术家
            index_artist = m_file_content_wcs.find(L"PERFORMER ", index_track + 6);
            if (index_artist < next_track_index)
            {
                index2 = m_file_content_wcs.find(L'\"', index_artist);
                index3 = m_file_content_wcs.find(L'\"', index2 + 1);
                song_info.artist = m_file_content_wcs.substr(index2 + 1, index3 - index2 - 1);
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
                //if(!m_result.back().end_pos.isZero())
                //    m_result.back().lengh = Time(m_result.back().end_pos - m_result.back().start_pos);
            }

            CCommon::StringNormalize(song_info.title);
            CCommon::StringNormalize(song_info.artist);

            m_result.push_back(song_info);

            //查找当前音轨的所有标签
            int index_next_track = m_file_content_wcs.find(L"TRACK ", index_track + 6);
            FindAllProperty(m_file_content_wcs.substr(index_track, index_next_track - index_track), track_property_map);
        }
        // 如果没有下一个FILE标签则退出
        if (next_file_index == wstring::npos)
            break;
        else
            index_file = next_file_index;
    }

    //设置曲目总数
    int total_tracks = m_result.size();
    for (auto& song_info : m_result)
    {
        song_info.total_tracks = total_tracks;
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

wstring CCueFile::GetCommand(const wstring& str_contents, const wstring& str, size_t pos)
{
    if (pos == wstring::npos)
        return wstring();

    wstring command;
    size_t index1 = str_contents.find(str, pos);
    if (index1 == wstring::npos)
        return wstring();
    size_t index2 = str_contents.find(L'\"', index1 + str.size());
    size_t index3 = str_contents.find(L'\"', index2 + 1);
    size_t index_rtn = str_contents.find(L'\n', index1);
    if (index2 < index_rtn)     //当前行找到了引号，则获取引号之间的字符串
    {
        command = str_contents.substr(index2 + 1, index3 - index2 - 1);
    }
    else        //当前行没有找到引号
    {
        index2 = str_contents.find(L' ', index1 + str.size());
        index3 = index_rtn;
        size_t count = index3 - index2 - 1;
        if (count > 0)
            command = str_contents.substr(index2 + 1, count);
    }

    return command;
}

void CCueFile::FindAllProperty(const wstring& str_contents, std::map<std::wstring, std::wstring>& property_map)
{
    std::vector<std::wstring> contents_list;
    CCommon::StringSplitWithMulitChars(str_contents, L"\r\n", contents_list);
    for (const auto& str : contents_list)
    {
        size_t index_quote = str.find(L'\"');           //查找引号
        size_t index_space1 = str.find(L' ');           //查找第1个空格
        if (index_space1 == std::wstring::npos || index_space1 > index_quote)   //空格必须在引号前
            break;
        size_t index_space2 = str.find(L' ', index_space1 + 1); //查找第2个空格
        if (index_space2 > index_quote)
            index_space2 = std::wstring::npos;          //第2个空格在引号后面，则认为无效
        size_t index{ index_space2 == std::wstring::npos ? index_space1 : index_space2 };
        std::wstring key = str.substr(0, index);
        std::wstring value = str.substr(index + 1);
        CCommon::StringNormalize(key);
        CCommon::StringNormalize(value);
        if (CCommon::StringLeftMatch(key, L"FILE") || CCommon::StringLeftMatch(key, L"TRACK") || CCommon::StringLeftMatch(key, L"INDEX"))
            continue;
        if (!key.empty())
        {
            property_map[key] = value;
        }
    }
}
