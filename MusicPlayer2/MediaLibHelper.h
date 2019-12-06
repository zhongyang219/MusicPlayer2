#pragma once
#include <set>
#include "SongInfo.h"

#define STR_OTHER_CLASSIFY_TYPE L"eRk0Q6ov"
class CMediaClassifier
{
public:
    enum ClassificationType
    {
        CT_ARTIST,
        CT_ALBUM,
    };

public:
    CMediaClassifier(ClassificationType type, bool hide_only_one_classification = false);
    ~CMediaClassifier();

     const std::map<std::wstring, std::vector<SongInfo>>& GetMeidaList() const;
    void ClassifyMedia();

private:

private:
    std::map<std::wstring, std::vector<SongInfo>> m_media_list;
    ClassificationType m_type;
    bool m_hide_only_one_classification;       //如果为true，则把只有一项的类别归到“其他”里面
};

