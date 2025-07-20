#include "stdafx.h"
#include "ListItem.h"
#include "MusicPlayer2.h"
#include "CRecentList.h"
#include "FilePathHelper.h"
#include "SongDataManager.h"
#include "SongInfoHelper.h"


ListItem::ListItem(ListType _type, wstring _path, ListItem::ClassificationType _medialib_type)
    : type(_type), path(_path), medialib_type(_medialib_type)
{
}

SortMode ListItem::GetDefaultSortMode() const
{
    if (sort_mode != SM_UNSORT)
        return sort_mode;
    if (empty() || type == LT_PLAYLIST)
        return SM_UNSORT;
    if (type == LT_FOLDER)
        return SM_U_FILE;
    if (type == LT_MEDIA_LIB)
    {
        // 唱片集默认按音轨号排序
        if (medialib_type == ClassificationType::CT_ALBUM)
            return SM_U_TRACK;
        // 其他默认按路径排序
        else
            return SM_U_PATH;
    }
    ASSERT(FALSE);
    return SM_UNSORT;
}

wstring ListItem::GetTypeDisplayName() const
{
    switch (type)
    {
    case LT_FOLDER: return theApp.m_str_table.LoadText(L"TXT_FOLDER");
    case LT_PLAYLIST: return theApp.m_str_table.LoadText(L"TXT_PLAYLIST");
    case LT_MEDIA_LIB:
        switch (medialib_type)
        {
        case ClassificationType::CT_ARTIST: return theApp.m_str_table.LoadText(L"TXT_ARTIST");
        case ClassificationType::CT_ALBUM: return theApp.m_str_table.LoadText(L"TXT_ALBUM");
        case ClassificationType::CT_GENRE: return theApp.m_str_table.LoadText(L"TXT_GENRE");
        case ClassificationType::CT_YEAR: return theApp.m_str_table.LoadText(L"TXT_YEAR");
        case ClassificationType::CT_TYPE: return theApp.m_str_table.LoadText(L"TXT_FILE_TYPE");
        case ClassificationType::CT_BITRATE: return theApp.m_str_table.LoadText(L"TXT_BITRATE");
        case ClassificationType::CT_RATING: return theApp.m_str_table.LoadText(L"TXT_RATING");
        case ClassificationType::CT_NONE: return theApp.m_str_table.LoadText(L"TXT_MEDIA_LIB");
        }
    }
    ASSERT(FALSE);
    return wstring();
}

IconMgr::IconType ListItem::GetTypeIcon() const
{
    switch (type)
    {
    case LT_FOLDER: return IconMgr::IconType::IT_Folder;
    case LT_PLAYLIST:
        if (CRecentList::IsSpecPlaylist(*this, CRecentList::PT_FAVOURITE))
            return  IconMgr::IconType::IT_Favorite_On;
        else
            return IconMgr::IconType::IT_Playlist;
    case LT_MEDIA_LIB:
        switch (medialib_type)
        {
        case ClassificationType::CT_ARTIST: return IconMgr::IconType::IT_Artist;
        case ClassificationType::CT_ALBUM: return IconMgr::IconType::IT_Album;
        case ClassificationType::CT_GENRE: return IconMgr::IconType::IT_Genre;
        case ClassificationType::CT_YEAR: return IconMgr::IconType::IT_Year;
        case ClassificationType::CT_TYPE: return IconMgr::IconType::IT_File_Relate;
        case ClassificationType::CT_BITRATE: return IconMgr::IconType::IT_Bitrate;
        case ClassificationType::CT_RATING: return IconMgr::IconType::IT_Star;
        case ClassificationType::CT_NONE: return IconMgr::IconType::IT_Media_Lib;
        }
    }
    ASSERT(FALSE);
    return IconMgr::IconType::IT_App;
}

wstring ListItem::GetDisplayName() const
{
    switch (type)
    {
    case LT_FOLDER:
        if (!path.empty())
        {
            wstring tmp{ path };
            tmp.pop_back();
            return tmp;
        }
        else
            return L"error folder path";
    case LT_PLAYLIST:
        if (CRecentList::IsSpecPlaylist(*this, CRecentList::PT_DEFAULT))
            return theApp.m_str_table.LoadText(L"TXT_PLAYLIST_NAME_DEFAULT");
        if (CRecentList::IsSpecPlaylist(*this, CRecentList::PT_FAVOURITE))
            return theApp.m_str_table.LoadText(L"TXT_PLAYLIST_NAME_FAVOURITE");
        if (CRecentList::IsSpecPlaylist(*this, CRecentList::PT_TEMP))
            return theApp.m_str_table.LoadText(L"TXT_PLAYLIST_NAME_TEMP");
        return CFilePathHelper(path).GetFileNameWithoutExtension();
    case LT_MEDIA_LIB:
        if (medialib_type == ClassificationType::CT_NONE)   // 所有曲目
            return theApp.m_str_table.LoadText(L"TXT_ALL_TRACKS");
        if (path == STR_OTHER_CLASSIFY_TYPE)                // 显示名称为<其他>时
            return theApp.m_str_table.LoadText(L"TXT_CLASSIFY_OTHER");
        if (!path.empty())                                  // 直接返回项目名
            return path;
        // 项目名path为空表示项目<未知xxx>
        switch (medialib_type)
        {
        case ClassificationType::CT_ARTIST: return theApp.m_str_table.LoadText(L"TXT_EMPTY_ARTIST");
        case ClassificationType::CT_ALBUM: return theApp.m_str_table.LoadText(L"TXT_EMPTY_ALBUM");
        case ClassificationType::CT_GENRE: return theApp.m_str_table.LoadText(L"TXT_EMPTY_GENRE");
        case ClassificationType::CT_YEAR: return theApp.m_str_table.LoadText(L"TXT_EMPTY_YEAR");
        }
    }
    ASSERT(FALSE);
    return wstring();
}

wstring ListItem::GetLastTrackDisplayName() const
{
    if (last_track.path.empty())
        return L"-";
    SongInfo song_info = CSongDataManager::GetInstance().GetSongInfo(last_track);
    if (song_info.file_path.empty())
        song_info.file_path = last_track.path;
    wstring name = CSongInfoHelper::GetDisplayStr(song_info, theApp.m_media_lib_setting_data.display_format);
    return name;
}

