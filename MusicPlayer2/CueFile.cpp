#include "stdafx.h"
#include "CueFile.h"


CCueFile::CCueFile(const std::wstring& file_path, Time length)
{
}


CCueFile::~CCueFile()
{
}

const std::map<int, SongInfo>& CCueFile::GetAnalysisResult() const
{
    return m_result;
}
