#include "stdafx.h"
#include "MediaLibHelper.h"
#include "MusicPlayer2.h"
#include <set>

CMediaLibHelper::CMediaLibHelper(ClassificationType type)
    : m_type(type)
{
    AnalyseMedia();
}


CMediaLibHelper::~CMediaLibHelper()
{
}

std::map<std::wstring, std::vector<std::wstring>>& CMediaLibHelper::GetMeidaList()
{
    return m_media_list;
}

void CMediaLibHelper::AnalyseMedia()
{
    for (const auto& song_info : theApp.m_song_data)
    {
        if(song_info.first.empty())
            continue;

        std::wstring item_name;
        switch (m_type)
        {
        case CMediaLibHelper::CT_ARTIST:
            item_name = song_info.second.artist;
            break;
        case CMediaLibHelper::CT_ALBUM:
            item_name = song_info.second.album;
            break;
        default:
            break;
        }

        auto iter = m_media_list.find(item_name);
        if (iter != m_media_list.end())
        {
            iter->second.push_back(song_info.first);
        }
        else
        {
            m_media_list[item_name].push_back(song_info.first);
        }

    }

    int a = 0;
}
