#pragma once
#include <set>

class CMediaLibHelper
{
public:
    enum ClassificationType
    {
        CT_ARTIST,
        CT_ALBUM,
    };

public:
    CMediaLibHelper(ClassificationType type);
    ~CMediaLibHelper();

    std::map<std::wstring, std::vector<std::wstring>>& GetMeidaList();

private:
    void AnalyseMedia();

private:
    std::map<std::wstring, std::vector<std::wstring>> m_media_list;
    ClassificationType m_type;
};

