#pragma once
#include <vector>
#include "Time.h"
#include "SongInfo.h"
#include "Common.h"

class CCueFile
{
public:
    CCueFile(const std::wstring& file_path);
    CCueFile();
    ~CCueFile();
    void LoadContentsDirect(const std::wstring& cue_contets);
    // void SetTotalLength(Time length);       //设置cue对应音频文件的总长度（需要在解析完成后调用GetAudioFileName获取解析到的音频文件路径，再获取该音频文件的长度）
    std::vector<SongInfo>& GetAnalysisResult();
    // std::wstring GetAudioFileName() const;

    //将所有cue音轨保存到cue文件
    //如果file_path为空，则保存到m_file_path
    bool Save(std::wstring file_path = std::wstring());

    //从解析结果中获取一个音轨信息
    SongInfo& GetTrackInfo(int track);

    const std::map<std::wstring, std::wstring>& GetCuePropertyMap() const;
    const std::map<std::wstring, std::wstring>& GetTrackPropertyMap(int track);

private:
    void DoAnalysis();
    Time PhaseIndex(size_t pos);
    std::string TimeToString(const Time& pos);
    wstring GetCommand(const wstring& str, size_t pos = 0);
    std::map<std::wstring, std::wstring> m_cue_property_map;
    std::map<int, std::map<std::wstring, std::wstring>> m_track_property_maps;

private:
    std::wstring m_file_path;
    //std::string m_file_content;
    std::wstring m_file_content_wcs;
    CodeType m_code_type{ CodeType::AUTO };
    std::vector<SongInfo> m_result;
};

