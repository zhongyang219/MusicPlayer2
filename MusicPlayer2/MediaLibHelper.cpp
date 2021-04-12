#include "stdafx.h"
#include "MediaLibHelper.h"
#include "MusicPlayer2.h"
#include <set>
#include "SongDataManager.h"

CMediaClassifier::CMediaClassifier(ClassificationType type, bool hide_only_one_classification)
    : m_type(type), m_hide_only_one_classification(hide_only_one_classification)
{
}


CMediaClassifier::~CMediaClassifier()
{
}

const CMediaClassifier::MediaList& CMediaClassifier::GetMeidaList() const
{
    return m_media_list;
}

CMediaClassifier::MediaList& CMediaClassifier::GetMeidaList()
{
    return m_media_list;
}

void CMediaClassifier::ClassifyMedia()
{
    m_media_list.clear();
    for (const auto& song_info : CSongDataManager::GetInstance().GetSongData())
    {
        if (song_info.first.empty())
            continue;

        std::vector<std::wstring> item_names;
        switch (m_type)
        {
        case CMediaClassifier::CT_ARTIST:
            song_info.second.GetArtistList(item_names);      //有的歌曲可能有多个艺术家，将解析到的艺术家保存到vector里
            for (auto& item_name : item_names)
                CCommon::StringNormalize(item_name);
            if (item_names.empty() || (item_names.size() == 1 && item_names[0] == CCommon::LoadText(IDS_DEFAULT_ARTIST).GetString()))
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
        case CMediaClassifier::CT_GENRE:
        {
            wstring str_genre = song_info.second.genre;
            if (str_genre == CCommon::LoadText(IDS_DEFAULT_GENRE).GetString())
                str_genre.clear();
            item_names.push_back(str_genre);
        }
        break;
        case CMediaClassifier::CT_YEAR:
        {
            wstring str_year = song_info.second.get_year();
            if (str_year == CCommon::LoadText(IDS_DEFAULT_YEAR).GetString())
                str_year.clear();
            if (str_year.size() > 4)
                str_year.resize(4);
            item_names.push_back(str_year);
        }
        break;
        case CMediaClassifier::CT_TYPE:
        {
            wstring str_type = CFilePathHelper(song_info.first).GetFileExtension();
            item_names.push_back(str_type);
        }
        break;
        case CMediaClassifier::CT_BITRATE:
        {
            wstring str_type;
            if (song_info.second.bitrate == 0)
                str_type = L"-";
            else if (song_info.second.bitrate < 32)
                str_type = L"<32";
            else if (song_info.second.bitrate < 64)
                str_type = L"32+";
            else if (song_info.second.bitrate < 128)
                str_type = L"64+";
            else if (song_info.second.bitrate < 192)
                str_type = L"128+";
            else if (song_info.second.bitrate < 300)
                str_type = L"192+";
            else if (song_info.second.bitrate < 512)
                str_type = L"300+";
            else if (song_info.second.bitrate < 1024)
                str_type = L"512+";
            else
                str_type = L"1024+";
            item_names.push_back(str_type);
        }
        break;
        case CT_RATING:
        {
            wstring str_type;
            if (song_info.second.rating >= 1 && song_info.second.rating <= 5)
                str_type = std::to_wstring(song_info.second.rating);
            else
                str_type = CCommon::LoadText(IDS_UNRATED).GetString();
            item_names.push_back(str_type);
            break;
        }
        default:
            break;
        }

        for (const auto& item_name : item_names)
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

    std::vector<SongInfo> other_list;

    //查找只有一个项目的分类，将其归到“其他”类里
    if (m_hide_only_one_classification && (m_type == CT_ARTIST || m_type == CT_ALBUM || m_type == CT_GENRE))
    {
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
        else if (m_type == CT_ALBUM)
            std::sort(other_list.begin(), other_list.end(), SongInfo::ByAlbum);
        else if (m_type == CT_GENRE)
            std::sort(other_list.begin(), other_list.end(), [](const SongInfo& a, const SongInfo& b) {return a.genre < b.genre; });
        //else if (m_type == CT_YEAR)
        //    std::sort(other_list.begin(), other_list.end(), [](const SongInfo& a, const SongInfo& b) {return a.year < b.year; });
        if (!other_list.empty())
            m_media_list[STR_OTHER_CLASSIFY_TYPE] = other_list;
    }

    //将年份不是4位数字的归到“其他”类里
    if (m_type == CT_YEAR)
    {
        for (auto& iter = m_media_list.begin(); iter != m_media_list.end();)
        {
            if (!iter->first.empty() && !IsStringYear(iter->first))     //如果年份不是4位数字，则添加到其他列表里
            {
                other_list.insert(other_list.end(), iter->second.begin(), iter->second.end());
                iter = m_media_list.erase(iter);
            }
            else
            {
                ++iter;
            }
        }
        if (!other_list.empty())
            m_media_list[STR_OTHER_CLASSIFY_TYPE] = other_list;
    }

    for (auto& item : m_media_list)
    {
        if (item.first != STR_OTHER_CLASSIFY_TYPE)
        {
            if (m_type == CT_ALBUM)    //“唱片集”类别中，默认按音轨序号排序
                std::sort(item.second.begin(), item.second.end(), [](const SongInfo& a, const SongInfo& b) {return a.track < b.track; });
            else    //其他类别默认按文件路径排序
                std::sort(item.second.begin(), item.second.end(), [](const SongInfo& a, const SongInfo& b) {return a.file_path < b.file_path; });
        }
    }
}

bool CMediaClassifier::IsStringYear(std::wstring str)
{
    if (str.size() < 4)
        return false;
    str.resize(4);

    for (size_t i = 0; i < 4; i++)
    {
        if (str[i] < L'0' || str[i] > L'9')
            return false;
    }

    return str > L"1000" && str < L"3000";
}

void CMediaClassifier::ClearResult()
{
    m_media_list.clear();
}

void CMediaClassifier::RemoveFiles(std::vector<SongInfo> songs)
{
    auto isRemoved = [&](const SongInfo& song)
    {
        for (const auto& item : songs)
        {
            if (item.IsSameSong(song))
                return true;
        }
        return false;
    };
    for (auto& item : m_media_list)
    {
        auto iter_removed = std::remove_if(item.second.begin(), item.second.end(), isRemoved);
        item.second.erase(iter_removed, item.second.end());
    }
}

void CMediaClassifier::SetHideOnlyOneClassification(bool hide_only_one_classification)
{
    m_hide_only_one_classification = hide_only_one_classification;
}
