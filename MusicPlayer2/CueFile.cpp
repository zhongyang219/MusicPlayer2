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
    //获取cue文件的专辑标题
    CFilePathHelper cue_file_path{ m_file_path };
    string album_name;
    size_t index1 = m_file_content.find("TITLE");
    size_t index2 = m_file_content.find('\"', index1);
    size_t index3 = m_file_content.find('\"', index2 + 1);
    album_name = m_file_content.substr(index2 + 1, index3 - index2 - 1);

    index1 = m_file_content.find("FILE");
    index2 = m_file_content.find('\"', index1);
    index3 = m_file_content.find('\"', index2 + 1);
    m_audio_file_name = CCommon::StrToUnicode(m_file_content.substr(index2 + 1, index3 - index2 - 1), m_code_type);

    SongInfo song_info{};
    song_info.album = CCommon::StrToUnicode(album_name, m_code_type);
    song_info.file_name = m_audio_file_name;
    song_info.file_path = cue_file_path.GetDir() + song_info.file_name;
    //song_info.bitrate = bitrate;
    song_info.is_cue = true;
    song_info.info_acquired = true;

    size_t index_track{};
    size_t index_title{};
    size_t index_artist{};
    size_t index_pos{};
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
        index_pos = m_file_content.find("INDEX ", index_track + 6);
        index1 = m_file_content.find(":", index_pos + 6);
        index2 = m_file_content.rfind(" ", index1);
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

        song_info.start_pos = time;
        if (!time.isZero() && !m_result.empty())
        {
            m_result.back().end_pos = time;
            m_result.back().lengh = Time(time - m_result.back().start_pos);
        }

        m_result.push_back(song_info);
    }

}
