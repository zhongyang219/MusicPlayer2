#include "stdafx.h"
#include "MediaLibPlaylistMgr.h"
#include "MusicPlayer2.h"
#include "SongDataManager.h"

bool MediaLibPlaylistInfo::operator==(const MediaLibPlaylistInfo& other) const
{
    return medialib_type == other.medialib_type && path == other.path;
}

bool MediaLibPlaylistInfo::operator<(const MediaLibPlaylistInfo& other) const
{
    if (medialib_type != other.medialib_type)
        return medialib_type < other.medialib_type;
    else
        return path < other.path;
}

bool MediaLibPlaylistInfo::isValid() const
{
    return (!path.empty() || track_num > 0) && (medialib_type >= 0 && medialib_type < CMediaClassifier::CT_MAX);
}

/////////////////////////////////////////////////////////////////////////////////
CMediaLibPlaylistMgr CMediaLibPlaylistMgr::m_instance;

CMediaLibPlaylistMgr& CMediaLibPlaylistMgr::Instance()
{
    return m_instance;
}

std::vector<SongInfo> CMediaLibPlaylistMgr::GetSongList(CMediaClassifier::ClassificationType type, const std::wstring& name)
{
    if (type == CMediaClassifier::CT_NONE)
    {
        //返回所有曲目
        std::vector<SongInfo> song_list;
        CSongDataManager::GetInstance().GetSongData([&](const CSongDataManager::SongDataMap& song_data_map)
            {
                for (const auto& song_info : song_data_map)
                {
                    song_list.push_back(song_info.second);
                }
            });
        return song_list;
    }
    else
    {
        CMediaClassifier classifier(type, name == STR_OTHER_CLASSIFY_TYPE);
        classifier.ClassifyMedia();
        const auto& song_list{ classifier.GetMeidaList()[name] };
        if (song_list.empty())
        {
            MediaLibPlaylistInfo empty_item;
            empty_item.path = name;
            empty_item.medialib_type = type;
            m_instance.m_empty_items.insert(empty_item);
        }
        return song_list;
    }
}

IconMgr::IconType CMediaLibPlaylistMgr::GetIcon(CMediaClassifier::ClassificationType type)
{
    switch (type)
    {
    case CMediaClassifier::CT_ARTIST: return IconMgr::IT_Artist;
    case CMediaClassifier::CT_ALBUM: return IconMgr::IT_Album;
    case CMediaClassifier::CT_GENRE: return IconMgr::IT_Genre;
    case CMediaClassifier::CT_YEAR: return IconMgr::IT_Year;
    case CMediaClassifier::CT_TYPE: return IconMgr::IT_File_Relate;
    case CMediaClassifier::CT_BITRATE: return IconMgr::IT_Bitrate;
    case CMediaClassifier::CT_RATING: return IconMgr::IT_Star;
    case CMediaClassifier::CT_NONE: return IconMgr::IT_Media_Lib;
    }
    return IconMgr::IT_App;
}

std::wstring CMediaLibPlaylistMgr::GetTypeName(CMediaClassifier::ClassificationType type)
{
    switch (type)
    {
    case CMediaClassifier::CT_ARTIST: return theApp.m_str_table.LoadText(L"TXT_ARTIST");
    case CMediaClassifier::CT_ALBUM: return theApp.m_str_table.LoadText(L"TXT_ALBUM");
    case CMediaClassifier::CT_GENRE: return theApp.m_str_table.LoadText(L"TXT_GENRE");
    case CMediaClassifier::CT_YEAR: return theApp.m_str_table.LoadText(L"TXT_YEAR");
    case CMediaClassifier::CT_TYPE: return theApp.m_str_table.LoadText(L"TXT_FILE_TYPE");
    case CMediaClassifier::CT_BITRATE: return theApp.m_str_table.LoadText(L"TXT_BITRATE");
    case CMediaClassifier::CT_RATING: return theApp.m_str_table.LoadText(L"TXT_RATING");
    case CMediaClassifier::CT_NONE: return theApp.m_str_table.LoadText(L"TXT_MEDIA_LIB");
    }
    return std::wstring();
}

SortMode CMediaLibPlaylistMgr::GetDefaultSortMode(CMediaClassifier::ClassificationType type)
{
    //唱片集默认按音轨号排序
    if (type == CMediaClassifier::CT_ALBUM)
        return SM_U_TRACK;
    //其他默认按路径排序
    else
        return SM_U_PATH;
}

std::wstring CMediaLibPlaylistMgr::GetMediaLibItemDisplayName(CMediaClassifier::ClassificationType type, const std::wstring medialib_item_name)
{
    //所有曲目
    if (type == CMediaClassifier::CT_NONE)
        return theApp.m_str_table.LoadText(L"TXT_ALL_TRACKS");

    //显示名称为<其他>时
    if (medialib_item_name == STR_OTHER_CLASSIFY_TYPE)
        return theApp.m_str_table.LoadText(L"TXT_CLASSIFY_OTHER");
    //显示名称为<未知xxx>时
    if (medialib_item_name.empty())
    {
        switch (type)
        {
        case CMediaClassifier::CT_ARTIST: return theApp.m_str_table.LoadText(L"TXT_EMPTY_ARTIST");
        case CMediaClassifier::CT_ALBUM: return theApp.m_str_table.LoadText(L"TXT_EMPTY_ALBUM");
        case CMediaClassifier::CT_GENRE: return theApp.m_str_table.LoadText(L"TXT_EMPTY_GENRE");
        case CMediaClassifier::CT_YEAR: return theApp.m_str_table.LoadText(L"TXT_EMPTY_YEAR");
        }
    }
    return medialib_item_name;
}

