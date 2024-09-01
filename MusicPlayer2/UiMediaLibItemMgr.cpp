#include "stdafx.h"
#include "UiMediaLibItemMgr.h"
#include "MusicPlayer2.h"
#include "MediaLibPlaylistMgr.h"
#include "Playlist.h"
#include "SongDataManager.h"
#include "SongInfoHelper.h"
#include "Player.h"

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
    std::shared_lock<std::shared_mutex> lock(m_shared_mutex);

    GetClassifiedMeidaLibItemList(CMediaClassifier::CT_ARTIST);
    GetClassifiedMeidaLibItemList(CMediaClassifier::CT_ALBUM);
    GetClassifiedMeidaLibItemList(CMediaClassifier::CT_GENRE);
    GetClassifiedMeidaLibItemList(CMediaClassifier::CT_YEAR);
    GetClassifiedMeidaLibItemList(CMediaClassifier::CT_TYPE);
    GetClassifiedMeidaLibItemList(CMediaClassifier::CT_BITRATE);
    GetClassifiedMeidaLibItemList(CMediaClassifier::CT_RATING);

    m_loading = false;
    m_inited = true;
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
    std::shared_lock<std::shared_mutex> lock(m_shared_mutex);

    CPlaylistFile playlist_file;
    playlist_file.LoadFromFile(theApp.m_playlist_dir + FAVOURITE_PLAYLIST_NAME);
    playlist_file.MoveToSongList(m_may_favourite_song_list);
    CSongDataManager::GetInstance().LoadSongsInfo(m_may_favourite_song_list);  // 从媒体库加载歌曲属性

    m_loading = false;
    m_inited = true;
}

void CUiMyFavouriteItemMgr::GetSongList(std::vector<SongInfo>& song_list) const
{
    std::copy(m_may_favourite_song_list.begin(), m_may_favourite_song_list.end(), std::back_inserter(song_list));
}

bool CUiMyFavouriteItemMgr::Contains(const SongInfo& song) const
{
    auto iter = std::find_if(m_may_favourite_song_list.begin(), m_may_favourite_song_list.end(), [&](const SongInfo& a) {
        return a.IsSameSong(song);
    });
    return iter != m_may_favourite_song_list.end();
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
    if (!m_loading && m_inited && !song.IsEmpty())
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
    std::shared_lock<std::shared_mutex> lock(m_shared_mutex);

    m_all_tracks_list.clear();
    //从song data中读取
    CSongDataManager::GetInstance().GetSongData([&](const CSongDataManager::SongDataMap& song_data_map) {
        for (const auto& song_info : song_data_map)
        {
            UTrackInfo item;
            item.song_key = song_info.first;
            item.name = CSongInfoHelper::GetDisplayStr(song_info.second, theApp.m_media_lib_setting_data.display_format);
            item.length = song_info.second.length();
            item.is_favourite = CUiMyFavouriteItemMgr::Instance().Contains(song_info.second);
            m_all_tracks_list.push_back(item);
        }
    });

    //从CMediaLibPlaylistMgr中查找“所有曲目”，并获取排序方式
    auto info = CMediaLibPlaylistMgr::Instance().FindItem(CMediaClassifier::CT_NONE, std::wstring());
    //对所有曲目排序
    auto sort_fun = [&](const UTrackInfo& a, const UTrackInfo& b) {
        SongInfo song_a{ CSongDataManager::GetInstance().GetSongInfo(a.song_key) };
        SongInfo song_b{ CSongDataManager::GetInstance().GetSongInfo(b.song_key) };
        switch (info.sort_mode)
        {
        case SM_U_FILE: return SongInfo::ByFileName(song_a, song_b);
        case SM_D_FILE: return SongInfo::ByFileNameDecending(song_a, song_b);
        case SM_U_PATH: return SongInfo::ByPath(song_a, song_b);
        case SM_D_PATH: return SongInfo::ByPathDecending(song_a, song_b);
        case SM_U_TITLE: return SongInfo::ByTitle(song_a, song_b);
        case SM_D_TITLE: return SongInfo::ByTitleDecending(song_a, song_b);
        case SM_U_ARTIST: return SongInfo::ByArtist(song_a, song_b);
        case SM_D_ARTIST: return SongInfo::ByArtistDecending(song_a, song_b);
        case SM_U_ALBUM: return SongInfo::ByAlbum(song_a, song_b);
        case SM_D_ALBUM: return SongInfo::ByAlbumDecending(song_a, song_b);
        case SM_U_TRACK: return SongInfo::ByTrack(song_a, song_b);
        case SM_D_TRACK: return SongInfo::ByTrackDecending(song_a, song_b);
        case SM_U_LISTEN: return SongInfo::ByListenTime(song_a, song_b);
        case SM_D_LISTEN: return SongInfo::ByListenTimeDecending(song_a, song_b);
        case SM_U_TIME: return SongInfo::ByModifiedTime(song_a, song_b);
        case SM_D_TIME: return SongInfo::ByModifiedTimeDecending(song_a, song_b);
        default: return SongInfo::ByFileName(song_a, song_b);
        }
    };
    std::stable_sort(m_all_tracks_list.begin(), m_all_tracks_list.end(), sort_fun);

    m_loading = false;
    m_inited = true;
}

