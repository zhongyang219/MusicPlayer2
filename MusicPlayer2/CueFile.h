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
    void SetTotalLength(Time length);       //设置cue对应音频文件的总长度（需要在解析完成后调用GetAudioFileName获取解析到的音频文件路径，再获取该音频文件的长度）
    const std::vector<SongInfo>& GetAnalysisResult() const;
    // std::wstring GetAudioFileName() const;

private:
    void DoAnalysis();
    Time PhaseIndex(size_t pos);
    wstring GetCommand(const wstring& str, size_t pos = 0);

private:
    std::wstring m_file_path;
    //std::string m_file_content;
    std::wstring m_file_content_wcs;
    CodeType m_code_type{ CodeType::AUTO };
    std::vector<SongInfo> m_result;
};

