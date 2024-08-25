#include "stdafx.h"
#include "IconMgr.h"
#include "MusicPlayer2.h"

IconMgr::IconMgr() {}

IconMgr::~IconMgr() {}

HICON IconMgr::GetHICON(IconType type, IconStyle style, IconSize size)
{
    if (type == IT_NO_ICON)
        return NULL;
    int key = MakeKey(type, style, size);
    {   // 加读锁查找缓存的HICON
        std::shared_lock<std::shared_mutex> readLock(m_shared_mutex);
        auto iter = m_icon_map.find(key);
        if (iter != m_icon_map.end())
            return iter->second;
    }
    {   // 缓存中不存在时加写锁加载HICON
        std::unique_lock<std::shared_mutex> writeLock(m_shared_mutex);
        auto iter = m_icon_map.find(key);   // 避免并发问题此处需要先再次检查
        if (iter != m_icon_map.end())
            return iter->second;

        auto [id_dark, id_light, id_filled, id_color] = GetIconID(type);
        UINT id{};
        switch (style)
        {
        case IconMgr::IS_OutlinedDark: id = id_dark; break;
        case IconMgr::IS_OutlinedLight: id = id_light; break;
        case IconMgr::IS_Filled: id = id_filled; break;
        case IconMgr::IS_Color: id = id_color; break;
        default: break;
        }
        // 没有要求的风格时fallback到其他风格
        if (!id) id = id_color;
        if (!id) id = id_dark;
        if (!id) id = id_light;
        if (!id) id = id_filled;

        HICON hIcon = NULL;
        if (id)
        {
            int width = GetIconWidth(size);
            if (width)
                hIcon = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(id), IMAGE_ICON, width, width, 0);
            else
                hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(id));  // 全量加载所有分辨率
        }
        m_icon_map.emplace(key, hIcon);         // 即使hIcon为NULL也保存, 这表示当前的图标不存在
        return hIcon;
    }
}

int IconMgr::GetIconWidth(IconSize size)
{
    if (size == IS_ALL)
        return 0;
    int width;
    switch (size)
    {
    case IS_DPI_16: case IS_DPI_16_Full_Screen: width = theApp.DPI(16); break;
    case IS_DPI_20: case IS_DPI_20_Full_Screen: width = theApp.DPI(20); break;
    case IS_DPI_32: case IS_DPI_32_Full_Screen: width = theApp.DPI(32); break;
    case IS_ORG_512: width = 512; break;
    default: width = theApp.DPI(16); break;  // 系统默认小图标大小 SM_CXICON * SM_CYICON
    }
    if (size == IS_DPI_16_Full_Screen || size == IS_DPI_20_Full_Screen || size == IS_DPI_32_Full_Screen)
        width = static_cast<int>(width * CONSTVAL::FULL_SCREEN_ZOOM_FACTOR);
    if (width < 32)
        width = CCommon::IconSizeNormalize(width);
    return width;
}

