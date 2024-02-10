#include "stdafx.h"
#include "MusicPlayer2.h"
#include "MenuMgr.h"
#include "WIC.h"
#include "WinVersionHelper.h"


MenuMgr::MenuBase::MenuBase(MenuMgr* pMenuMgr, MenuMgr::MenuType menu_type)
    : m_pMenuMgr(pMenuMgr), m_menu_type(menu_type)
{
}

MenuMgr::MenuBase::~MenuBase()
{
}

void MenuMgr::MenuBase::CreateMenu(bool is_popup, bool add_accelerator)
{
    m_add_accelerator = add_accelerator;
    if (is_popup)
        m_menu.CreatePopupMenu();
    else
        m_menu.CreateMenu();
}

void MenuMgr::MenuBase::AppendItem(UINT wID, const wstring& id_text, HICON hicon, const wchar_t* text)
{
    wstring menu_text;                      // mii使用其内部数据指针，此对象需要有效到api完成
    MENUITEMINFO mii = { sizeof(mii) };
    mii.fMask = MIIM_ID | MIIM_STRING | MIIM_FTYPE;
    mii.wID = wID;
    if (id_text.empty() && text != nullptr) // 空id_text的话直接使用参数text做正文（给UpdateMenu开的后门）
        mii.dwTypeData = const_cast<LPWSTR>(text);
    else
    {
        menu_text = theApp.m_str_table.LoadMenuText(MenuMgr::GetMenuNameStr(m_menu_type), id_text);
        if (text != nullptr)                // 由参数直接指定的快捷键优先级更高
        {
            menu_text.append(L"\t").append(text);
        }
        else if (m_add_accelerator)         // 如果CreateMenu时要求自动附加Accelerator快捷键描述
        {
            wstring shortcut = theApp.m_accelerator_res.GetShortcutDescriptionById(wID);
            if (!shortcut.empty())
                menu_text += L'\t' + shortcut;
        }
        mii.dwTypeData = const_cast<LPWSTR>(menu_text.c_str());
    }
#ifndef COMPILE_IN_WIN_XP
    if (hicon)
    {
        HBITMAP hbmp;
        CMenuIcon::GetBitmapByIcon(hicon, hbmp);
        if (hbmp)
        {
            mii.fMask |= MIIM_BITMAP;
            mii.hbmpItem = hbmp;
        }
    }
#endif
    m_menu.InsertMenuItemW(m_end_pos++, &mii, TRUE);
}

void MenuMgr::MenuBase::AppendSubMenu(MenuMgr::MenuType sub_menu_type, HICON hicon)
{
    MENUITEMINFO mii = { sizeof(mii) };
    mii.fMask = MIIM_STRING | MIIM_SUBMENU | MIIM_FTYPE;
    mii.hSubMenu = m_pMenuMgr->GetSafeHmenu(sub_menu_type);
    wstring menu_text = theApp.m_str_table.LoadMenuText(MenuMgr::GetMenuNameStr(m_menu_type), MenuMgr::GetMenuNameStr(sub_menu_type));
    mii.dwTypeData = const_cast<LPWSTR>(menu_text.c_str());
#ifndef COMPILE_IN_WIN_XP
    if (hicon)
    {
        HBITMAP hbmp;
        CMenuIcon::GetBitmapByIcon(hicon, hbmp);
        if (hbmp)
        {
            mii.fMask |= MIIM_BITMAP;
            mii.hbmpItem = hbmp;
        }
    }
#endif
    m_menu.InsertMenuItemW(m_end_pos++, &mii, TRUE);
}

void MenuMgr::MenuBase::AppendSeparator()
{
    m_menu.AppendMenuW(MF_SEPARATOR);
    m_end_pos++;
}

void MenuMgr::MenuBase::SetDefaultItem()
{
    ASSERT(m_end_pos > 0);
    m_menu.SetDefaultItem(m_end_pos - 1, TRUE);
}

void MenuMgr::MenuBase::UpdateMenu(const vector<MenuItem>& items)
{
    ASSERT(m_update_pos != -1);
    static wstring empty{ L"" };
    // 将items插入菜单中m_update_pos位置，会替换掉上次设置的内容（此方法不能为一个菜单更新两段内容）
    int bk_org_cnt = m_end_pos;         // m_end_pos为菜单初始化后的初始项目数
    m_end_pos = m_update_pos;           // 也是新项目的插入点
    int cnt = m_menu.GetMenuItemCount() - bk_org_cnt;
    while (cnt--)                       // 删除旧的外部提供菜单项
        m_menu.DeleteMenu(m_update_pos, MF_BYPOSITION);   // DeleteMenu如果用于移除子菜单项会销毁子菜单句柄
    for (const auto& item : items)      // 插入新的外部提供菜单项
    {
        if (item.id != NULL)
            AppendItem(item.id, empty, item.hicon, item.text.c_str());
        else
            AppendSeparator();
    }
    m_end_pos = bk_org_cnt;             // 恢复为初始项目数
    // 这里有一点小问题，如果不销毁重建菜单句柄那么菜单会记忆其所需过的文本宽度，也就是不会变窄（至少我不知道怎样做）
    // 销毁重建会使得这里的共享子菜单机制不能正常工作
}

MenuMgr::MenuMgr()
{
}

MenuMgr::~MenuMgr()
{
}

void MenuMgr::UpdateMenu(MenuType menu_type, const vector<MenuItem>& items)
{
    MenuBase& menu = GetMenuBase(menu_type);
    menu.UpdateMenu(items);
}

HMENU MenuMgr::GetSafeHmenu(MenuType menu_type)
{
    return GetMenuBase(menu_type).m_menu.GetSafeHmenu();
}

CMenu* MenuMgr::GetMenu(MenuType menu_type)
{
    return &GetMenuBase(menu_type).m_menu;
}

MenuMgr::MenuBase& MenuMgr::GetMenuBase(MenuType menu_type)
{
    if (m_menus[menu_type] != nullptr)
    {
        // 此断言失败说明有代码错误销毁了它之前得到的菜单句柄，菜单句柄生命周期由MenuBase对象负责，其他位置只能使用
        ASSERT(m_menus[menu_type]->m_menu.GetSafeHmenu());
        return *m_menus[menu_type];
    }
    m_menus[menu_type] = std::make_unique<MenuBase>(this, menu_type);
    CreateMenu(*m_menus[menu_type]);           // 内部创建子菜单时会使用MenuBase构造得到的this指针调用MenuMgr::GetSafeHmenu递归
    return *m_menus[menu_type];
}

