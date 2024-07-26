#include "stdafx.h"
#include "UiMediaLibItemMgr.h"
#include "MusicPlayer2.h"
#include "MediaLibPlaylistMgr.h"
#include "Playlist.h"
#include "SongDataManager.h"

CUiMediaLibItemMgr CUiMediaLibItemMgr::m_instance;

CUiMediaLibItemMgr::~CUiMediaLibItemMgr()
{
}

CUiMediaLibItemMgr& CUiMediaLibItemMgr::Instance()
{
    return m_instance;
}

void CUiMediaLibItemMgr::GetClassifiedMeidaLibItemList(CMediaClassifier::ClassificationType type)
{
    auto& item_list{ m_item_map[type] };
    item_list.clear();
    CMediaClassifier classifier(type, theApp.m_media_lib_setting_data.hide_only_one_classification);
    classifier.ClassifyMedia();
    for (const auto& item : classifier.GetMeidaList())
    {
        if (item.first != STR_OTHER_CLASSIFY_TYPE)
        {
            ItemInfo info;
            info.name = item.first;
            info.count = item.second.size();
            item_list.push_back(info);
        }
    }
}

void CUiMediaLibItemMgr::Init()
{
    m_loading = true;

    GetClassifiedMeidaLibItemList(CMediaClassifier::CT_ARTIST);
    GetClassifiedMeidaLibItemList(CMediaClassifier::CT_ALBUM);
    GetClassifiedMeidaLibItemList(CMediaClassifier::CT_GENRE);
    GetClassifiedMeidaLibItemList(CMediaClassifier::CT_YEAR);
    GetClassifiedMeidaLibItemList(CMediaClassifier::CT_TYPE);
    GetClassifiedMeidaLibItemList(CMediaClassifier::CT_BITRATE);
    GetClassifiedMeidaLibItemList(CMediaClassifier::CT_RATING);

    m_loading = false;
}

int CUiMediaLibItemMgr::GetItemCount(CMediaClassifier::ClassificationType type) const
{
    if (!m_loading)
    {
        auto iter = m_item_map.find(type);
        if (iter != m_item_map.end())
            return static_cast<int>(iter->second.size());
    }
    return 0;
}

std::wstring CUiMediaLibItemMgr::GetItemDisplayName(CMediaClassifier::ClassificationType type, int index) const
{
    if (!m_loading)
    {
        const std::wstring& name{ GetItemName(type, index) };
        return CMediaLibPlaylistMgr::GetMediaLibItemDisplayName(type, name);
    }
    return std::wstring();
}

const CUiMediaLibItemMgr::ItemInfo& CUiMediaLibItemMgr::GetItemInfo(CMediaClassifier::ClassificationType type, int index) const
{
    if (!m_loading)
    {
        auto iter = m_item_map.find(type);
        if (iter != m_item_map.end())
        {
            const auto& item_list{ iter->second };
            if (index >= 0 && index < static_cast<int>(item_list.size()))
            {
                return item_list[index];
            }
        }
    }
    static ItemInfo empty_info{};
    return empty_info;
}

const std::wstring& CUiMediaLibItemMgr::GetItemName(CMediaClassifier::ClassificationType type, int index) const
{
    return GetItemInfo(type, index).name;
}

int CUiMediaLibItemMgr::GetItemSongCount(CMediaClassifier::ClassificationType type, int index) const
{
    return GetItemInfo(type, index).count;
}

void CUiMediaLibItemMgr::SetCurrentName(CMediaClassifier::ClassificationType type, const std::wstring& name)
{
    m_current_name_map[type] = name;
    m_current_index_map.erase(type);
}

int CUiMediaLibItemMgr::GetCurrentIndex(CMediaClassifier::ClassificationType type)
{
    if (!m_loading)
    {
        auto index_iter = m_current_index_map.find(type);
        if (index_iter == m_current_index_map.end())
        {
            //根据名称查找所在的序号
            auto name_iter = m_current_name_map.find(type);
            if (name_iter != m_current_name_map.end())
            {
                const auto& name_list{ m_item_map[type] };
                auto iter = std::find_if(name_list.begin(), name_list.end(), [&](const ItemInfo& item) {
                    return item.name == name_iter->second;
                });
                if (iter != name_list.end())
                {
                    int index = iter - name_list.begin();
                    //设置当前项目的序号
                    m_current_index_map[type] = index;
                    return index;
                }
            }
        }
        else
        {
            return index_iter->second;
        }
    }

    return -1;
}

CUiMediaLibItemMgr::CUiMediaLibItemMgr()
{
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
CUiMyFavouriteItemMgr CUiMyFavouriteItemMgr::m_instance;

CUiMyFavouriteItemMgr::~CUiMyFavouriteItemMgr()
{
}

CUiMyFavouriteItemMgr& CUiMyFavouriteItemMgr::Instance()
{
    return m_instance;
}

int CUiMyFavouriteItemMgr::GetSongCount() const
{
    if (!m_loading)
        return static_cast<int>(m_may_favourite_song_list.size());
    else
        return 0;
}

const SongInfo& CUiMyFavouriteItemMgr::GetSongInfo(int index) const
{
    if (!m_loading)
    {
        if (index >= 0 && index < GetSongCount())
            return m_may_favourite_song_list[index];
    }
    static SongInfo empty_song;
    return empty_song;
}

void CUiMyFavouriteItemMgr::UpdateMyFavourite()
{
    m_loading = true;

    CPlaylistFile playlist_file;
    playlist_file.LoadFromFile(theApp.m_playlist_dir + FAVOURITE_PLAYLIST_NAME);
    playlist_file.MoveToSongList(m_may_favourite_song_list);
    CSongDataManager::GetInstance().LoadSongsInfo(m_may_favourite_song_list);  // 从媒体库加载歌曲属性

    m_loading = false;
}

void CUiMyFavouriteItemMgr::GetSongList(std::vector<SongInfo>& song_list) const
{
    std::copy(m_may_favourite_song_list.begin(), m_may_favourite_song_list.end(), std::back_inserter(song_list));
}

CUiMyFavouriteItemMgr::CUiMyFavouriteItemMgr()
{
}
