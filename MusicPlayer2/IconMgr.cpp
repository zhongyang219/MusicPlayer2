#include "stdafx.h"
#include "IconMgr.h"
#include "MusicPlayer2.h"

IconMgr::IconMgr() {}

IconMgr::~IconMgr() {}

HICON IconMgr::GetHICON(IconType type, IconStyle style, IconSize size)
{
    int key = MakeKey(type, style, size);
    auto iter = m_icon_map.find(key);
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
    // 没有要求的风格时fallback到其他风格（如果存在）
    if (!id) id = id_color;
    if (!id) id = id_dark;
    if (!id) id = id_light;
    if (!id) id_filled;

    HICON hIcon = NULL;
    if (id)
    {
        auto [cx, cy] = GetIconSize(size);
        hIcon = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(id), IMAGE_ICON, cx, cy, cx ? 0 : LR_DEFAULTSIZE);
    }
    m_icon_map.emplace(key, hIcon);         // 即使hIcon为NULL也保存, 这表示当前的图标不存在
    return hIcon;
}

CSize IconMgr::GetIconSize(IconSize size)
{
    int width;
    switch (size)
    {
    case IS_DPI_16: case IS_DPI_16_Full_Screen: width = theApp.DPI(16); break;
    case IS_DPI_20: case IS_DPI_20_Full_Screen: width = theApp.DPI(20); break;
    case IS_DPI_32: case IS_DPI_32_Full_Screen: width = theApp.DPI(32); break;
    case IS_ORG_512: width = 512; break;
    default: width = 0; break;  // 系统默认图标大小 SM_CXICON * SM_CYICON, 此时返回0
    }
    if (size == IS_DPI_16_Full_Screen || size == IS_DPI_20_Full_Screen || size == IS_DPI_32_Full_Screen)
        width = static_cast<int>(width * CONSTVAL::FULL_SCREEN_ZOOM_FACTOR);
    if (width < 32)
        width = CCommon::IconSizeNormalize(width);
    return CSize(width, width);
}