const wchar_t* MenuMgr::GetMenuNameStr(MenuType menu_type)
{
    // 这里记录菜单在language.ini的键名，当需要同样翻译的不同菜单时（比如是否为其附加全局快捷键）
    // 可以为不同的菜单枚举使用相同的键名以复用相同的翻译字符串（建议仅为子集关系的菜单这样做）
    switch (menu_type)
    {
    case MenuMgr::MainPopupMenu:
        return L"MAIN_POP";
    case MenuMgr::MainMenu:
        return L"MAIN";
    case MenuMgr::MainFileMenu:
        return L"MAIN_FILE";
    case MenuMgr::MainPlayCtrlMenu:
        return L"MAIN_PLAYCTRL";
    case MenuMgr::MainPlayCtrlRepeatModeMenu:
        return L"MAIN_PLAYCTRL_REPEAT_MODE";
    case MenuMgr::MainPlayCtrlAbRepeatMenu:
        return L"MAIN_PLAYCTRL_AB";
    case MenuMgr::MainPlaylistMenu:
        return L"MAIN_PLAYLIST";
    case MenuMgr::MainPlaylistAddMenu:
        return L"MAIN_PLAYLIST_ADD";
    case MenuMgr::MainPlaylistDelMenu:
        return L"MAIN_PLAYLIST_DEL";
    case MenuMgr::MainPlaylistSortMenu:
        return L"MAIN_PLAYLIST_SORT";
    case MenuMgr::MainPlaylistDispModeMenu:
        return L"MAIN_PLAYLIST_DISP_MODE";
    case MenuMgr::MainLyricMenu:
        return L"MAIN_LRC";
    case MenuMgr::MainLrcChConvMenu:
        return L"MAIN_LRC_CH_CONV";
    case MenuMgr::MainLrcInnerLrcMenu:
        return L"MAIN_LRC_INNER_LRC";
    case MenuMgr::MainViewMenu:
        return L"MAIN_VIEW";
    case MenuMgr::MainViewSwitchUiMenu:
        return L"MAIN_VIEW_SWITCH_UI";
    case MenuMgr::MainToolMenu:
        return L"MAIN_TOOL";
    case MenuMgr::MainToolCreateShortcutMenu:
        return L"MAIN_TOOL_SHORTCUT";
    case MenuMgr::MainToolAlbumCoverMenu:
        return L"MAIN_TOOL_ALBUM_COVER";
    case MenuMgr::MainToolDelCurPlayingMenu:
        return L"MAIN_TOOL_DEL_CUR_PLAYING";
    case MenuMgr::MainHelpMenu:
        return L"MAIN_HELP";
    case MenuMgr::PlaylistToolBarMenu:
        return L"PLAYLIST_TOOLBAR";
    case MenuMgr::PlaylistToolBarListMenu:
        return L"PLAYLIST_TOOLBAR_LIST";
    case MenuMgr::PlaylistToolBarEditMenu:
        return L"PLAYLIST_TOOLBAR_EDIT";
    case MenuMgr::PlaylistToolBarFolderMenu:
        return L"PLAYLIST_TOOLBAR_FOLDER";
    case MenuMgr::MainAreaMenu:
        return L"MAIN_AREA";
    case MenuMgr::MainAreaLrcMenu:
        return L"MAIN_LRC";
    case MenuMgr::MainAreaPlaylistBtnMenu:
        return L"MAIN_AREA_PLAYLIST_BTN";
    case MenuMgr::DlrcMenu:
        return L"DLRC";
    case MenuMgr::DlrcDefMenu:
        return L"DLRC_DEFAULT_STYLE";
    case MenuMgr::RecentFolderPlaylistMenu:
        return L"RECENT_FOLDER_PLAYLIST";
    case MenuMgr::PlaylistMenu:
        return L"PLAYLIST";
    case MenuMgr::RateMenu:
        return L"RATE";
    case MenuMgr::AddToPlaylistMenu:
        return L"ADD_TO_PLAYLIST";
    case MenuMgr::MiniAreaMenu:
        return L"MINI_AREA";
    case MenuMgr::MiniModeSwitchUiMenu:
        return L"MINI_SWITCH_UI";
    case MenuMgr::NotifyMenu:
        return L"NOTIFY";
    case MenuMgr::LibSetPathMenu:
        return L"LIB_SET_PATH";
    case MenuMgr::LibPlaylistMenu:
        return L"LIB_PLAYLIST";
    case MenuMgr::LibLeftMenu:
        return L"LIB_LEFT";
    case MenuMgr::LibRightMenu:
        return L"LIB_RIGHT";
    case MenuMgr::LeMenu:
        return L"LE";
    case MenuMgr::LeFileMenu:
        return L"LE_FILE";
    case MenuMgr::LeEditMenu:
        return L"LE_EDIT";
    case MenuMgr::LeEditChConvMenu:
        return L"LE_EDIT_CH_CONV";
    case MenuMgr::LeEditTagDislocMenu:
        return L"LE_EDIT_TAG_DISLOC";
    case MenuMgr::LePlayCtrlMenu:
        return L"LE_PLAY_CTRL";
    case MenuMgr::FcListMenu:
        return L"FC_LIST";
    case MenuMgr::LdListMenu:
        return L"LD_LIST";
    case MenuMgr::OptDlrcDefStyleMenu:
        return L"OPT_DLRC_DEFAULT_STYLE";
    case MenuMgr::OptDlrcModDefStyleMenu:
        return L"OPT_DLRC_MOD_DEF_STYLE";
    case MenuMgr::PropertyAdvMenu:
        return L"PROPERTY_ADV";
    case MenuMgr::PropertyCoverMenu:
        return L"PROPERTY_COVER";
    default:
        return L"";
        break;
    }
}

#define APPEND_ITEM(id, ...) AppendItem(id, L#id, __VA_ARGS__)

void MenuMgr::CreateMenu(MenuBase& menu)
{
    // git log 47698b win11下使用复选功能的菜单项不设置图标
    static bool is_win11OrLater = CWinVersionHelper::IsWindows11OrLater();
    switch (menu.GetMenuType())
    {
    case MenuMgr::MainPopupMenu:
        menu.CreateMenu(true, true);
        menu.AppendSubMenu(MainFileMenu, theApp.m_icon_set.select_folder.GetIcon(true));
        menu.AppendSubMenu(MainPlayCtrlMenu, theApp.m_icon_set.play_new.GetIcon(true));
        menu.AppendSubMenu(MainPlaylistMenu, theApp.m_icon_set.show_playlist.GetIcon(true));
        menu.AppendSubMenu(MainLyricMenu, theApp.m_icon_set.lyric.GetIcon(true));
        menu.AppendSubMenu(MainViewMenu, theApp.m_icon_set.playlist_dock.GetIcon(true));
        menu.AppendSubMenu(MainToolMenu, theApp.m_icon_set.setting.GetIcon(true));
        menu.AppendSubMenu(MainHelpMenu, theApp.m_icon_set.help.GetIcon(true));
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_MENU_EXIT, theApp.m_icon_set.exit);
#ifdef _DEBUG
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_TEST, NULL);
        menu.APPEND_ITEM(ID_TEST_DIALOG, NULL);
