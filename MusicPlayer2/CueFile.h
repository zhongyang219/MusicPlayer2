#pragma once
#include <map>
#include "Time.h"
#include "SongInfo.h"

class CCueFile
{
public:
    CCueFile(const std::wstring& file_path, Time length);
    ~CCueFile();
    const std::map<int, SongInfo>& GetAnalysisResult() const;

private:

private:
    std::wstring m_file_path;
    std::wstring m_audio_file_path;
    std::map<int, SongInfo> m_result;
};

