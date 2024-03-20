#pragma once
#include "SongInfo.h"
#include "Common.h"

class CCueFile
{
public:
    CCueFile(const std::wstring& file_path);
    CCueFile();
    ~CCueFile();

    std::vector<SongInfo>& GetAnalysisResult();

    //将所有cue音轨保存到cue文件
    //如果file_path为空，则保存到m_file_path
    bool Save(std::wstring file_path = std::wstring());

    //从解析结果中获取一个音轨信息
    SongInfo& GetTrackInfo(const std::wstring& audio_path, int track);

    const std::map<std::wstring, std::wstring>& GetCuePropertyMap() const;
    const std::map<std::wstring, std::wstring>& GetTrackPropertyMap(const std::wstring& audio_path, int track);

private:
    void DoAnalysis();
    Time PhaseIndex(size_t pos);
    std::string TimeToString(const Time& pos);
    static wstring GetCommand(const wstring& str_contents, const wstring& str, size_t pos = 0);

    //查找str_contents中的所有属性，并添加到property_map中
    static void FindAllProperty(const wstring& str_contents, std::map<std::wstring, std::wstring>& property_map);

private:
    std::wstring m_file_path;
    std::wstring m_file_content_wcs;
    CodeType m_code_type{ CodeType::AUTO };
    std::vector<SongInfo> m_result;
    std::map<std::wstring, std::wstring> m_cue_property_map;        //保存整个cue共享的属性
    std::map<std::wstring, std::map<int, std::map<std::wstring, std::wstring>>> m_track_property_maps;  //保存cue中每个音频文件每个音轨的属性
};