void CMediaLibPlaylistMgr::EmplaceMediaLibPlaylist(CMediaClassifier::ClassificationType type, const wstring& name, int track, int pos, int track_num, int total_time, unsigned __int64 last_played_time, SortMode sort_mode)
{
    std::shared_lock<std::shared_mutex> lock(m_shared_mutex);
    MediaLibPlaylistInfo playlist_info;
    playlist_info.path = name;
    playlist_info.track = track;
    playlist_info.position = pos;
    playlist_info.track_num = track_num;
    playlist_info.total_time = total_time;
    playlist_info.last_played_time = last_played_time;
    playlist_info.medialib_type = type;
    playlist_info.sort_mode = sort_mode;

    for (size_t i{ 0 }; i < m_media_lib_playlist.size(); i++)
    {
        if (playlist_info == m_media_lib_playlist[i])
            m_media_lib_playlist.erase(m_media_lib_playlist.begin() + i);   //如果媒体库项目已经在列表中，就先把它列表中删除
    }

    if (playlist_info.isValid() && !m_empty_items.contains(playlist_info))
        m_media_lib_playlist.push_front(playlist_info);                       //如果媒体库项目不是空的，就插入到m_recent_playlists的前面
}

MediaLibPlaylistInfo CMediaLibPlaylistMgr::GetCurrentPlaylistInfo() const
{
    std::shared_lock<std::shared_mutex> lock(m_shared_mutex);
    if (!m_media_lib_playlist.empty())
        return m_media_lib_playlist.front();
    return MediaLibPlaylistInfo();
}

bool CMediaLibPlaylistMgr::DeleteItem(const MediaLibPlaylistInfo* item)
{
    if (item != nullptr)
    {
        std::shared_lock<std::shared_mutex> lock(m_shared_mutex);
        for (auto iter = m_media_lib_playlist.begin(); iter != m_media_lib_playlist.end(); ++iter)
        {
            if (&(*iter) == item)
            {
                m_media_lib_playlist.erase(iter);
                return true;
            }
        }
    }
    return false;
}

void CMediaLibPlaylistMgr::IterateItems(std::function<void(const MediaLibPlaylistInfo&)> func)
{
    std::shared_lock<std::shared_mutex> lock(m_shared_mutex);
    for (const auto& item : m_media_lib_playlist)
    {
        func(item);
    }
}

MediaLibPlaylistInfo CMediaLibPlaylistMgr::FindItem(CMediaClassifier::ClassificationType type, const wstring& name) const
{
    std::shared_lock<std::shared_mutex> lock(m_shared_mutex);
    for (auto iter = m_media_lib_playlist.begin(); iter != m_media_lib_playlist.end(); ++iter)
    {
        if (iter->medialib_type == type && iter->path == name)
            return *iter;
    }
    return MediaLibPlaylistInfo();
}

void CMediaLibPlaylistMgr::SavePlaylistData()
{
    // 打开或者新建文件
    CFile file;
    BOOL bRet = file.Open(theApp.m_recent_medialib_playlist_path.c_str(),
        CFile::modeCreate | CFile::modeWrite);
    if (!bRet)		//打开文件失败
    {
        return;
    }
    std::shared_lock<std::shared_mutex> lock(m_shared_mutex);
    // 构造CArchive对象
    CArchive ar(&file, CArchive::store);
    // 写数据
    ar << 1;        //写入数据文件版本

    ar << static_cast<unsigned int>(m_media_lib_playlist.size());		//写入m_media_lib_playlist的大小
    for (auto& path_info : m_media_lib_playlist)
    {
        ar << CString(path_info.path.c_str())
            << path_info.track
            << path_info.position
            << path_info.track_num
            << path_info.total_time
            << path_info.last_played_time
            << path_info.medialib_type
            << path_info.sort_mode
            ;
    }
    // 关闭CArchive对象
    ar.Close();
    // 关闭文件
    file.Close();
}

void CMediaLibPlaylistMgr::LoadPlaylistData()
{
    std::shared_lock<std::shared_mutex> lock(m_shared_mutex);
    m_media_lib_playlist.clear();

    // 打开文件
    CFile file;
    BOOL bRet = file.Open(theApp.m_recent_medialib_playlist_path.c_str(), CFile::modeRead);
    if (!bRet)		//文件不存在
    {
        return;
    }
    // 构造CArchive对象
    CArchive ar(&file, CArchive::load);
    // 读数据
    unsigned int size{};
    try
    {
        int version{};
        ar >> version;

        ar >> size;		//读取长度
        for (unsigned int i{}; i < size; i++)
        {
            MediaLibPlaylistInfo path_info;
            CString strTmp;
            ar >> strTmp;
            path_info.path = strTmp.GetString();
            ar >> path_info.track;
            ar >> path_info.position;
            ar >> path_info.track_num;
            ar >> path_info.total_time;
            ar >> path_info.last_played_time;
            int medialib_type{};
            ar >> medialib_type;
            path_info.medialib_type = static_cast<CMediaClassifier::ClassificationType>(medialib_type);
            int sort_mode{};
            ar >> sort_mode;
            path_info.sort_mode = static_cast<SortMode>(sort_mode);

            if (path_info.isValid())
                m_media_lib_playlist.push_back(path_info);
        }
    }
    catch (CArchiveException* exception)
    {
        //捕获序列化时出现的异常
        wstring info = theApp.m_str_table.LoadTextFormat(L"MSG_SERIALIZE_ERROR", { theApp.m_recent_medialib_playlist_path, exception->m_cause });
        theApp.WriteLog(info);
    }
    // 关闭对象
    ar.Close();
    // 关闭文件
    file.Close();
}
