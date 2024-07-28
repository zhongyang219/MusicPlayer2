#include "stdafx.h"
#include "UiMediaLibItemMgr.h"
#include "MusicPlayer2.h"
#include "MediaLibPlaylistMgr.h"
#include "Playlist.h"
#include "SongDataManager.h"
#include "SongInfoHelper.h"

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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
CUiAllTracksMgr CUiAllTracksMgr::m_instance;

CUiAllTracksMgr::~CUiAllTracksMgr()
{
}

CUiAllTracksMgr& CUiAllTracksMgr::Instance()
{
    return m_instance;
}

int CUiAllTracksMgr::GetSongCount() const
{
    if (!m_loading)
        return static_cast<int>(m_all_tracks_list.size());
    else
        return 0;
}

SongInfo CUiAllTracksMgr::GetSongInfo(int index) const
{
    if (!m_loading)
    {
        if (index >= 0 && index < GetSongCount())
            return CSongDataManager::GetInstance().GetSongInfo(m_all_tracks_list[index].song_key);
    }
    static SongInfo empty_song;
    return empty_song;
}

const CUiAllTracksMgr::UTrackInfo& CUiAllTracksMgr::GetItem(int index) const
{
    if (!m_loading)
    {
        if (index >= 0 && index < GetSongCount())
            return m_all_tracks_list[index];
    }
    static UTrackInfo empty_item;
    return empty_item;
}

int CUiAllTracksMgr::GetCurrentIndex() const
{
    return m_current_index;
}

void CUiAllTracksMgr::SetCurrentSong(const SongInfo& song)
{
    if (!m_loading)
    {
        int index{};
        for (const auto& item : m_all_tracks_list)
        {
            if (std::equal_to<SongKey>()(SongKey(song), item.song_key))
            {
                m_current_index = index;
                break;
            }
            index++;
        }
    }
}

void CUiAllTracksMgr::UpdateAllTracks()
{
    m_loading = true;

    m_all_tracks_list.clear();
    CSongDataManager::GetInstance().GetSongData([&](const CSongDataManager::SongDataMap& song_data_map) {
        for (const auto& song_info : song_data_map)
        {
            UTrackInfo item;
            item.song_key = song_info.first;
            item.name = CSongInfoHelper::GetDisplayStr(song_info.second, theApp.m_media_lib_setting_data.display_format);
            item.length = song_info.second.length();
            m_all_tracks_list.push_back(item);
        }
    });

    m_loading = false;
}

void CUiAllTracksMgr::GetSongList(std::vector<SongInfo>& song_list) const
{
    for (const auto& item : m_all_tracks_list)
    {
        SongInfo song{ CSongDataManager::GetInstance().GetSongInfo(item.song_key) };
        song_list.push_back(song);
    }
}

CUiAllTracksMgr::CUiAllTracksMgr()
{
}