#endif
        break;
    case MenuMgr::MainMenu:
        menu.CreateMenu(false, true);
        menu.AppendSubMenu(MainFileMenu, NULL);
        menu.AppendSubMenu(MainPlayCtrlMenu, NULL);
        menu.AppendSubMenu(MainPlaylistMenu, NULL);
        menu.AppendSubMenu(MainLyricMenu, NULL);
        menu.AppendSubMenu(MainViewMenu, NULL);
        menu.AppendSubMenu(MainToolMenu, NULL);
        menu.AppendSubMenu(MainHelpMenu, NULL);
        break;
    case MenuMgr::MainFileMenu:
        menu.CreateMenu(true, true);
        menu.APPEND_ITEM(ID_FILE_OPEN, theApp.m_icon_set.music);
        menu.APPEND_ITEM(ID_FILE_OPEN_FOLDER, theApp.m_icon_set.select_folder.GetIcon(true));
        menu.APPEND_ITEM(ID_FILE_OPEN_URL, theApp.m_icon_set.link.GetIcon(true));
        menu.APPEND_ITEM(ID_FILE_OPEN_PLAYLIST, theApp.m_icon_set.show_playlist.GetIcon(true));
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_MENU_EXIT, theApp.m_icon_set.exit);
        break;
    case MenuMgr::MainPlayCtrlMenu:
        menu.CreateMenu(true, true);
        menu.APPEND_ITEM(ID_PLAY_PAUSE, theApp.m_icon_set.play_pause);
        menu.APPEND_ITEM(ID_STOP, theApp.m_icon_set.stop_new);
        menu.APPEND_ITEM(ID_PREVIOUS, theApp.m_icon_set.previous_new.GetIcon(true));
        menu.APPEND_ITEM(ID_NEXT, theApp.m_icon_set.next_new.GetIcon(true));
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_REW, theApp.m_icon_set.rew_new);
        menu.APPEND_ITEM(ID_FF, theApp.m_icon_set.ff_new);
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_SPEED_UP, theApp.m_icon_set.speed_up);
        menu.APPEND_ITEM(ID_SLOW_DOWN, theApp.m_icon_set.slow_down);
        menu.APPEND_ITEM(ID_ORIGINAL_SPEED, NULL);
        menu.AppendSeparator();
        menu.AppendSubMenu(MainPlayCtrlRepeatModeMenu, NULL);
        menu.AppendSubMenu(MainPlayCtrlAbRepeatMenu, NULL);
        break;
    case MenuMgr::MainPlayCtrlRepeatModeMenu:
        menu.CreateMenu(true, false);
        menu.APPEND_ITEM(ID_PLAY_ORDER, theApp.m_icon_set.play_oder.GetIcon(true));
        menu.APPEND_ITEM(ID_PLAY_SHUFFLE, theApp.m_icon_set.play_shuffle.GetIcon(true));
        menu.APPEND_ITEM(ID_PLAY_RANDOM, theApp.m_icon_set.play_random.GetIcon(true));
        menu.APPEND_ITEM(ID_LOOP_PLAYLIST, theApp.m_icon_set.loop_playlist.GetIcon(true));
        menu.APPEND_ITEM(ID_LOOP_TRACK, theApp.m_icon_set.loop_track.GetIcon(true));
        menu.APPEND_ITEM(ID_PLAY_TRACK, theApp.m_icon_set.play_track.GetIcon(true));
        break;
    case MenuMgr::MainPlayCtrlAbRepeatMenu:
        menu.CreateMenu(true, true);
        menu.APPEND_ITEM(ID_AB_REPEAT, NULL);
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_SET_A_POINT, NULL);
        menu.APPEND_ITEM(ID_SET_B_POINT, NULL);
        menu.APPEND_ITEM(ID_NEXT_AB_REPEAT, NULL);
        menu.APPEND_ITEM(ID_RESET_AB_REPEAT, NULL);
        break;
    case MenuMgr::MainPlaylistMenu:
        menu.CreateMenu(true, true);
        menu.AppendSubMenu(MainPlaylistAddMenu, theApp.m_icon_set.add.GetIcon(true));
        menu.AppendSubMenu(MainPlaylistDelMenu, theApp.m_icon_set.close.GetIcon(true));
        menu.APPEND_ITEM(ID_RELOAD_PLAYLIST, theApp.m_icon_set.loop_playlist.GetIcon(true));
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_SAVE_AS_NEW_PLAYLIST, theApp.m_icon_set.save_new);
        menu.APPEND_ITEM(ID_SAVE_CURRENT_PLAYLIST_AS, theApp.m_icon_set.save_as);
        menu.AppendSeparator();
        menu.AppendSubMenu(MainPlaylistSortMenu, theApp.m_icon_set.sort.GetIcon(true));
        menu.AppendSubMenu(MainPlaylistDispModeMenu, theApp.m_icon_set.display_mode.GetIcon(true));
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_LOCATE_TO_CURRENT, theApp.m_icon_set.locate.GetIcon(true));
        break;
    case MenuMgr::MainPlaylistAddMenu:
        menu.CreateMenu(true, true);
        menu.APPEND_ITEM(ID_PLAYLIST_ADD_FILE, theApp.m_icon_set.music);
        menu.APPEND_ITEM(ID_PLAYLIST_ADD_FOLDER, theApp.m_icon_set.select_folder.GetIcon(true));
        menu.APPEND_ITEM(ID_PLAYLIST_ADD_URL, theApp.m_icon_set.link.GetIcon(true));
        break;
    case MenuMgr::MainPlaylistDelMenu:
        menu.CreateMenu(true, true);
        menu.APPEND_ITEM(ID_REMOVE_FROM_PLAYLIST, theApp.m_icon_set.close.GetIcon(true));
        menu.APPEND_ITEM(ID_DELETE_FROM_DISK, NULL);
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_EMPTY_PLAYLIST, NULL);
        menu.APPEND_ITEM(ID_REMOVE_SAME_SONGS, NULL);
        menu.APPEND_ITEM(ID_REMOVE_INVALID_ITEMS, NULL);
        break;
    case MenuMgr::MainPlaylistSortMenu:
        menu.CreateMenu(true, true);
        menu.APPEND_ITEM(ID_SORT_BY_FILE, NULL);
        menu.APPEND_ITEM(ID_SORT_BY_PATH, NULL);
        menu.APPEND_ITEM(ID_SORT_BY_TITLE, NULL);
        menu.APPEND_ITEM(ID_SORT_BY_ARTIST, NULL);
        menu.APPEND_ITEM(ID_SORT_BY_ALBUM, NULL);
        menu.APPEND_ITEM(ID_SORT_BY_TRACK, NULL);
        menu.APPEND_ITEM(ID_SORT_BY_MODIFIED_TIME, NULL);
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_ACCENDING_ORDER, NULL);
        menu.APPEND_ITEM(ID_DESENDING_ORDER, NULL);
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_INVERT_PLAYLIST, NULL);
        break;
    case MenuMgr::MainPlaylistDispModeMenu:
        menu.CreateMenu(true, true);
        menu.APPEND_ITEM(ID_DISP_FILE_NAME, NULL);
        menu.APPEND_ITEM(ID_DISP_TITLE, NULL);
        menu.APPEND_ITEM(ID_DISP_ARTIST_TITLE, NULL);
        menu.APPEND_ITEM(ID_DISP_TITLE_ARTIST, NULL);
        break;
    case MenuMgr::MainLyricMenu:
        menu.CreateMenu(true, true);
        menu.APPEND_ITEM(ID_RELOAD_LYRIC, theApp.m_icon_set.loop_playlist.GetIcon(true));
        menu.APPEND_ITEM(ID_COPY_CURRENT_LYRIC, theApp.m_icon_set.copy);
        menu.APPEND_ITEM(ID_COPY_ALL_LYRIC, NULL);
        menu.APPEND_ITEM(ID_EDIT_LYRIC, theApp.m_icon_set.edit.GetIcon(true));
        menu.AppendSubMenu(MainLrcChConvMenu, NULL);
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_SHOW_LYRIC_TRANSLATE, NULL);
        menu.APPEND_ITEM(ID_SHOW_DESKTOP_LYRIC, NULL);
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_LYRIC_FORWARD, theApp.m_icon_set.lyric_forward.GetIcon(true));
        menu.APPEND_ITEM(ID_LYRIC_DELAY, theApp.m_icon_set.lyric_delay.GetIcon(true));
        menu.APPEND_ITEM(ID_SAVE_MODIFIED_LYRIC, theApp.m_icon_set.save_new);
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_RELATE_LOCAL_LYRIC, theApp.m_icon_set.lyric.GetIcon(true));
        menu.APPEND_ITEM(ID_DELETE_LYRIC, theApp.m_icon_set.close.GetIcon(true));
        menu.APPEND_ITEM(ID_UNLINK_LYRIC, theApp.m_icon_set.unlink.GetIcon(true));
        menu.APPEND_ITEM(ID_BROWSE_LYRIC, theApp.m_icon_set.folder_explore.GetIcon(true));
        menu.AppendSubMenu(MainLrcInnerLrcMenu, theApp.m_icon_set.internal_lyric);
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_DOWNLOAD_LYRIC, theApp.m_icon_set.download);
        menu.APPEND_ITEM(ID_LYRIC_BATCH_DOWNLOAD, theApp.m_icon_set.download1);
        break;
    case MenuMgr::MainLrcChConvMenu:
        menu.CreateMenu(true, true);
        menu.APPEND_ITEM(ID_TRANSLATE_TO_SIMPLIFIED_CHINESE, NULL);
        menu.APPEND_ITEM(ID_TRANSLATE_TO_TRANDITIONAL_CHINESE, NULL);
        break;
    case MenuMgr::MainLrcInnerLrcMenu:
        menu.CreateMenu(true, true);
        menu.APPEND_ITEM(ID_EMBED_LYRIC_TO_AUDIO_FILE, NULL);
        menu.APPEND_ITEM(ID_DELETE_LYRIC_FROM_AUDIO_FILE, NULL);
        break;
    case MenuMgr::MainViewMenu:
        menu.CreateMenu(true, true);
        menu.APPEND_ITEM(ID_SHOW_PLAYLIST, is_win11OrLater ? NULL : theApp.m_icon_set.playlist_dock.GetIcon(true));
        menu.APPEND_ITEM(ID_FLOAT_PLAYLIST, is_win11OrLater ? NULL : theApp.m_icon_set.playlist_float);
        menu.APPEND_ITEM(ID_USE_STANDARD_TITLE_BAR, NULL);
        menu.APPEND_ITEM(ID_SHOW_MENU_BAR, is_win11OrLater ? NULL : theApp.m_icon_set.menu.GetIcon(true));
        menu.APPEND_ITEM(ID_ALWAYS_SHOW_STATUS_BAR, NULL);
        menu.APPEND_ITEM(ID_ALWAYS_ON_TOP, is_win11OrLater ? NULL : theApp.m_icon_set.pin);
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_MINI_MODE, theApp.m_icon_set.mini.GetIcon(true));
        menu.APPEND_ITEM(ID_FULL_SCREEN, theApp.m_icon_set.full_screen1.GetIcon(true));
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_DARK_MODE, theApp.m_icon_set.dark_mode.GetIcon(true));
        menu.AppendSubMenu(MainViewSwitchUiMenu, theApp.m_icon_set.skin.GetIcon(true));
        break;
    case MenuMgr::MainViewSwitchUiMenu:
        menu.CreateMenu(true, true);
        menu.APPEND_ITEM(ID_SWITCH_UI, theApp.m_icon_set.skin.GetIcon(true));
        menu.AppendSeparator();
        menu.SetUpdatePos();            // 设置Update插入点
        break;
    case MenuMgr::MainToolMenu:
        menu.CreateMenu(true, true);
        menu.APPEND_ITEM(ID_MEDIA_LIB, theApp.m_icon_set.media_lib.GetIcon(true));
        menu.APPEND_ITEM(ID_FIND, theApp.m_icon_set.find_songs.GetIcon(true));
        menu.APPEND_ITEM(ID_EXPLORE_PATH, theApp.m_icon_set.folder_explore.GetIcon(true));
        menu.APPEND_ITEM(ID_SONG_INFO, theApp.m_icon_set.info.GetIcon(true));
        menu.APPEND_ITEM(ID_EQUALIZER, theApp.m_icon_set.eq.GetIcon(true));
        menu.APPEND_ITEM(ID_FORMAT_CONVERT1, theApp.m_icon_set.convert);
        menu.APPEND_ITEM(ID_CURRENT_EXPLORE_ONLINE, theApp.m_icon_set.online);
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_RE_INI_BASS, theApp.m_icon_set.loop_playlist.GetIcon(true));
        menu.AppendSubMenu(MainToolCreateShortcutMenu, theApp.m_icon_set.shortcut);
        menu.AppendSeparator();
        menu.AppendSubMenu(MainToolAlbumCoverMenu, theApp.m_icon_set.album_cover);
        menu.AppendSubMenu(MainToolDelCurPlayingMenu, theApp.m_icon_set.close.GetIcon(true));
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_LISTEN_STATISTICS, theApp.m_icon_set.statistics);
        menu.APPEND_ITEM(ID_TOOL_FILE_RELATE, theApp.m_icon_set.file_relate);
        menu.APPEND_ITEM(ID_OPTION_SETTINGS, theApp.m_icon_set.setting.GetIcon(true));
        break;
    case MenuMgr::MainToolCreateShortcutMenu:
        menu.CreateMenu(true, true);
        menu.APPEND_ITEM(ID_CREATE_DESKTOP_SHORTCUT, NULL);
        menu.APPEND_ITEM(ID_CREATE_PLAY_SHORTCUT, NULL);
        menu.APPEND_ITEM(ID_CREATE_MINI_MODE_SHORT_CUT, NULL);
        break;
    case MenuMgr::MainToolAlbumCoverMenu:
        menu.CreateMenu(true, true);
        menu.APPEND_ITEM(ID_DOWNLOAD_ALBUM_COVER, theApp.m_icon_set.download);
        menu.APPEND_ITEM(ID_ALBUM_COVER_SAVE_AS, theApp.m_icon_set.save_as);
        menu.APPEND_ITEM(ID_DELETE_ALBUM_COVER, theApp.m_icon_set.close.GetIcon(true));
        menu.APPEND_ITEM(ID_ALBUM_COVER_INFO, theApp.m_icon_set.album_cover);
        break;
    case MenuMgr::MainToolDelCurPlayingMenu:
        menu.CreateMenu(true, true);
        menu.APPEND_ITEM(ID_REMOVE_CURRENT_FROM_PLAYLIST, NULL);
        menu.APPEND_ITEM(ID_DELETE_CURRENT_FROM_DISK, NULL);
        break;
    case MenuMgr::MainHelpMenu:
        menu.CreateMenu(true, true);
        menu.APPEND_ITEM(ID_HELP, theApp.m_icon_set.help.GetIcon(true), L"F1");
        menu.APPEND_ITEM(ID_ONLINE_HELP, NULL);
        menu.APPEND_ITEM(ID_HELP_FAQ, NULL);
        menu.APPEND_ITEM(ID_HELP_CUSTOM_UI, NULL);
        menu.APPEND_ITEM(ID_HELP_UPDATE_LOG, NULL);
        menu.APPEND_ITEM(ID_SUPPORTED_FORMAT, NULL);
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_APP_ABOUT, theApp.m_icon_set.app.GetIcon());
        break;
    case MenuMgr::PlaylistToolBarMenu:
        menu.CreateMenu(true, true);
        menu.AppendSubMenu(MainPlaylistAddMenu, theApp.m_icon_set.add.GetIcon(true));
        menu.AppendSubMenu(MainPlaylistDelMenu, theApp.m_icon_set.close.GetIcon(true));
        menu.AppendSubMenu(MainPlaylistSortMenu, theApp.m_icon_set.sort.GetIcon(true));
        menu.AppendSubMenu(PlaylistToolBarListMenu, theApp.m_icon_set.show_playlist.GetIcon(true));
        menu.AppendSubMenu(PlaylistToolBarEditMenu, theApp.m_icon_set.edit.GetIcon(true));
        menu.AppendSubMenu(PlaylistToolBarFolderMenu, theApp.m_icon_set.select_folder.GetIcon(true));
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_LOCATE_TO_CURRENT, theApp.m_icon_set.locate.GetIcon(true));
        break;
    case MenuMgr::PlaylistToolBarListMenu:
        menu.CreateMenu(true, true);
        menu.APPEND_ITEM(ID_RELOAD_PLAYLIST, theApp.m_icon_set.loop_playlist.GetIcon(true));
        menu.APPEND_ITEM(ID_SAVE_AS_NEW_PLAYLIST, theApp.m_icon_set.save_new);
        menu.APPEND_ITEM(ID_SAVE_CURRENT_PLAYLIST_AS, theApp.m_icon_set.save_as);
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_PLAYLIST_FIX_PATH_ERROR, theApp.m_icon_set.fix);
        menu.AppendSubMenu(MainPlaylistDispModeMenu, theApp.m_icon_set.display_mode.GetIcon(true));
        menu.APPEND_ITEM(ID_PLAYLIST_OPTIONS, theApp.m_icon_set.setting.GetIcon(true));
        break;
    case MenuMgr::PlaylistToolBarEditMenu:
        menu.CreateMenu(true, true);
        menu.AppendSubMenu(AddToPlaylistMenu, theApp.m_icon_set.add.GetIcon(true));
        menu.APPEND_ITEM(ID_COPY_FILE_TO, NULL);
        menu.APPEND_ITEM(ID_MOVE_FILE_TO, NULL);
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_MOVE_PLAYLIST_ITEM_UP, NULL);
        menu.APPEND_ITEM(ID_MOVE_PLAYLIST_ITEM_DOWN, NULL);
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_PLAYLIST_SELECT_ALL, NULL);
        menu.APPEND_ITEM(ID_PLAYLIST_SELECT_NONE, NULL);
        menu.APPEND_ITEM(ID_PLAYLIST_SELECT_REVERT, NULL);
        break;
    case MenuMgr::PlaylistToolBarFolderMenu:
        menu.CreateMenu(true, true);
        menu.APPEND_ITEM(ID_FILE_OPEN_FOLDER, theApp.m_icon_set.select_folder.GetIcon(true));
        menu.APPEND_ITEM(ID_CONTAIN_SUB_FOLDER, NULL);
        break;
    case MenuMgr::MainAreaMenu:
        menu.CreateMenu(true, true);
        menu.APPEND_ITEM(ID_SONG_INFO, theApp.m_icon_set.info.GetIcon(true));
        menu.AppendSubMenu(MainPlayCtrlRepeatModeMenu, NULL);
        menu.AppendSubMenu(AddToPlaylistMenu, theApp.m_icon_set.add.GetIcon(true));
        menu.AppendSubMenu(RateMenu, theApp.m_icon_set.star);
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_MEDIA_LIB, theApp.m_icon_set.media_lib.GetIcon(true));
        menu.APPEND_ITEM(ID_FIND, theApp.m_icon_set.find_songs.GetIcon(true));
        menu.APPEND_ITEM(ID_EXPLORE_PATH, theApp.m_icon_set.folder_explore.GetIcon(true));
        menu.APPEND_ITEM(ID_EQUALIZER, theApp.m_icon_set.eq.GetIcon(true));
        menu.APPEND_ITEM(ID_CURRENT_EXPLORE_ONLINE, theApp.m_icon_set.online);
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_DOWNLOAD_ALBUM_COVER, theApp.m_icon_set.album_cover);
        menu.APPEND_ITEM(ID_ALBUM_COVER_SAVE_AS, theApp.m_icon_set.save_as);
        menu.APPEND_ITEM(ID_DELETE_ALBUM_COVER, theApp.m_icon_set.close.GetIcon(true));
        menu.APPEND_ITEM(ID_ALWAYS_USE_EXTERNAL_ALBUM_COVER, NULL);
        menu.APPEND_ITEM(ID_ALBUM_COVER_INFO, theApp.m_icon_set.album_cover);
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_VIEW_ARTIST, theApp.m_icon_set.artist.GetIcon(true));
        menu.APPEND_ITEM(ID_VIEW_ALBUM, theApp.m_icon_set.album.GetIcon(true));
        menu.AppendSeparator();
        menu.AppendSubMenu(MainViewSwitchUiMenu, theApp.m_icon_set.skin.GetIcon(true));
        menu.APPEND_ITEM(ID_OPTION_SETTINGS, theApp.m_icon_set.setting.GetIcon(true));
        break;
    case MenuMgr::MainAreaLrcMenu:
        menu.CreateMenu(true, true);
        menu.APPEND_ITEM(ID_RELOAD_LYRIC, theApp.m_icon_set.loop_playlist.GetIcon(true));
        menu.APPEND_ITEM(ID_COPY_CURRENT_LYRIC, theApp.m_icon_set.copy);
        menu.APPEND_ITEM(ID_COPY_ALL_LYRIC, NULL);
        menu.APPEND_ITEM(ID_EDIT_LYRIC, theApp.m_icon_set.edit.GetIcon(true));
        menu.AppendSubMenu(MainLrcChConvMenu, NULL);
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_SHOW_LYRIC_TRANSLATE, NULL);
        menu.APPEND_ITEM(ID_SHOW_DESKTOP_LYRIC, NULL);
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_LYRIC_FORWARD, theApp.m_icon_set.lyric_forward.GetIcon(true));
        menu.APPEND_ITEM(ID_LYRIC_DELAY, theApp.m_icon_set.lyric_delay.GetIcon(true));
        menu.APPEND_ITEM(ID_SAVE_MODIFIED_LYRIC, theApp.m_icon_set.save_new);
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_RELATE_LOCAL_LYRIC, theApp.m_icon_set.lyric.GetIcon(true));
        menu.APPEND_ITEM(ID_DELETE_LYRIC, theApp.m_icon_set.close.GetIcon(true));
        menu.APPEND_ITEM(ID_UNLINK_LYRIC, theApp.m_icon_set.unlink.GetIcon(true));
        menu.APPEND_ITEM(ID_BROWSE_LYRIC, theApp.m_icon_set.folder_explore.GetIcon(true));
        menu.AppendSubMenu(MainLrcInnerLrcMenu, theApp.m_icon_set.internal_lyric);
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_DOWNLOAD_LYRIC, theApp.m_icon_set.download);
        menu.APPEND_ITEM(ID_LYRIC_BATCH_DOWNLOAD, theApp.m_icon_set.download1);
        menu.AppendSeparator();
        menu.AppendSubMenu(MainViewSwitchUiMenu, theApp.m_icon_set.skin.GetIcon(true));
        menu.APPEND_ITEM(ID_OPTION_SETTINGS, theApp.m_icon_set.setting.GetIcon(true));
        break;
    case MenuMgr::MainAreaPlaylistBtnMenu:
        menu.CreateMenu(true, false);
        menu.APPEND_ITEM(ID_DOCKED_PLAYLIST, NULL);
        menu.APPEND_ITEM(ID_FLOATED_PLAYLIST, NULL);
        break;
    case MenuMgr::DlrcMenu:
        menu.CreateMenu(true, true);
        menu.APPEND_ITEM(ID_PLAY_PAUSE, theApp.m_icon_set.play_pause);
        menu.APPEND_ITEM(ID_STOP, theApp.m_icon_set.stop_new);
        menu.APPEND_ITEM(ID_PREVIOUS, theApp.m_icon_set.previous_new.GetIcon(true));
        menu.APPEND_ITEM(ID_NEXT, theApp.m_icon_set.next_new.GetIcon(true));
        menu.AppendSubMenu(MainPlayCtrlRepeatModeMenu, NULL);
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_SHOW_MAIN_WINDOW, NULL);
        menu.APPEND_ITEM(ID_LYRIC_DISPLAYED_DOUBLE_LINE, theApp.m_icon_set.double_line.GetIcon(true));
        menu.APPEND_ITEM(ID_LYRIC_BACKGROUND_PENETRATE, theApp.m_icon_set.skin.GetIcon(true));
        menu.APPEND_ITEM(ID_LOCK_DESKTOP_LRYIC, theApp.m_icon_set.lock.GetIcon(true));
        menu.AppendSubMenu(DlrcDefMenu, theApp.m_icon_set.media_lib.GetIcon(true));
        menu.APPEND_ITEM(ID_OPTION_SETTINGS, theApp.m_icon_set.setting.GetIcon(true));
        menu.APPEND_ITEM(ID_CLOSE_DESKTOP_LYRIC, theApp.m_icon_set.close.GetIcon(true));
        break;
    case MenuMgr::DlrcDefMenu:
        menu.CreateMenu(true, true);
        menu.APPEND_ITEM(ID_LYRIC_DEFAULT_STYLE1, NULL);
        menu.APPEND_ITEM(ID_LYRIC_DEFAULT_STYLE2, NULL);
        menu.APPEND_ITEM(ID_LYRIC_DEFAULT_STYLE3, NULL);
        break;
    case MenuMgr::RecentFolderPlaylistMenu:
        menu.CreateMenu(true, false);
        menu.SetUpdatePos();            // 设置Update插入点
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_MEDIA_LIB, theApp.m_icon_set.media_lib.GetIcon(true));
        break;
    case MenuMgr::PlaylistMenu:
        menu.CreateMenu(true, true);
        menu.APPEND_ITEM(ID_PLAY_ITEM, theApp.m_icon_set.play_new.GetIcon(true));
        menu.SetDefaultItem();
        menu.APPEND_ITEM(ID_PLAY_AS_NEXT, theApp.m_icon_set.play_as_next);
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_EXPLORE_ONLINE, theApp.m_icon_set.online);
        menu.APPEND_ITEM(ID_FORMAT_CONVERT, theApp.m_icon_set.convert);
        menu.AppendSubMenu(RateMenu, theApp.m_icon_set.star);
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_RELOAD_PLAYLIST, theApp.m_icon_set.loop_playlist.GetIcon(true));
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_REMOVE_FROM_PLAYLIST, theApp.m_icon_set.close.GetIcon(true));
        menu.APPEND_ITEM(ID_DELETE_FROM_DISK, NULL);
        menu.AppendSeparator();
        menu.AppendSubMenu(AddToPlaylistMenu, theApp.m_icon_set.add.GetIcon(true));
        menu.APPEND_ITEM(ID_COPY_FILE_TO, NULL);
        menu.APPEND_ITEM(ID_MOVE_FILE_TO, NULL);
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_MOVE_PLAYLIST_ITEM_UP, NULL);
        menu.APPEND_ITEM(ID_MOVE_PLAYLIST_ITEM_DOWN, NULL);
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_PLAYLIST_VIEW_ARTIST, theApp.m_icon_set.artist.GetIcon(true));
        menu.APPEND_ITEM(ID_PLAYLIST_VIEW_ALBUM, theApp.m_icon_set.album.GetIcon(true));
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_RENAME, theApp.m_icon_set.rename);
        menu.APPEND_ITEM(ID_EXPLORE_TRACK, theApp.m_icon_set.folder_explore.GetIcon(true));
        menu.APPEND_ITEM(ID_ITEM_PROPERTY, theApp.m_icon_set.info.GetIcon(true));
        break;
    case MenuMgr::RateMenu:
        menu.CreateMenu(true, true);
        menu.APPEND_ITEM(ID_RATING_1, NULL);
        menu.APPEND_ITEM(ID_RATING_2, NULL);
        menu.APPEND_ITEM(ID_RATING_3, NULL);
        menu.APPEND_ITEM(ID_RATING_4, NULL);
        menu.APPEND_ITEM(ID_RATING_5, NULL);
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_RATING_NONE, NULL);
        break;
    case MenuMgr::AddToPlaylistMenu:
        menu.CreateMenu(true, true);
        menu.APPEND_ITEM(ID_ADD_TO_NEW_PLAYLIST, theApp.m_icon_set.add.GetIcon(true));
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_ADD_TO_DEFAULT_PLAYLIST, NULL);
        menu.APPEND_ITEM(ID_ADD_TO_MY_FAVOURITE, theApp.m_icon_set.favourite.GetIcon(true));
        menu.SetUpdatePos();            // 设置Update插入点
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_ADD_TO_OTHER_PLAYLIST, theApp.m_icon_set.show_playlist.GetIcon(true));
        break;
    case MenuMgr::MiniAreaMenu:
        menu.CreateMenu(true, true);
        menu.APPEND_ITEM(ID_SONG_INFO, theApp.m_icon_set.info.GetIcon(true));
        menu.AppendSubMenu(MainPlayCtrlRepeatModeMenu, NULL);
        menu.AppendSubMenu(AddToPlaylistMenu, theApp.m_icon_set.add.GetIcon(true));
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_DOWNLOAD_LYRIC, theApp.m_icon_set.download);
        menu.APPEND_ITEM(ID_DOWNLOAD_ALBUM_COVER, theApp.m_icon_set.album_cover);
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_ADD_REMOVE_FROM_FAVOURITE, theApp.m_icon_set.favourite.GetIcon(true));
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_MEDIA_LIB, theApp.m_icon_set.media_lib.GetIcon(true));
        menu.APPEND_ITEM(ID_FIND, theApp.m_icon_set.find_songs.GetIcon(true));
        menu.APPEND_ITEM(ID_EXPLORE_PATH, theApp.m_icon_set.folder_explore.GetIcon(true));
        menu.APPEND_ITEM(ID_EQUALIZER, theApp.m_icon_set.eq.GetIcon(true));
        menu.APPEND_ITEM(ID_OPTION_SETTINGS, theApp.m_icon_set.setting.GetIcon(true));
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_SHOW_PLAY_LIST, is_win11OrLater ? NULL : theApp.m_icon_set.show_playlist.GetIcon(true), L"Ctrl+L");
        menu.APPEND_ITEM(ID_MINI_MODE_ALWAYS_ON_TOP, is_win11OrLater ? NULL : theApp.m_icon_set.pin);
        menu.AppendSubMenu(MiniModeSwitchUiMenu, theApp.m_icon_set.skin.GetIcon(true));
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_MINI_MIDE_MINIMIZE, theApp.m_icon_set.minimize.GetIcon(true));
        menu.APPEND_ITEM(IDOK, theApp.m_icon_set.mini_restore.GetIcon(true), L"Esc");
        menu.APPEND_ITEM(ID_MINI_MODE_EXIT, theApp.m_icon_set.exit, L"Ctrl+X");
        break;
    case MenuMgr::MiniModeSwitchUiMenu:
        menu.CreateMenu(true, true);
        menu.APPEND_ITEM(ID_MINIMODE_UI_DEFAULT, NULL);
        menu.SetUpdatePos();            // 设置Update插入点
        break;
    case MenuMgr::NotifyMenu:
        menu.CreateMenu(true, false);
        menu.APPEND_ITEM(ID_PLAY_PAUSE, theApp.m_icon_set.play_pause);
        menu.APPEND_ITEM(ID_PREVIOUS, theApp.m_icon_set.previous_new.GetIcon(true));
        menu.APPEND_ITEM(ID_NEXT, theApp.m_icon_set.next_new.GetIcon(true));
        menu.AppendSubMenu(MainPlayCtrlRepeatModeMenu, NULL);
        menu.APPEND_ITEM(ID_MINIMODE_RESTORE, theApp.m_icon_set.mini.GetIcon(true));
        menu.APPEND_ITEM(ID_SHOW_DESKTOP_LYRIC, theApp.m_icon_set.lyric.GetIcon(true));
        menu.APPEND_ITEM(ID_LOCK_DESKTOP_LRYIC, theApp.m_icon_set.lock.GetIcon(true));
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_OPTION_SETTINGS, theApp.m_icon_set.setting.GetIcon(true));
        menu.APPEND_ITEM(ID_MENU_EXIT, theApp.m_icon_set.exit);
        break;
    case MenuMgr::LibSetPathMenu:
        menu.CreateMenu(true, false);
        menu.APPEND_ITEM(ID_PLAY_PATH, theApp.m_icon_set.play_new.GetIcon(true));
        menu.SetDefaultItem();
        menu.APPEND_ITEM(ID_DELETE_PATH, theApp.m_icon_set.close.GetIcon(true));
        menu.APPEND_ITEM(ID_BROWSE_PATH, theApp.m_icon_set.folder_explore.GetIcon(true));
        menu.APPEND_ITEM(ID_CONTAIN_SUB_FOLDER, NULL);
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_CLEAR_INVALID_PATH, NULL);
        break;
    case MenuMgr::LibPlaylistMenu:
        menu.CreateMenu(true, false);
        menu.APPEND_ITEM(ID_PLAY_PLAYLIST, theApp.m_icon_set.play_new.GetIcon(true));
        menu.SetDefaultItem();
        menu.APPEND_ITEM(ID_RENAME_PLAYLIST, theApp.m_icon_set.rename);
        menu.APPEND_ITEM(ID_DELETE_PLAYLIST, theApp.m_icon_set.close.GetIcon(true));
        menu.APPEND_ITEM(ID_SAVE_AS_NEW_PLAYLIST, theApp.m_icon_set.save_new);
        menu.APPEND_ITEM(ID_PLAYLIST_SAVE_AS, theApp.m_icon_set.save_as);
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_PLAYLIST_BROWSE_FILE, theApp.m_icon_set.folder_explore.GetIcon(true));
        menu.APPEND_ITEM(ID_PLAYLIST_FIX_PATH_ERROR, theApp.m_icon_set.fix);
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_NEW_PLAYLIST, theApp.m_icon_set.add.GetIcon(true));
        break;
    case MenuMgr::LibLeftMenu:
        menu.CreateMenu(true, false);
        menu.APPEND_ITEM(ID_PLAY_ITEM, theApp.m_icon_set.play_new.GetIcon(true));
        menu.SetDefaultItem();
        menu.AppendSubMenu(AddToPlaylistMenu, theApp.m_icon_set.add.GetIcon(true));
        menu.APPEND_ITEM(ID_ADD_TO_NEW_PLAYLIST_AND_PLAY, theApp.m_icon_set.play_in_playlist);
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_COPY_TEXT, theApp.m_icon_set.copy);
        break;
    case MenuMgr::LibRightMenu:
        menu.CreateMenu(true, false);
        menu.APPEND_ITEM(ID_PLAY_ITEM, theApp.m_icon_set.play_new.GetIcon(true));
        menu.SetDefaultItem();
        menu.APPEND_ITEM(ID_PLAY_AS_NEXT, theApp.m_icon_set.play_as_next);
        menu.APPEND_ITEM(ID_PLAY_ITEM_IN_FOLDER_MODE, theApp.m_icon_set.play_in_folder);
        menu.AppendSeparator();
        menu.AppendSubMenu(AddToPlaylistMenu, theApp.m_icon_set.add.GetIcon(true));
        menu.APPEND_ITEM(ID_ADD_TO_NEW_PLAYLIST_AND_PLAY, theApp.m_icon_set.play_in_playlist);
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_EXPLORE_ONLINE, theApp.m_icon_set.online);
        menu.APPEND_ITEM(ID_FORMAT_CONVERT, theApp.m_icon_set.convert);
        menu.APPEND_ITEM(ID_EXPLORE_TRACK, theApp.m_icon_set.folder_explore.GetIcon(true));
        menu.APPEND_ITEM(ID_DELETE_FROM_DISK, theApp.m_icon_set.close.GetIcon(true));
        menu.APPEND_ITEM(ID_ITEM_PROPERTY, theApp.m_icon_set.info.GetIcon(true));
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_COPY_TEXT, theApp.m_icon_set.copy);
        break;
    case MenuMgr::LeMenu:
        menu.CreateMenu(false, false);
        menu.AppendSubMenu(LeFileMenu, NULL);
        menu.AppendSubMenu(LeEditMenu, NULL);
        menu.AppendSubMenu(LePlayCtrlMenu, NULL);
        break;
    case MenuMgr::LeFileMenu:
        menu.CreateMenu(true, false);
        menu.APPEND_ITEM(ID_LYRIC_OPEN, theApp.m_icon_set.music);
        menu.APPEND_ITEM(ID_LYRIC_SAVE, theApp.m_icon_set.save_new, L"Ctrl+S");
        menu.APPEND_ITEM(ID_LYRIC_SAVE_AS, theApp.m_icon_set.save_as);
        menu.AppendSeparator();
        menu.APPEND_ITEM(IDCANCEL, theApp.m_icon_set.exit);
        break;
    case MenuMgr::LeEditMenu:
        menu.CreateMenu(true, false);
        menu.APPEND_ITEM(ID_LYRIC_INSERT_TAG, theApp.m_icon_set.le_add_tag, L"F8");
        menu.APPEND_ITEM(ID_LYRIC_REPLACE_TAG, theApp.m_icon_set.le_replace_tag, L"F9");
        menu.APPEND_ITEM(ID_LYRIC_DELETE_TAG, theApp.m_icon_set.le_delete_tag, L"Ctrl+Del");
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_LYRIC_FIND, theApp.m_icon_set.le_find, L"Ctrl+F");
        menu.APPEND_ITEM(ID_LYRIC_REPLACE, theApp.m_icon_set.le_replace, L"Ctrl+H");
        menu.APPEND_ITEM(ID_FIND_NEXT, NULL, L"F3");
        menu.AppendSeparator();
        menu.AppendSubMenu(LeEditChConvMenu, NULL);
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_LRYIC_MERGE_SAME_TIME_TAG, NULL);
        menu.APPEND_ITEM(ID_LYRIC_SWAP_TEXT_AND_TRANSLATION, NULL);
        menu.AppendSubMenu(LeEditTagDislocMenu, NULL);
        break;
    case MenuMgr::LeEditChConvMenu:
        menu.CreateMenu(true, false);
        menu.APPEND_ITEM(ID_LE_TRANSLATE_TO_SIMPLIFIED_CHINESE, NULL);
        menu.APPEND_ITEM(ID_LE_TRANSLATE_TO_TRANDITIONAL_CHINESE, NULL);
        break;
    case MenuMgr::LeEditTagDislocMenu:
        menu.CreateMenu(true, false);
        menu.APPEND_ITEM(ID_LYRIC_TIME_TAG_FORWARD, NULL);
        menu.APPEND_ITEM(ID_LYRIC_TIME_TAG_DELAY, NULL);
        break;
    case MenuMgr::LePlayCtrlMenu:
        menu.CreateMenu(true, false);
        menu.APPEND_ITEM(ID_PLAY_PAUSE, theApp.m_icon_set.play_pause, L"Ctrl+P");
        menu.APPEND_ITEM(ID_REW, theApp.m_icon_set.rew_new, CAcceleratorRes::Key(VK_LEFT, true).ToString().c_str());
        menu.APPEND_ITEM(ID_FF, theApp.m_icon_set.ff_new, CAcceleratorRes::Key(VK_RIGHT, true).ToString().c_str());
        menu.APPEND_ITEM(ID_SEEK_TO_CUR_LINE, theApp.m_icon_set.locate.GetIcon(true), L"Ctrl+G");
        break;
    case MenuMgr::FcListMenu:
        menu.CreateMenu(true, false);
        menu.APPEND_ITEM(ID_ADD_FILE, theApp.m_icon_set.add.GetIcon(true));
        menu.APPEND_ITEM(ID_CLEAR_LIST, NULL);
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_EDIT_TAG_INFO, theApp.m_icon_set.edit.GetIcon(true));
        menu.APPEND_ITEM(ID_DELETE_SELECT, theApp.m_icon_set.close.GetIcon(true));
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_MOVE_UP, NULL, CAcceleratorRes::Key(VK_UP, true).ToString().c_str());
        menu.APPEND_ITEM(ID_MOVE_DOWN, NULL, CAcceleratorRes::Key(VK_DOWN, true).ToString().c_str());
        break;
    case MenuMgr::LdListMenu:
        menu.CreateMenu(true, false);
        menu.APPEND_ITEM(ID_LD_LYRIC_DOWNLOAD, theApp.m_icon_set.download);
        menu.SetDefaultItem();
        menu.APPEND_ITEM(ID_LD_PREVIEW, NULL);
        menu.APPEND_ITEM(ID_LD_LYRIC_SAVEAS, theApp.m_icon_set.save_as);
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_LD_COPY_TITLE, NULL);
        menu.APPEND_ITEM(ID_LD_COPY_ARTIST, NULL);
        menu.APPEND_ITEM(ID_LD_COPY_ALBUM, NULL);
        menu.APPEND_ITEM(ID_LD_COPY_ID, NULL);
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_LD_RELATE, NULL);
        menu.APPEND_ITEM(ID_LD_VIEW_ONLINE, theApp.m_icon_set.online);
        break;
    case MenuMgr::OptDlrcDefStyleMenu:
        menu.CreateMenu(true, false);
        menu.APPEND_ITEM(ID_LYRIC_DEFAULT_STYLE1, NULL);
        menu.APPEND_ITEM(ID_LYRIC_DEFAULT_STYLE2, NULL);
        menu.APPEND_ITEM(ID_LYRIC_DEFAULT_STYLE3, NULL);
        menu.AppendSeparator();
        menu.AppendSubMenu(OptDlrcModDefStyleMenu, NULL);
        break;
    case MenuMgr::OptDlrcModDefStyleMenu:
        menu.CreateMenu(true, false);
        menu.APPEND_ITEM(ID_LYRIC_DEFAULT_STYLE1_MODIFY, NULL);
        menu.APPEND_ITEM(ID_LYRIC_DEFAULT_STYLE2_MODIFY, NULL);
        menu.APPEND_ITEM(ID_LYRIC_DEFAULT_STYLE3_MODIFY, NULL);
        menu.AppendSeparator();
        menu.APPEND_ITEM(ID_RESTORE_DEFAULT_STYLE, NULL);
        break;
    case MenuMgr::PropertyAdvMenu:
        menu.CreateMenu(true, false);
        menu.APPEND_ITEM(ID_COPY_TEXT, NULL);
        menu.APPEND_ITEM(ID_COPY_ALL_TEXT, NULL);
        break;
    case MenuMgr::PropertyCoverMenu:
        menu.CreateMenu(true, false);
        menu.APPEND_ITEM(ID_COVER_BROWSE, theApp.m_icon_set.folder_explore.GetIcon(true));
        menu.APPEND_ITEM(ID_COVER_DELETE, theApp.m_icon_set.close.GetIcon(true));
        menu.APPEND_ITEM(ID_COVER_SAVE_AS, theApp.m_icon_set.save_as);
        menu.APPEND_ITEM(ID_COMPRESS_SIZE, NULL);
        break;
    default:
        ASSERT(false);                  // 参数错误或缺少case或缺少break
        menu.CreateMenu(true, true);    // 创建一个空的弹出菜单占位
        break;
    }
}