void CUiAllTracksMgr::GetSongList(std::vector<SongInfo>& song_list) const
{
    for (const auto& item : m_all_tracks_list)
    {
        SongInfo song{ CSongDataManager::GetInstance().GetSongInfo(item.song_key) };
        song_list.push_back(song);
    }
}

void CUiAllTracksMgr::AddOrRemoveMyFavourite(int index)
{
    if (!m_loading && m_inited && index >= 0 && index < GetSongCount())
    {
        m_all_tracks_list[index].is_favourite = !m_all_tracks_list[index].is_favourite;
    }
}

CUiAllTracksMgr::CUiAllTracksMgr()
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
CUiFolderExploreMgr CUiFolderExploreMgr::m_instance;

CUiFolderExploreMgr& CUiFolderExploreMgr::Instance()
{
    return m_instance;
}

std::vector<std::shared_ptr<UiElement::TreeElement::Node>>& CUiFolderExploreMgr::GetRootNodes()
{
    if (m_loading)
    {
        static std::vector<std::shared_ptr<UiElement::TreeElement::Node>> empty_nodes;
        return empty_nodes;
    }
    else
    {
        return m_root_nodes;
    }
}

void CUiFolderExploreMgr::UpdateFolders()
{
    m_loading = true;

    m_root_nodes.clear();

    //这里等待播放内核加载完成，否则无法判断文件夹内音频文件数量
    while (!CPlayer::GetInstance().IsPlayerCoreInited())
    {
        Sleep(20);
    }

    //获取媒体库目录
    for (const auto& default_folder : theApp.m_media_lib_setting_data.media_folders)
    {
        int audo_file_count = GetAudioFilesNum(default_folder);
        auto root_node = UiElement::FolderExploreTree::CreateNode(default_folder, audo_file_count, nullptr);
        root_node->collapsed = false;       //默认展开顶级节点
        m_root_nodes.push_back(root_node);
        CreateFolderNodeByPath(default_folder, root_node);
    }

    m_loading = false;
}

CUiFolderExploreMgr::CUiFolderExploreMgr()
{
}

void CUiFolderExploreMgr::CreateFolderNodeByPath(std::wstring path, std::shared_ptr<UiElement::TreeElement::Node> parent)
{
    //文件句柄
    intptr_t hFile = 0;
    //文件信息
    _wfinddata_t fileinfo;
    if (path.back() != '\\' && path.back() != '/')
        path.push_back('\\');
    if ((hFile = _wfindfirst((path + L"*").c_str(), &fileinfo)) != -1)
    {
        do
        {
            std::wstring file_name = fileinfo.name;
            if (file_name == L"." || file_name == L"..")
                continue;

            std::wstring sub_file_path = path + file_name;
            int audo_file_count = GetAudioFilesNum(sub_file_path);
            if (CCommon::IsFolder(sub_file_path) && audo_file_count > 0)        //当前是文件夹且包含音频文件，则递归调用
            {
                auto sub_node = UiElement::FolderExploreTree::CreateNode(file_name, audo_file_count, parent);
                CreateFolderNodeByPath(sub_file_path, sub_node);
            }
        } while (_wfindnext(hFile, &fileinfo) == 0);
    }
    _findclose(hFile);
}

int CUiFolderExploreMgr::GetAudioFilesNum(std::wstring path)
{
    if (path.back() != '\\' && path.back() != '/')
        path.push_back('\\');

    auto iter = m_folder_audio_files_num.find(path);
    if (iter != m_folder_audio_files_num.end())
    {
        return iter->second;
    }
    else
    {
        int audio_file_count = 0;
        //文件句柄
        intptr_t hFile = 0;
        //文件信息
        _wfinddata_t fileinfo;
        if ((hFile = _wfindfirst((path + L"*").c_str(), &fileinfo)) != -1)
        {
            do
            {
                wstring file_name = fileinfo.name;
                if (file_name == L"." || file_name == L"..")
                    continue;

                if (CCommon::IsFolder(path + file_name))        //是文件夹，则递归调用
                {
                    audio_file_count += GetAudioFilesNum(path + file_name);
                }
                else
                {
                    if (CAudioCommon::FileIsAudio(path + file_name))	//找到了音频文件
                        audio_file_count++;
                }
            } while (_wfindnext(hFile, &fileinfo) == 0);
        }
        _findclose(hFile);

        m_folder_audio_files_num[path] = audio_file_count;
        return audio_file_count;
    }
}
