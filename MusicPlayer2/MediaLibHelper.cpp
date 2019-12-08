#include "stdafx.h"
#include "MediaLibHelper.h"
#include "MusicPlayer2.h"
#include <set>

CMediaClassifier::CMediaClassifier(ClassificationType type, bool hide_only_one_classification)
    : m_type(type), m_hide_only_one_classification(hide_only_one_classification)
{
}


CMediaClassifier::~CMediaClassifier()
{
}

const std::map<std::wstring, std::vector<SongInfo>>& CMediaClassifier::GetMeidaList() const
{
    return m_media_list;
}

void CMediaClassifier::ClassifyMedia()
{
    m_media_list.clear();
    for (const auto& song_info : theApp.m_song_data)
    {
        if(song_info.first.empty())
            continue;

        std::vector<std::wstring> item_names;
        switch (m_type)
        {
        case CMediaClassifier::CT_ARTIST:
            CCommon::StringSplitWithMulitChars(song_info.second.artist, L"/;&", item_names, true);      //有的歌曲可能有多个艺术家，将解析到的艺术家保存到vector里
            for (auto& item_name : item_names)
                CCommon::StringNormalize(item_name);
            if (item_names.empty() || (item_names.size()==1 && item_names[0] == CCommon::LoadText(IDS_DEFAULT_ARTIST).GetString()))
            {
                item_names.clear();
                item_names.push_back(std::wstring());
            }
            break;
        case CMediaClassifier::CT_ALBUM:
        {
            wstring str_album = song_info.second.album;
            if (str_album == CCommon::LoadText(IDS_DEFAULT_ALBUM).GetString())
                str_album.clear();
            item_names.push_back(str_album);
        }
            break;
        default:
            break;
        }

        for(const auto& item_name : item_names)
        {
            auto iter = m_media_list.find(item_name);
            if (iter != m_media_list.end())
            {
                iter->second.push_back(song_info.second);
                iter->second.back().file_path = song_info.first;
            }
            else
            {
                m_media_list[item_name].push_back(song_info.second);
                m_media_list[item_name].back().file_path = song_info.first;
            }
        }

    }

    //查找只有一个项目的分类，将其归到“其他”类里
    if (m_hide_only_one_classification)
    {
        std::vector<SongInfo> other_list;
        for (auto& iter = m_media_list.begin(); iter != m_media_list.end();)
        {
            if (iter->second.size() == 1)
            {
                //确保其他类列表里的项目不会重复
                if (!CCommon::IsItemInVector(other_list, [&](const SongInfo& item) {
                    return item.file_path == iter->second[0].file_path;
                }))
                {
                    other_list.push_back(iter->second[0]);
                }
                iter = m_media_list.erase(iter);
            }
            else
            {
                ++iter;
            }
        }
        if (m_type == CT_ARTIST)
            std::sort(other_list.begin(), other_list.end(), SongInfo::ByArtist);
        else if(m_type == CT_ALBUM)
            std::sort(other_list.begin(), other_list.end(), SongInfo::ByAlbum);
        m_media_list[STR_OTHER_CLASSIFY_TYPE] = other_list;
    }
}