std::tuple<UINT, UINT, UINT, UINT> IconMgr::GetIconID(IconType type)
{
    switch (type)
    {   // return { 深色的图标, 浅色的图标, 填充风格图标, 彩色图标 };
    case IconMgr::IconType::IT_App:
#ifdef _DEBUG
        return { NULL, NULL, NULL, IDI_APP_DEBUG };
#else
        return { NULL, NULL, NULL, IDR_MAINFRAME };
#endif
    case IconMgr::IconType::IT_App_Monochrome:
        return { IDI_APP_DARK, IDI_APP_LIGHT, NULL, NULL };
    case IconMgr::IconType::IT_Stop:
        return { IDI_STOP_NEW_D, IDI_STOP_NEW, IDI_STOP, NULL };
    case IconMgr::IconType::IT_Play:
        return { IDI_PLAY_NEW_D, IDI_PLAY_NEW, IDI_PLAY, NULL };
    case IconMgr::IconType::IT_Pause:
        return { IDI_PAUSE_NEW_D, IDI_PAUSE_NEW, IDI_PAUSE, NULL };
    case IconMgr::IconType::IT_Play_Pause:
        return { IDI_PLAY_PAUSE_D, NULL, IDI_PLAY_PAUSE, NULL };
    case IconMgr::IconType::IT_Previous:
        return { IDI_PREVIOUS_NEW_D, IDI_PREVIOUS_NEW, IDI_PREVIOUS, NULL };
    case IconMgr::IconType::IT_Next:
        return { IDI_NEXT_NEW_D, IDI_NEXT_NEW, IDI_NEXT1, NULL };
    case IconMgr::IconType::IT_Favorite_On:
        return { IDI_FAVOURITE_D, IDI_FAVOURITE, NULL, NULL };
    case IconMgr::IconType::IT_Favorite_Off:
        return { NULL, NULL, NULL, IDI_HEART };
    case IconMgr::IconType::IT_Media_Lib:
        return { IDI_MEDIA_LIB_D, IDI_MEDIA_LIB, NULL, NULL };
    case IconMgr::IconType::IT_Playlist:
        return { IDI_PLAYLIST_D, IDI_PLAYLIST, NULL, NULL };
    case IconMgr::IconType::IT_Menu:
        return { IDI_MENU_D, IDI_MENU, NULL, NULL };
    case IconMgr::IconType::IT_Full_Screen_On:
        return { IDI_FULL_SCREEN1_D, IDI_FULL_SCREEN1, NULL, NULL };
    case IconMgr::IconType::IT_Full_Screen_Off:
        return { IDI_FULL_SCREEN_D, IDI_FULL_SCREEN, NULL, NULL };
    case IconMgr::IconType::IT_Minimize:
        return { IDI_MINIMIZE_D, IDI_MINIMIZE, NULL, NULL };
    case IconMgr::IconType::IT_Maxmize_On:
        return { IDI_MAXIMIZE_D, IDI_MAXIMIZE, NULL, NULL };
    case IconMgr::IconType::IT_Maxmize_Off:
        return { IDI_RESTORE_D, IDI_RESTORE, NULL, NULL };
    case IconMgr::IconType::IT_Close:
        return { IDI_CLOSE_D, IDI_CLOSE, NULL, NULL };
    case IconMgr::IconType::IT_Play_Order:
        return { IDI_PLAY_ORDER_D, IDI_PLAY_ORDER, NULL, NULL };
    case IconMgr::IconType::IT_Loop_Playlist:
        return { IDI_LOOP_PLAYLIST_D, IDI_LOOP_PLAYLIST, NULL, NULL };
    case IconMgr::IconType::IT_Loop_Track:
        return { IDI_LOOP_TRACK_D, IDI_LOOP_TRACK, NULL, NULL };
    case IconMgr::IconType::IT_Play_Shuffle:
        return { IDI_PLAY_SHUFFLE_D, IDI_PLAY_SHUFFLE, NULL, NULL };
    case IconMgr::IconType::IT_Play_Random:
        return { IDI_PLAY_RANDOM_D, IDI_PLAY_RANDOM, NULL, NULL };
    case IconMgr::IconType::IT_Play_Track:
        return { IDI_PLAY_TRACK_D, IDI_PLAY_TRACK, NULL, NULL };
    case IconMgr::IconType::IT_Setting:
        return { IDI_SETTING_D, IDI_SETTING, NULL, NULL };
    case IconMgr::IconType::IT_Equalizer:
        return { IDI_EQ_D, IDI_EQ, NULL, NULL };
    case IconMgr::IconType::IT_Skin:
        return { IDI_SKIN_D, IDI_SKIN, NULL, NULL };
    case IconMgr::IconType::IT_Mini_On:
        return { IDI_MINI_D, IDI_MINI, NULL, NULL };
    case IconMgr::IconType::IT_Mini_Off:
        return { IDI_MINI_RESTORE_D, IDI_MINI_RESTORE, NULL, NULL };
    case IconMgr::IconType::IT_Info:
        return { IDI_SONG_INFO_D, IDI_SONG_INFO, NULL, NULL };
    case IconMgr::IconType::IT_Find:
        return { IDI_FIND_SONGS_D, IDI_FIND_SONGS, NULL, NULL };
    case IconMgr::IconType::IT_Dark_Mode_On:
        return { IDI_DARK_MODE_D, NULL, NULL, NULL };
    case IconMgr::IconType::IT_Dark_Mode_Off:
        return { NULL, IDI_LIGHT_MODE, NULL, NULL };
    case IconMgr::IconType::IT_Volume0:
        return { IDI_VOLUME0_D, IDI_VOLUME0, NULL, NULL };
    case IconMgr::IconType::IT_Volume1:
        return { IDI_VOLUME1_D, IDI_VOLUME1, NULL, NULL };
    case IconMgr::IconType::IT_Volume2:
        return { IDI_VOLUME2_D, IDI_VOLUME2, NULL, NULL };
    case IconMgr::IconType::IT_Volume3:
        return { IDI_VOLUME3_D, IDI_VOLUME3, NULL, NULL };
    case IconMgr::IconType::IT_Switch_Display:
        return { IDI_SWITCH_D, IDI_SWITCH, NULL, NULL };
    case IconMgr::IconType::IT_Folder:
        return { IDI_SELECT_FOLDER_D, IDI_SELECT_FOLDER, NULL, IDI_EXPLORE_FOLDER };
    case IconMgr::IconType::IT_Music:
        return { IDI_MUSIC_D, NULL, NULL, NULL };
    case IconMgr::IconType::IT_Link:
        return { IDI_LINK_D, IDI_LINK, NULL, NULL };
    case IconMgr::IconType::IT_Exit:
        return { IDI_EXIT_D, NULL, NULL, NULL };
    case IconMgr::IconType::IT_Rewind:
        return { IDI_REW_NEW_D, NULL, IDI_REW, NULL };
    case IconMgr::IconType::IT_Fast_Forward:
        return { IDI_FF_NEW_D, NULL, IDI_FF, NULL };
    case IconMgr::IconType::IT_Speed_Up:
        return { IDI_SPEED_UP_D, NULL, NULL, NULL };
    case IconMgr::IconType::IT_Slow_Down:
        return { IDI_SLOW_DOWN_D, NULL, NULL, NULL };
    case IconMgr::IconType::IT_Add:
        return { IDI_ADD_D, IDI_ADD, NULL, NULL };
    case IconMgr::IconType::IT_Save:
        return { IDI_SAVE_NEW_D, NULL, NULL, NULL };
    case IconMgr::IconType::IT_Save_As:
        return { IDI_SAVE_AS_D, NULL, NULL, NULL };
    case IconMgr::IconType::IT_Sort_Mode:
        return { IDI_SORT_D, IDI_SORT, NULL, NULL };
    case IconMgr::IconType::IT_Playlist_Display_Mode:
        return { IDI_DISPLAY_MODE_D, IDI_DISPLAY_MODE, NULL, NULL };
    case IconMgr::IconType::IT_Locate:
        return { IDI_LOCATE_D, IDI_LOCATE, NULL, NULL };
    case IconMgr::IconType::IT_Lyric:
        return { IDI_LYRIC_D, IDI_LYRIC, NULL, NULL };
    case IconMgr::IconType::IT_Copy:
        return { IDI_COPY, NULL, NULL, NULL };
    case IconMgr::IconType::IT_Edit:
        return { IDI_EDIT_D, NULL, NULL, NULL };
    case IconMgr::IconType::IT_Unlink:
        return { IDI_UNLINK_D, IDI_UNLINK, NULL, NULL };
    case IconMgr::IconType::IT_Folder_Explore:
        return { IDI_FOLDER_EXPLORE_D, IDI_FOLDER_EXPLORE, NULL, NULL };
    case IconMgr::IconType::IT_Internal_Lyric:
        return { IDI_INTERNAL_LYRIC_D, NULL, NULL, NULL };
    case IconMgr::IconType::IT_Download:
        return { IDI_DOWNLOAD_D, NULL, NULL, NULL };
    case IconMgr::IconType::IT_Download_Batch:
        return { IDI_DOWNLOAD1_D, NULL, NULL, NULL };
    case IconMgr::IconType::IT_Playlist_Dock:
        return { IDI_PLAYLIST_DOCK_D, IDI_PLAYLIST_DOCK, NULL, NULL };
    case IconMgr::IconType::IT_Playlist_Float:
        return { IDI_PLAYLIST_FLOAT_D, NULL, NULL, NULL };
    case IconMgr::IconType::IT_Pin:
        return { IDI_PIN_D, NULL, NULL, NULL };
    case IconMgr::IconType::IT_Convert:
        return { IDI_CONVERT_D, NULL, NULL, NULL };
    case IconMgr::IconType::IT_Online:
        return { IDI_ONLINE_D, NULL, NULL, NULL };
    case IconMgr::IconType::IT_Shortcut:
        return { IDI_SHORTCUT_D, NULL, NULL, NULL };
    case IconMgr::IconType::IT_Album_Cover:
        return { IDI_ALBUM_COVER_D, IDI_ALBUM_COVER, NULL, NULL };
    case IconMgr::IconType::IT_Statistics:
        return { IDI_STATISTICS_D, NULL, NULL, NULL };
    case IconMgr::IconType::IT_File_Relate:
        return { IDI_FILE_RELATE_D, IDI_FILE_RELATE, NULL, NULL };
    case IconMgr::IconType::IT_Help:
        return { IDI_HELP_D, IDI_HELP, NULL, NULL };
    case IconMgr::IconType::IT_Fix:
        return { IDI_FIX_D, NULL, NULL, NULL };
    case IconMgr::IconType::IT_Star:
        return { IDI_STAR_D, IDI_STAR, NULL, NULL };
    case IconMgr::IconType::IT_Artist:
        return { IDI_ARTIST_D, IDI_ARTIST, NULL, NULL };
    case IconMgr::IconType::IT_Album:
        return { IDI_ALBUM_D, IDI_ALBUM, NULL, NULL };
    case IconMgr::IconType::IT_Genre:
        return { IDI_GENRE_D, IDI_GENRE, NULL, NULL };
    case IconMgr::IconType::IT_Year:
        return { IDI_YEAR_D, IDI_YEAR, NULL, NULL };
    case IconMgr::IconType::IT_Bitrate:
        return { IDI_BITRATE_D, IDI_BITRATE, NULL, NULL };
    case IconMgr::IconType::IT_History:
        return { IDI_RECENT_SONG_D, IDI_RECENT_SONG, NULL, NULL };
    case IconMgr::IconType::IT_Key_Board:
        return { IDI_HOT_KEY, NULL, NULL, NULL };
    case IconMgr::IconType::IT_Reverb:
        return { IDI_REVERB_D, IDI_REVERB, NULL, NULL };
    case IconMgr::IconType::IT_Double_Line:
        return { IDI_DOUBLE_LINE_D, NULL, NULL, NULL };
    case IconMgr::IconType::IT_Lock:
        return { IDI_LOCK_D, NULL, NULL, NULL };
    case IconMgr::IconType::IT_Play_As_Next:
        return { IDI_PLAY_AS_NEXT, NULL, NULL, NULL };
    case IconMgr::IconType::IT_Rename:
        return { IDI_RENAME_D, NULL, NULL, NULL };
    case IconMgr::IconType::IT_Play_In_Playlist:
        return { IDI_PLAY_IN_PLAYLIST_D, IDI_PLAY_IN_PLAYLIST, NULL, NULL };
    case IconMgr::IconType::IT_Play_In_Folder:
        return { IDI_PLAY_IN_FOLDER, NULL, NULL, NULL };
    case IconMgr::IconType::IT_Tag:
        return { IDI_TAG, NULL, NULL, NULL };
    case IconMgr::IconType::IT_More:
        return { IDI_MORE_D, NULL, NULL, NULL };
    case IconMgr::IconType::IT_NowPlaying:
        return { IDI_NOW_PLAYING_D, IDI_NOW_PLAYING, NULL, NULL };
    case IconMgr::IconType::IT_Karaoke:
        return { IDI_KARAOKE_D, IDI_KARAOKE, NULL, NULL };
    case IconMgr::IconType::IT_Refresh:
        return { IDI_REFRESH_D, IDI_REFRESH, NULL, NULL };
    case IconMgr::IconType::IT_NewFolder:
        return { IDI_NEW_FOLDER_D, IDI_NEW_FOLDER, NULL, NULL };
    case IconMgr::IconType::IT_Background:
        return { IDI_BACKGROUND_D, NULL, NULL, NULL };
    case IconMgr::IconType::IT_Le_Tag_Insert:
        return { NULL, NULL, NULL, IDI_ADD_TAG };
    case IconMgr::IconType::IT_Le_Tag_Replace:
        return { NULL, NULL, NULL, IDI_REPLACE_TAG };
    case IconMgr::IconType::IT_Le_Tag_Delete:
        return { NULL, NULL, NULL, IDI_DELETE_TAG };
    case IconMgr::IconType::IT_Le_Save:
        return { NULL, NULL, NULL, IDI_SAVE };
    case IconMgr::IconType::IT_Le_Find:
        return { NULL, NULL, NULL, IDI_FIND };
    case IconMgr::IconType::IT_Le_Replace:
        return { NULL, NULL, NULL, IDI_REPLACE };
    case IconMgr::IconType::IT_Triangle_Left:
        return { IDI_LYRIC_DELAY_D, NULL, NULL, NULL };
    case IconMgr::IconType::IT_Triangle_Up:
        return { IDI_UP_D, NULL, NULL, NULL };
    case IconMgr::IconType::IT_Triangle_Right:
        return { IDI_LYRIC_FORWARD_D, NULL, NULL, NULL };
    case IconMgr::IconType::IT_Triangle_Down:
        return { IDI_EXPAND_D, IDI_EXPAND, NULL, NULL };
    case IconMgr::IconType::IT_Ok:
        return { IDI_OK_D, NULL, NULL, NULL };
    case IconMgr::IconType::IT_Cancel:
        return { IDI_CLOSE_D, NULL, NULL, NULL };
    case IconMgr::IconType::IT_TreeCollapsed:
        return { IDI_TREE_COLLAPSED_D, IDI_TREE_COLLAPSED, NULL, NULL };
    case IconMgr::IconType::IT_TreeExpanded:
        return { IDI_TREE_EXPANDED_D, IDI_TREE_EXPANDED, NULL, NULL };
    case IconMgr::IconType::IT_Default_Cover_Playing:
        return { NULL, NULL, NULL, IDI_DEFAULT_COVER };
    case IconMgr::IconType::IT_Default_Cover_Stopped:
        return { NULL, NULL, NULL, IDI_DEFAULT_COVER_NOT_PLAYED };
    default:
        return { NULL, NULL, NULL, NULL };
    }
}