std::tuple<UINT, UINT, UINT, UINT> IconMgr::GetIconID(IconType type)
{
    switch (type)
    {   // return { 深色的图标, 浅色的图标, 填充风格图标, 彩色图标 };
    case IconMgr::IT_App:
#ifdef _DEBUG
        return { IDI_APP_DARK, IDI_APP_LIGHT, NULL, IDI_APP_DEBUG };
#else
        return { IDI_APP_DARK, IDI_APP_LIGHT, NULL, IDR_MAINFRAME };
#endif
    case IconMgr::IT_Stop:
        return { IDI_STOP_NEW_D, IDI_STOP_NEW, IDI_STOP, NULL };
    case IconMgr::IT_Play:
        return { IDI_PLAY_NEW_D, IDI_PLAY_NEW, IDI_PLAY, NULL };
    case IconMgr::IT_Pause:
        return { IDI_PAUSE_NEW_D, IDI_PAUSE_NEW, IDI_PAUSE, NULL };
    case IconMgr::IT_Play_Pause:
        return { IDI_PLAY_PAUSE_D, NULL, IDI_PLAY_PAUSE, NULL };
    case IconMgr::IT_Previous:
        return { IDI_PREVIOUS_NEW_D, IDI_PREVIOUS_NEW, IDI_PREVIOUS, NULL };
    case IconMgr::IT_Next:
        return { IDI_NEXT_NEW_D, IDI_NEXT_NEW, IDI_NEXT1, NULL };
    case IconMgr::IT_Favorite_On:
        return { IDI_FAVOURITE_D, IDI_FAVOURITE, NULL, NULL };
    case IconMgr::IT_Favorite_Off:
        return { NULL, NULL, NULL, IDI_HEART };
    case IconMgr::IT_Media_Lib:
        return { IDI_MEDIA_LIB_D, IDI_MEDIA_LIB, NULL, NULL };
    case IconMgr::IT_Playlist:
        return { IDI_PLAYLIST_D, IDI_PLAYLIST, NULL, NULL };
    case IconMgr::IT_Menu:
        return { IDI_MENU_D, IDI_MENU, NULL, NULL };
    case IconMgr::IT_Full_Screen_On:
        return { IDI_FULL_SCREEN1_D, IDI_FULL_SCREEN1, NULL, NULL };
    case IconMgr::IT_Full_Screen_Off:
        return { IDI_FULL_SCREEN_D, IDI_FULL_SCREEN, NULL, NULL };
    case IconMgr::IT_Minimize:
        return { IDI_MINIMIZE_D, IDI_MINIMIZE, NULL, NULL };
    case IconMgr::IT_Maxmize_On:
        return { IDI_MAXIMIZE_D, IDI_MAXIMIZE, NULL, NULL };
    case IconMgr::IT_Maxmize_Off:
        return { IDI_RESTORE_D, IDI_RESTORE, NULL, NULL };
    case IconMgr::IT_Close:
        return { IDI_CLOSE_D, IDI_CLOSE, NULL, NULL };
    case IconMgr::IT_Play_Order:
        return { IDI_PLAY_ORDER_D, IDI_PLAY_ORDER, NULL, NULL };
    case IconMgr::IT_Loop_Playlist:
        return { IDI_LOOP_PLAYLIST_D, IDI_LOOP_PLAYLIST, NULL, NULL };
    case IconMgr::IT_loop_track:
        return { IDI_LOOP_TRACK_D, IDI_LOOP_TRACK, NULL, NULL };
    case IconMgr::IT_Play_Shuffle:
        return { IDI_PLAY_SHUFFLE_D, IDI_PLAY_SHUFFLE, NULL, NULL };
    case IconMgr::IT_Play_Random:
        return { IDI_PLAY_RANDOM_D, IDI_PLAY_RANDOM, NULL, NULL };
    case IconMgr::IT_Play_Track:
        return { IDI_PLAY_TRACK_D, IDI_PLAY_TRACK, NULL, NULL };
    case IconMgr::IT_Setting:
        return { IDI_SETTING_D, IDI_SETTING, NULL, NULL };
    case IconMgr::IT_Equalizer:
        return { IDI_EQ_D, IDI_EQ, NULL, NULL };
    case IconMgr::IT_Skin:
        return { IDI_SKIN_D, IDI_SKIN, NULL, NULL };
    case IconMgr::IT_Mini_On:
        return { IDI_MINI_D, IDI_MINI, NULL, NULL };
    case IconMgr::IT_Mini_Off:
        return { IDI_MINI_RESTORE_D, IDI_MINI_RESTORE, NULL, NULL };
    case IconMgr::IT_Info:
        return { IDI_SONG_INFO_D, IDI_SONG_INFO, NULL, NULL };
    case IconMgr::IT_Find:
        return { IDI_FIND_SONGS_D, IDI_FIND_SONGS, NULL, NULL };
    case IconMgr::IT_Dark_Mode_On:
        return { IDI_DARK_MODE_D, NULL, NULL, NULL };
    case IconMgr::IT_Dark_Mode_Off:
        return { NULL, IDI_LIGHT_MODE, NULL, NULL };
    case IconMgr::IT_Volume0:
        return { IDI_VOLUME0_D, IDI_VOLUME0, NULL, NULL };
    case IconMgr::IT_Volume1:
        return { IDI_VOLUME1_D, IDI_VOLUME1, NULL, NULL };
    case IconMgr::IT_Volume2:
        return { IDI_VOLUME2_D, IDI_VOLUME2, NULL, NULL };
    case IconMgr::IT_Volume3:
        return { IDI_VOLUME3_D, IDI_VOLUME3, NULL, NULL };
    case IconMgr::IT_Switch_Display:
        return { IDI_SWITCH_D, IDI_SWITCH, NULL, NULL };
    case IconMgr::IT_Folder:
        return { IDI_SELECT_FOLDER_D, IDI_SELECT_FOLDER, NULL, IDI_EXPLORE_FOLDER };
    case IconMgr::IT_Lyric:
        return { IDI_LYRIC_D, IDI_LYRIC, NULL, NULL };
    case IconMgr::IT_Playlist_Dock:
        return { IDI_PLAYLIST_DOCK_D, IDI_PLAYLIST_DOCK, NULL, NULL };
    case IconMgr::IT_Help:
        return { IDI_HELP_D, IDI_HELP, NULL, NULL };
    case IconMgr::IT_Locate:
        return { IDI_LOCATE_D, IDI_LOCATE, NULL, NULL };
    case IconMgr::IT_Triangle_Left:
        return { IDI_LYRIC_DELAY_D, NULL, NULL, NULL };
    case IconMgr::IT_Triangle_Up:
        return { IDI_UP_D, NULL, NULL, NULL };
    case IconMgr::IT_Triangle_Right:
        return { IDI_LYRIC_FORWARD_D, NULL, NULL, NULL };
    case IconMgr::IT_Triangle_Down:
        return { IDI_EXPAND_D, IDI_EXPAND, NULL, NULL };
    default:
        return { NULL, NULL, NULL, NULL };
    }
}

