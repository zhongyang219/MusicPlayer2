#include "stdafx.h"
#include "CueFile.h"
#include "FilePathHelper.h"


CCueFile::CCueFile(const std::wstring& file_path)
    : m_file_path(file_path)
{
    ifstream OpenFile{ m_file_path };
    if (OpenFile.fail())
        return;
    string current_line;
    char ch;
    while (!OpenFile.eof())
    {
        OpenFile.get(ch);
        m_file_content.push_back(ch);
        if (m_file_content.size() > 102400) break;	//限制cue文件最大为100KB
    }
    CodeType m_code_type{ CodeType::AUTO };		//cue文件的编码类型
    if (m_file_content.size() >= 3 && m_file_content[0] == -17 && m_file_content[1] == -69 && m_file_content[2] == -65)
        m_code_type = CodeType::UTF8;

    DoAnalysis();
}


CCueFile::~CCueFile()
{
}

void CCueFile::SetTotalLength(Time length)
{
    if (!m_result.empty())
    {
        m_result.back().end_pos = length;
        m_result.back().lengh = Time(length - m_result.back().start_pos);
    }
}

const std::vector<SongInfo>& CCueFile::GetAnalysisResult() const
{
    return m_result;
}

std::wstring CCueFile::GetAudioFileName() const
{
    return m_audio_file_name;
}

void CCueFile::DoAnalysis()
{
    CFilePathHelper cue_file_path{ m_file_path };

    //获取关联音频文件的文件名
    m_audio_file_name = CCommon::StrToUnicode(GetCommand("FILE"), m_code_type);

    SongInfo song_info{};
    song_info.album = CCommon::StrToUnicode(GetCommand("TITLE"), m_code_type);
    song_info.genre = CCommon::StrToUnicode(GetCommand("REM GENRE"), m_code_type);
    song_info.year = CCommon::StrToUnicode(GetCommand("REM DATE"), m_code_type);
    song_info.comment = CCommon::StrToUnicode(GetCommand("REM COMMENT"), m_code_type);
    //song_info.file_name = m_audio_file_name;
    song_info.file_path = cue_file_path.GetDir() + m_audio_file_name;
    song_info.is_cue = true;
    song_info.info_acquired = true;

    size_t index_track{};
    size_t index_title{};
    size_t index_artist{};
    while (true)
    {
        //查找曲目序号
        index_track = m_file_content.find("TRACK ", index_track + 6);
        if (index_track == string::npos)
            break;
        string track_str = m_file_content.substr(index_track + 6, 3);
        song_info.track = atoi(track_str.c_str());
        size_t next_track_index = m_file_content.find("TRACK ", index_track + 6);
        //查找曲目标题
        size_t index2, index3;
        index_title = m_file_content.find("TITLE ", index_track + 6);
        if (index_title < next_track_index)
        {
            index2 = m_file_content.find('\"', index_title);
            index3 = m_file_content.find('\"', index2 + 1);
            song_info.title = CCommon::StrToUnicode(m_file_content.substr(index2 + 1, index3 - index2 - 1), m_code_type);
        }

        //查找曲目艺术家
        index_artist = m_file_content.find("PERFORMER ", index_track + 6);
        if (index_artist < next_track_index)
        {
            index2 = m_file_content.find('\"', index_artist);
            index3 = m_file_content.find('\"', index2 + 1);
            song_info.artist = CCommon::StrToUnicode(m_file_content.substr(index2 + 1, index3 - index2 - 1), m_code_type);
        }

        //查找曲目位置
        Time time_index00, time_index01;
        size_t index00_pos{}, index01_pos{};
        index00_pos = m_file_content.find("INDEX 00", index_track + 6);
        index01_pos = m_file_content.find("INDEX 01", index_track + 6);
        time_index00 = PhaseIndex(index00_pos);
        time_index01 = PhaseIndex(index01_pos);

        song_info.start_pos = time_index01;
        if (!m_result.empty())
        {
			if(!time_index00.isZero())
				m_result.back().end_pos = time_index00;
			else
				m_result.back().end_pos = time_index01;
			if(!m_result.back().end_pos.isZero())
				m_result.back().lengh = Time(m_result.back().end_pos - m_result.back().start_pos);
        }

        m_result.push_back(song_info);
    }

}

Time CCueFile::PhaseIndex(size_t pos)
{
    if (pos == wstring::npos)
        return Time();

    size_t index1 = m_file_content.find(":", pos);
    size_t index2 = m_file_content.rfind(" ", index1);
    string tmp;
    Time time;
    //获取分钟
    tmp = m_file_content.substr(index2 + 1, index1 - index2 - 1);
    time.min = atoi(tmp.c_str());
    //获取秒钟
    tmp = m_file_content.substr(index1 + 1, 2);
    time.sec = atoi(tmp.c_str());
    //获取毫秒
    tmp = m_file_content.substr(index1 + 4, 2);
    time.msec = atoi(tmp.c_str()) * 10;

    return time;
}

string CCueFile::GetCommand(const string& str, size_t pos)
{
    if (pos == string::npos)
        return string();

    string command;
    size_t index1 = m_file_content.find(str);
    if (index1 == string::npos)
        return string();
    size_t index2 = m_file_content.find('\"', index1 + str.size());
    size_t index3 = m_file_content.find('\"', index2 + 1);
    size_t index_rtn = m_file_content.find('\n', index1);
    if (index2 < index_rtn)     //当前行找到了引号，则获取引号之间的字符串
    {
        command = m_file_content.substr(index2 + 1, index3 - index2 - 1);
    }
    else        //当前行没有找到引号
    {
        index2 = m_file_content.find(' ', index1 + str.size());
        index3 = index_rtn;
        size_t count = index3 - index2 - 1;
        if (count > 0)
            command = m_file_content.substr(index2 + 1, count);
    }

    return command;
}
