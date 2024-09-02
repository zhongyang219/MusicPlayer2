#include "stdafx.h"
#include "MusicPlayer2.h"
#include "MenuMgr.h"
#include "WinVersionHelper.h"


class MenuMgr::MenuBase
{
public:
    MenuBase(MenuMgr* pMenuMgr, MenuMgr::MenuType menu_type)
        : m_pMenuMgr(pMenuMgr), m_menu_type(menu_type) {}
    ~MenuBase() {}

    void CreateMenu(bool is_popup, bool add_accelerator)
    {
        m_add_accelerator = add_accelerator;
        if (is_popup)
            m_menu.CreatePopupMenu();
        else
            m_menu.CreateMenu();
    }

    void AppendItem(UINT wID, const wstring& id_text, IconMgr::IconType icon_type = IconMgr::IT_NO_ICON, const wchar_t* text = nullptr)
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
        if (HBITMAP hBmp = m_pMenuMgr->GetMenuBitmapHandle(icon_type))
        {
            mii.fMask |= MIIM_BITMAP;
            mii.hbmpItem = hBmp;
        }
#endif
        m_menu.InsertMenuItemW(m_end_pos++, &mii, TRUE);
    }

    void AppendSubMenu(MenuMgr::MenuType sub_menu_type, IconMgr::IconType icon_type = IconMgr::IT_NO_ICON)
    {
        MENUITEMINFO mii = { sizeof(mii) };
        mii.fMask = MIIM_STRING | MIIM_SUBMENU | MIIM_FTYPE;
        mii.hSubMenu = m_pMenuMgr->GetSafeHmenu(sub_menu_type);
        wstring menu_text = theApp.m_str_table.LoadMenuText(MenuMgr::GetMenuNameStr(m_menu_type), MenuMgr::GetMenuNameStr(sub_menu_type));
        mii.dwTypeData = const_cast<LPWSTR>(menu_text.c_str());
#ifndef COMPILE_IN_WIN_XP
        if (HBITMAP hBmp = m_pMenuMgr->GetMenuBitmapHandle(icon_type))
        {
            mii.fMask |= MIIM_BITMAP;
            mii.hbmpItem = hBmp;
        }
#endif
        m_menu.InsertMenuItemW(m_end_pos++, &mii, TRUE);
    }

    void AppendSeparator()
    {
        m_menu.AppendMenuW(MF_SEPARATOR);
        m_end_pos++;
    }

    void SetDefaultItem()
    {
        ASSERT(m_end_pos > 0);
        m_menu.SetDefaultItem(m_end_pos - 1, TRUE);
    }

    void UpdateMenu(const vector<MenuMgr::MenuItem>& items)
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
                AppendItem(item.id, empty, item.icon_type, item.text.c_str());
            else
                AppendSeparator();
        }
        m_end_pos = bk_org_cnt;             // 恢复为初始项目数
        // 这里有一点小问题，如果不销毁重建菜单句柄那么菜单会记忆其所需过的文本宽度，也就是不会变窄（至少我不知道怎样做）
        // 销毁重建会使得这里的共享子菜单机制不能正常工作
    }

    MenuMgr::MenuType GetMenuType() const { return m_menu_type; }
    void SetUpdatePos() { m_update_pos = m_end_pos; }

    CMenu m_menu;
private:
    MenuMgr* m_pMenuMgr;
    MenuMgr::MenuType m_menu_type;
    int m_end_pos{ 0 };
    int m_update_pos{ -1 };
    bool m_add_accelerator{ false };                // AppendItem是否自动根据命令ID附加加速键列表中的快捷键文本
};

MenuMgr::MenuMgr()
{
}

MenuMgr::~MenuMgr()
{
}

typedef DWORD ARGB;

HBITMAP MenuMgr::GetMenuBitmapHandle(IconMgr::IconType icon_type)
{
    if (auto pBitmap = GetMenuBitmap(icon_type))
        return static_cast<HBITMAP>(pBitmap->GetSafeHandle());
    return NULL;
}

const CBitmap* MenuMgr::GetMenuBitmap(IconMgr::IconType icon_type)
{
    if (icon_type == IconMgr::IconType::IT_NO_ICON)
        return nullptr;

    HICON hIcon = theApp.m_icon_mgr.GetHICON(icon_type, IconMgr::IS_OutlinedDark, IconMgr::IconSize::IS_DPI_16);
    // 取得默认图标尺寸（应该与GetSystemMetrics(SM_CXSMICON/SM_CYSMICON)相同）
    auto [width, height] = IconMgr::GetIconSize(IconMgr::IconSize::IS_DPI_16);

    if (m_icon_bitmap_map[hIcon].GetSafeHandle())  // 已有bitmap时不再二次转换
        return &m_icon_bitmap_map[hIcon];

    // 创建一个带Alpha通道的DIB节
    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;       // 使用负高度以保证DIB的顶向下的排列
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;          // 每像素32位，带Alpha通道
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pvBits;
    HBITMAP hbm = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0);
    HDC hdcMem = CreateCompatibleDC(NULL);
    HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbm);
    // 用透明背景填充位图
    BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    AlphaBlend(hdcMem, 0, 0, width, height, hdcMem, 0, 0, width, height, bf);
    // 将图标绘制到位图上
    DrawIconEx(hdcMem, 0, 0, hIcon, width, height, 0, NULL, DI_NORMAL);
    // 清理
    SelectObject(hdcMem, hbmOld);
    DeleteDC(hdcMem);

    m_icon_bitmap_map[hIcon].Attach(hbm);

    return &m_icon_bitmap_map[hIcon];
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

const wchar_t* MenuMgr::GetMenuNameStr(MenuMgr::MenuType menu_type)
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
    case MenuMgr::LibLeftMenu: case MenuMgr::UiLibLeftMenu: case MenuMgr::LibFolderExploreMenu:
        return L"LIB_LEFT";
    case MenuMgr::LibRightMenu: case MenuMgr::UiMyFavouriteMenu: case MenuMgr::LibPlaylistRightMenu:
        return L"LIB_RIGHT";
    case MenuMgr::UiRecentPlayedMenu:
        return L"UI_RECENT_PLAYED";
    case MenuMgr::LibPlaylistSortMenu:
        return L"LIB_PLAYLIST_SORT";
    case MenuMgr::LibFolderSortMenu:
        return L"LIB_FOLDER_SORT";
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
    case MenuMgr::LeEditTranslateFormatConvertMenu:
        return L"LE_EDIT_TRANSLATE_FORMAT_CONVERT";
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

#define EX_ID(id) id, L#id

void MenuMgr::CreateMenu(MenuBase& menu)
{
    // git log 47698b win11下使用复选功能的菜单项不设置图标
    static bool is_win11OrLater = CWinVersionHelper::IsWindows11OrLater();
    switch (menu.GetMenuType())
    {
    case MenuMgr::MainPopupMenu:
        menu.CreateMenu(true, true);
        menu.AppendSubMenu(MainFileMenu, IconMgr::IconType::IT_Folder);
        menu.AppendSubMenu(MainPlayCtrlMenu, IconMgr::IconType::IT_Play);
        menu.AppendSubMenu(MainPlaylistMenu, IconMgr::IconType::IT_Playlist);
        menu.AppendSubMenu(MainLyricMenu, IconMgr::IconType::IT_Lyric);
        menu.AppendSubMenu(MainViewMenu, IconMgr::IconType::IT_Playlist_Dock);
        menu.AppendSubMenu(MainToolMenu, IconMgr::IconType::IT_Setting);
        menu.AppendSubMenu(MainHelpMenu, IconMgr::IconType::IT_Help);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_MENU_EXIT), IconMgr::IconType::IT_Exit);
#ifdef _DEBUG
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_TEST));
        menu.AppendItem(EX_ID(ID_TEST_DIALOG));
#endif
        break;
    case MenuMgr::MainMenu:
        menu.CreateMenu(false, true);
        menu.AppendSubMenu(MainFileMenu);
        menu.AppendSubMenu(MainPlayCtrlMenu);
        menu.AppendSubMenu(MainPlaylistMenu);
        menu.AppendSubMenu(MainLyricMenu);
        menu.AppendSubMenu(MainViewMenu);
        menu.AppendSubMenu(MainToolMenu);
        menu.AppendSubMenu(MainHelpMenu);
        break;
    case MenuMgr::MainFileMenu:
        menu.CreateMenu(true, true);
        menu.AppendItem(EX_ID(ID_FILE_OPEN), IconMgr::IconType::IT_Music);
        menu.AppendItem(EX_ID(ID_FILE_OPEN_FOLDER), IconMgr::IconType::IT_Folder);
        menu.AppendItem(EX_ID(ID_FILE_OPEN_URL), IconMgr::IconType::IT_Link);
        menu.AppendItem(EX_ID(ID_FILE_OPEN_PLAYLIST), IconMgr::IconType::IT_Playlist);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_MENU_EXIT), IconMgr::IconType::IT_Exit);
        break;
    case MenuMgr::MainPlayCtrlMenu:
        menu.CreateMenu(true, true);
        menu.AppendItem(EX_ID(ID_PLAY_PAUSE), IconMgr::IconType::IT_Play_Pause);
        menu.AppendItem(EX_ID(ID_STOP), IconMgr::IconType::IT_Stop);
        menu.AppendItem(EX_ID(ID_PREVIOUS), IconMgr::IconType::IT_Previous);
        menu.AppendItem(EX_ID(ID_NEXT), IconMgr::IconType::IT_Next);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_REW), IconMgr::IconType::IT_Rewind);
        menu.AppendItem(EX_ID(ID_FF), IconMgr::IconType::IT_Fast_Forward);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_SPEED_UP), IconMgr::IconType::IT_Speed_Up);
        menu.AppendItem(EX_ID(ID_SLOW_DOWN), IconMgr::IconType::IT_Slow_Down);
        menu.AppendItem(EX_ID(ID_ORIGINAL_SPEED));
        menu.AppendSeparator();
        menu.AppendSubMenu(MainPlayCtrlRepeatModeMenu);
        menu.AppendSubMenu(MainPlayCtrlAbRepeatMenu);
        break;
    case MenuMgr::MainPlayCtrlRepeatModeMenu:
        menu.CreateMenu(true, false);
        menu.AppendItem(EX_ID(ID_PLAY_ORDER), IconMgr::IconType::IT_Play_Order);
        menu.AppendItem(EX_ID(ID_PLAY_SHUFFLE), IconMgr::IconType::IT_Play_Shuffle);
        menu.AppendItem(EX_ID(ID_PLAY_RANDOM), IconMgr::IconType::IT_Play_Random);
        menu.AppendItem(EX_ID(ID_LOOP_PLAYLIST), IconMgr::IconType::IT_Loop_Playlist);
        menu.AppendItem(EX_ID(ID_LOOP_TRACK), IconMgr::IconType::IT_Loop_Track);
        menu.AppendItem(EX_ID(ID_PLAY_TRACK), IconMgr::IconType::IT_Play_Track);
        break;
    case MenuMgr::MainPlayCtrlAbRepeatMenu:
        menu.CreateMenu(true, true);
        menu.AppendItem(EX_ID(ID_AB_REPEAT));
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_SET_A_POINT));
        menu.AppendItem(EX_ID(ID_SET_B_POINT));
        menu.AppendItem(EX_ID(ID_NEXT_AB_REPEAT));
        menu.AppendItem(EX_ID(ID_RESET_AB_REPEAT));
        break;
    case MenuMgr::MainPlaylistMenu:
        menu.CreateMenu(true, true);
        menu.AppendSubMenu(MainPlaylistAddMenu, IconMgr::IconType::IT_Add);
        menu.AppendSubMenu(MainPlaylistDelMenu, IconMgr::IconType::IT_Cancel);
        menu.AppendItem(EX_ID(ID_RELOAD_PLAYLIST), IconMgr::IconType::IT_Loop_Playlist);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_SAVE_AS_NEW_PLAYLIST), IconMgr::IconType::IT_Save);
        menu.AppendItem(EX_ID(ID_SAVE_CURRENT_PLAYLIST_AS), IconMgr::IconType::IT_Save_As);
        menu.AppendSeparator();
        menu.AppendSubMenu(MainPlaylistSortMenu, IconMgr::IconType::IT_Sort_Mode);
        menu.AppendSubMenu(MainPlaylistDispModeMenu, IconMgr::IconType::IT_Playlist_Display_Mode);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_LOCATE_TO_CURRENT), IconMgr::IconType::IT_Locate);
        break;
    case MenuMgr::MainPlaylistAddMenu:
        menu.CreateMenu(true, true);
        menu.AppendItem(EX_ID(ID_PLAYLIST_ADD_FILE), IconMgr::IconType::IT_Music);
        menu.AppendItem(EX_ID(ID_PLAYLIST_ADD_FOLDER), IconMgr::IconType::IT_Folder);
        menu.AppendItem(EX_ID(ID_PLAYLIST_ADD_URL), IconMgr::IconType::IT_Link);
        break;
    case MenuMgr::MainPlaylistDelMenu:
        menu.CreateMenu(true, true);
        menu.AppendItem(EX_ID(ID_REMOVE_FROM_PLAYLIST), IconMgr::IconType::IT_Cancel);
        menu.AppendItem(EX_ID(ID_DELETE_FROM_DISK));
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_EMPTY_PLAYLIST));
        menu.AppendItem(EX_ID(ID_REMOVE_SAME_SONGS));
        menu.AppendItem(EX_ID(ID_REMOVE_INVALID_ITEMS));
        break;
    case MenuMgr::MainPlaylistSortMenu:
        menu.CreateMenu(true, true);
        menu.AppendItem(EX_ID(ID_SORT_BY_FILE));
        menu.AppendItem(EX_ID(ID_SORT_BY_PATH));
        menu.AppendItem(EX_ID(ID_SORT_BY_TITLE));
        menu.AppendItem(EX_ID(ID_SORT_BY_ARTIST));
        menu.AppendItem(EX_ID(ID_SORT_BY_ALBUM));
        menu.AppendItem(EX_ID(ID_SORT_BY_TRACK));
        menu.AppendItem(EX_ID(ID_SORT_BY_LISTEN_TIME));
        menu.AppendItem(EX_ID(ID_SORT_BY_MODIFIED_TIME));
        break;
    case MenuMgr::MainPlaylistDispModeMenu:
        menu.CreateMenu(true, true);
        menu.AppendItem(EX_ID(ID_DISP_FILE_NAME));
        menu.AppendItem(EX_ID(ID_DISP_TITLE));
        menu.AppendItem(EX_ID(ID_DISP_ARTIST_TITLE));
        menu.AppendItem(EX_ID(ID_DISP_TITLE_ARTIST));
        break;
    case MenuMgr::MainLyricMenu:
        menu.CreateMenu(true, true);
        menu.AppendItem(EX_ID(ID_RELOAD_LYRIC), IconMgr::IconType::IT_Loop_Playlist);
        menu.AppendItem(EX_ID(ID_COPY_CURRENT_LYRIC), IconMgr::IconType::IT_Copy);
        menu.AppendItem(EX_ID(ID_COPY_ALL_LYRIC));
        menu.AppendItem(EX_ID(ID_EDIT_LYRIC), IconMgr::IconType::IT_Edit);
        menu.AppendSubMenu(MainLrcChConvMenu);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_SHOW_LYRIC_TRANSLATE));
        menu.AppendItem(EX_ID(ID_SHOW_DESKTOP_LYRIC));
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_LYRIC_FORWARD), IconMgr::IconType::IT_Triangle_Left);
        menu.AppendItem(EX_ID(ID_LYRIC_DELAY), IconMgr::IconType::IT_Triangle_Right);
        menu.AppendItem(EX_ID(ID_SAVE_MODIFIED_LYRIC), IconMgr::IconType::IT_Save);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_RELATE_LOCAL_LYRIC), IconMgr::IconType::IT_Lyric);
        menu.AppendItem(EX_ID(ID_DELETE_LYRIC), IconMgr::IconType::IT_Cancel);
        menu.AppendItem(EX_ID(ID_UNLINK_LYRIC), IconMgr::IconType::IT_Unlink);
        menu.AppendItem(EX_ID(ID_BROWSE_LYRIC), IconMgr::IconType::IT_Folder_Explore);
        menu.AppendSubMenu(MainLrcInnerLrcMenu, IconMgr::IconType::IT_Internal_Lyric);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_DOWNLOAD_LYRIC), IconMgr::IconType::IT_Download);
        menu.AppendItem(EX_ID(ID_LYRIC_BATCH_DOWNLOAD), IconMgr::IconType::IT_Download_Batch);
        break;
    case MenuMgr::MainLrcChConvMenu:
        menu.CreateMenu(true, true);
        menu.AppendItem(EX_ID(ID_TRANSLATE_TO_SIMPLIFIED_CHINESE));
        menu.AppendItem(EX_ID(ID_TRANSLATE_TO_TRANDITIONAL_CHINESE));
        break;
    case MenuMgr::MainLrcInnerLrcMenu:
        menu.CreateMenu(true, true);
        menu.AppendItem(EX_ID(ID_EMBED_LYRIC_TO_AUDIO_FILE));
        menu.AppendItem(EX_ID(ID_DELETE_LYRIC_FROM_AUDIO_FILE));
        break;
    case MenuMgr::MainViewMenu:
        menu.CreateMenu(true, true);
        menu.AppendItem(EX_ID(ID_SHOW_PLAYLIST), is_win11OrLater ? IconMgr::IconType::IT_NO_ICON : IconMgr::IconType::IT_Playlist_Dock);
        menu.AppendItem(EX_ID(ID_FLOAT_PLAYLIST), is_win11OrLater ? IconMgr::IconType::IT_NO_ICON : IconMgr::IconType::IT_Playlist_Float);
        menu.AppendItem(EX_ID(ID_USE_STANDARD_TITLE_BAR));
        menu.AppendItem(EX_ID(ID_SHOW_MENU_BAR), is_win11OrLater ? IconMgr::IconType::IT_NO_ICON : IconMgr::IconType::IT_Menu);
        menu.AppendItem(EX_ID(ID_ALWAYS_SHOW_STATUS_BAR));
        menu.AppendItem(EX_ID(ID_ALWAYS_ON_TOP), is_win11OrLater ? IconMgr::IconType::IT_NO_ICON : IconMgr::IconType::IT_Pin);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_MINI_MODE), IconMgr::IconType::IT_Mini_On);
        menu.AppendItem(EX_ID(ID_FULL_SCREEN), IconMgr::IconType::IT_Full_Screen_On);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_DARK_MODE), IconMgr::IconType::IT_Dark_Mode_On);
        menu.AppendSubMenu(MainViewSwitchUiMenu, IconMgr::IconType::IT_Skin);
        break;
    case MenuMgr::MainViewSwitchUiMenu:
        menu.CreateMenu(true, true);
        menu.AppendItem(EX_ID(ID_SWITCH_UI), IconMgr::IconType::IT_Skin);
        menu.AppendSeparator();
        menu.SetUpdatePos();            // 设置Update插入点
        break;
    case MenuMgr::MainToolMenu:
        menu.CreateMenu(true, true);
        menu.AppendItem(EX_ID(ID_MEDIA_LIB), IconMgr::IconType::IT_Media_Lib);
        menu.AppendItem(EX_ID(ID_FIND), IconMgr::IconType::IT_Find);
        menu.AppendItem(EX_ID(ID_EXPLORE_PATH), IconMgr::IconType::IT_Folder_Explore);
        menu.AppendItem(EX_ID(ID_SONG_INFO), IconMgr::IconType::IT_Info);
        menu.AppendItem(EX_ID(ID_EQUALIZER), IconMgr::IconType::IT_Equalizer);
        menu.AppendItem(EX_ID(ID_FORMAT_CONVERT1), IconMgr::IconType::IT_Convert);
        menu.AppendItem(EX_ID(ID_CURRENT_EXPLORE_ONLINE), IconMgr::IconType::IT_Online);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_RE_INI_BASS), IconMgr::IconType::IT_Refresh);
        menu.AppendSubMenu(MainToolCreateShortcutMenu, IconMgr::IconType::IT_Shortcut);
        menu.AppendSeparator();
        menu.AppendSubMenu(MainToolAlbumCoverMenu, IconMgr::IconType::IT_Album_Cover);
        menu.AppendSubMenu(MainToolDelCurPlayingMenu, IconMgr::IconType::IT_Cancel);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_LISTEN_STATISTICS), IconMgr::IconType::IT_Statistics);
        menu.AppendItem(EX_ID(ID_TOOL_FILE_RELATE), IconMgr::IconType::IT_File_Relate);
        menu.AppendItem(EX_ID(ID_OPTION_SETTINGS), IconMgr::IconType::IT_Setting);
        break;
    case MenuMgr::MainToolCreateShortcutMenu:
        menu.CreateMenu(true, true);
        menu.AppendItem(EX_ID(ID_CREATE_DESKTOP_SHORTCUT));
        menu.AppendItem(EX_ID(ID_CREATE_PLAY_SHORTCUT));
        menu.AppendItem(EX_ID(ID_CREATE_MINI_MODE_SHORT_CUT));
        break;
    case MenuMgr::MainToolAlbumCoverMenu:
        menu.CreateMenu(true, true);
        menu.AppendItem(EX_ID(ID_DOWNLOAD_ALBUM_COVER), IconMgr::IconType::IT_Download);
        menu.AppendItem(EX_ID(ID_ALBUM_COVER_SAVE_AS), IconMgr::IconType::IT_Save_As);
        menu.AppendItem(EX_ID(ID_DELETE_ALBUM_COVER), IconMgr::IconType::IT_Cancel);
        menu.AppendItem(EX_ID(ID_ALBUM_COVER_INFO), IconMgr::IconType::IT_Album_Cover);
        break;
    case MenuMgr::MainToolDelCurPlayingMenu:
        menu.CreateMenu(true, true);
        menu.AppendItem(EX_ID(ID_REMOVE_CURRENT_FROM_PLAYLIST));
        menu.AppendItem(EX_ID(ID_DELETE_CURRENT_FROM_DISK));
        break;
    case MenuMgr::MainHelpMenu:
        menu.CreateMenu(true, true);
        menu.AppendItem(EX_ID(ID_HELP), IconMgr::IconType::IT_Help, L"F1");
        menu.AppendItem(EX_ID(ID_ONLINE_HELP));
        menu.AppendItem(EX_ID(ID_HELP_FAQ));
        menu.AppendItem(EX_ID(ID_HELP_CUSTOM_UI));
        menu.AppendItem(EX_ID(ID_HELP_UPDATE_LOG));
        menu.AppendItem(EX_ID(ID_SUPPORTED_FORMAT));
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_APP_ABOUT), IconMgr::IconType::IT_App);
        break;
    case MenuMgr::PlaylistToolBarMenu:
        menu.CreateMenu(true, true);
        menu.AppendSubMenu(MainPlaylistAddMenu, IconMgr::IconType::IT_Add);
        menu.AppendSubMenu(MainPlaylistDelMenu, IconMgr::IconType::IT_Cancel);
        menu.AppendSubMenu(MainPlaylistSortMenu, IconMgr::IconType::IT_Sort_Mode);
        menu.AppendSubMenu(PlaylistToolBarListMenu, IconMgr::IconType::IT_Playlist);
        menu.AppendSubMenu(PlaylistToolBarEditMenu, IconMgr::IconType::IT_Edit);
        menu.AppendSubMenu(PlaylistToolBarFolderMenu, IconMgr::IconType::IT_Folder);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_LOCATE_TO_CURRENT), IconMgr::IconType::IT_Locate);
        break;
    case MenuMgr::PlaylistToolBarListMenu:
        menu.CreateMenu(true, true);
        menu.AppendItem(EX_ID(ID_RELOAD_PLAYLIST), IconMgr::IconType::IT_Loop_Playlist);
        menu.AppendItem(EX_ID(ID_SAVE_AS_NEW_PLAYLIST), IconMgr::IconType::IT_Save);
        menu.AppendItem(EX_ID(ID_SAVE_CURRENT_PLAYLIST_AS), IconMgr::IconType::IT_Save_As);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_PLAYLIST_FIX_PATH_ERROR), IconMgr::IconType::IT_Fix);
        menu.AppendSubMenu(MainPlaylistDispModeMenu, IconMgr::IconType::IT_Playlist_Display_Mode);
        menu.AppendItem(EX_ID(ID_PLAYLIST_OPTIONS), IconMgr::IconType::IT_Setting);
        break;
    case MenuMgr::PlaylistToolBarEditMenu:
        menu.CreateMenu(true, true);
        menu.AppendSubMenu(AddToPlaylistMenu, IconMgr::IconType::IT_Add);
        menu.AppendItem(EX_ID(ID_COPY_FILE_TO));
        menu.AppendItem(EX_ID(ID_MOVE_FILE_TO));
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_MOVE_PLAYLIST_ITEM_UP));
        menu.AppendItem(EX_ID(ID_MOVE_PLAYLIST_ITEM_DOWN));
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_PLAYLIST_SELECT_ALL));
        menu.AppendItem(EX_ID(ID_PLAYLIST_SELECT_NONE));
        menu.AppendItem(EX_ID(ID_PLAYLIST_SELECT_REVERT));
        break;
    case MenuMgr::PlaylistToolBarFolderMenu:
        menu.CreateMenu(true, true);
        menu.AppendItem(EX_ID(ID_FILE_OPEN_FOLDER), IconMgr::IconType::IT_Folder);
        menu.AppendItem(EX_ID(ID_CONTAIN_SUB_FOLDER));
        break;
    case MenuMgr::MainAreaMenu:
        menu.CreateMenu(true, true);
        menu.AppendItem(EX_ID(ID_SONG_INFO), IconMgr::IconType::IT_Info);
        menu.AppendSubMenu(MainPlayCtrlRepeatModeMenu);
        menu.AppendSubMenu(AddToPlaylistMenu, IconMgr::IconType::IT_Add);
        menu.AppendSubMenu(RateMenu, IconMgr::IconType::IT_Star);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_MEDIA_LIB), IconMgr::IconType::IT_Media_Lib);
        menu.AppendItem(EX_ID(ID_FIND), IconMgr::IconType::IT_Find);
        menu.AppendItem(EX_ID(ID_EXPLORE_PATH), IconMgr::IconType::IT_Folder_Explore);
        menu.AppendItem(EX_ID(ID_EQUALIZER), IconMgr::IconType::IT_Equalizer);
        menu.AppendItem(EX_ID(ID_CURRENT_EXPLORE_ONLINE), IconMgr::IconType::IT_Online);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_DOWNLOAD_ALBUM_COVER), IconMgr::IconType::IT_Download);
        menu.AppendItem(EX_ID(ID_ALBUM_COVER_SAVE_AS), IconMgr::IconType::IT_Save_As);
        menu.AppendItem(EX_ID(ID_DELETE_ALBUM_COVER), IconMgr::IconType::IT_Cancel);
        menu.AppendItem(EX_ID(ID_ALWAYS_USE_EXTERNAL_ALBUM_COVER));
        menu.AppendItem(EX_ID(ID_ALBUM_COVER_INFO), IconMgr::IconType::IT_Album_Cover);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_VIEW_ARTIST), IconMgr::IconType::IT_Artist);
        menu.AppendItem(EX_ID(ID_VIEW_ALBUM), IconMgr::IconType::IT_Album);
        menu.AppendSeparator();
        menu.AppendSubMenu(MainViewSwitchUiMenu, IconMgr::IconType::IT_Skin);
        menu.AppendItem(EX_ID(ID_OPTION_SETTINGS), IconMgr::IconType::IT_Setting);
        break;
    case MenuMgr::MainAreaLrcMenu:
        menu.CreateMenu(true, true);
        menu.AppendItem(EX_ID(ID_RELOAD_LYRIC), IconMgr::IconType::IT_Loop_Playlist);
        menu.AppendItem(EX_ID(ID_COPY_CURRENT_LYRIC), IconMgr::IconType::IT_Copy);
        menu.AppendItem(EX_ID(ID_COPY_ALL_LYRIC));
        menu.AppendItem(EX_ID(ID_EDIT_LYRIC), IconMgr::IconType::IT_Edit);
        menu.AppendSubMenu(MainLrcChConvMenu);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_SHOW_LYRIC_TRANSLATE));
        menu.AppendItem(EX_ID(ID_SHOW_DESKTOP_LYRIC));
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_LYRIC_FORWARD), IconMgr::IconType::IT_Triangle_Left);
        menu.AppendItem(EX_ID(ID_LYRIC_DELAY), IconMgr::IconType::IT_Triangle_Right);
        menu.AppendItem(EX_ID(ID_SAVE_MODIFIED_LYRIC), IconMgr::IconType::IT_Save);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_RELATE_LOCAL_LYRIC), IconMgr::IconType::IT_Lyric);
        menu.AppendItem(EX_ID(ID_DELETE_LYRIC), IconMgr::IconType::IT_Cancel);
        menu.AppendItem(EX_ID(ID_UNLINK_LYRIC), IconMgr::IconType::IT_Unlink);
        menu.AppendItem(EX_ID(ID_BROWSE_LYRIC), IconMgr::IconType::IT_Folder_Explore);
        menu.AppendSubMenu(MainLrcInnerLrcMenu, IconMgr::IconType::IT_Internal_Lyric);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_DOWNLOAD_LYRIC), IconMgr::IconType::IT_Download);
        menu.AppendItem(EX_ID(ID_LYRIC_BATCH_DOWNLOAD), IconMgr::IconType::IT_Download_Batch);
        menu.AppendSeparator();
        menu.AppendSubMenu(MainViewSwitchUiMenu, IconMgr::IconType::IT_Skin);
        menu.AppendItem(EX_ID(ID_OPTION_SETTINGS), IconMgr::IconType::IT_Setting);
        break;
    case MenuMgr::MainAreaPlaylistBtnMenu:
        menu.CreateMenu(true, false);
        menu.AppendItem(EX_ID(ID_DOCKED_PLAYLIST));
        menu.AppendItem(EX_ID(ID_FLOATED_PLAYLIST));
        break;
    case MenuMgr::DlrcMenu:
        menu.CreateMenu(true, true);
        menu.AppendItem(EX_ID(ID_PLAY_PAUSE), IconMgr::IconType::IT_Play_Pause);
        menu.AppendItem(EX_ID(ID_STOP), IconMgr::IconType::IT_Stop);
        menu.AppendItem(EX_ID(ID_PREVIOUS), IconMgr::IconType::IT_Previous);
        menu.AppendItem(EX_ID(ID_NEXT), IconMgr::IconType::IT_Next);
        menu.AppendSubMenu(MainPlayCtrlRepeatModeMenu);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_SHOW_MAIN_WINDOW));
        menu.AppendItem(EX_ID(ID_LYRIC_DISPLAYED_DOUBLE_LINE), IconMgr::IconType::IT_Double_Line);
        menu.AppendItem(EX_ID(ID_LYRIC_BACKGROUND_PENETRATE), IconMgr::IconType::IT_Skin);
        menu.AppendItem(EX_ID(ID_LOCK_DESKTOP_LRYIC), IconMgr::IconType::IT_Lock);
        menu.AppendSubMenu(DlrcDefMenu, IconMgr::IconType::IT_Media_Lib);
        menu.AppendItem(EX_ID(ID_OPTION_SETTINGS), IconMgr::IconType::IT_Setting);
        menu.AppendItem(EX_ID(ID_CLOSE_DESKTOP_LYRIC), IconMgr::IconType::IT_Cancel);
        break;
    case MenuMgr::DlrcDefMenu:
        menu.CreateMenu(true, true);
        menu.AppendItem(EX_ID(ID_LYRIC_DEFAULT_STYLE1));
        menu.AppendItem(EX_ID(ID_LYRIC_DEFAULT_STYLE2));
        menu.AppendItem(EX_ID(ID_LYRIC_DEFAULT_STYLE3));
        break;
    case MenuMgr::RecentFolderPlaylistMenu:
        menu.CreateMenu(true, true);
        menu.SetUpdatePos();            // 设置Update插入点
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_MORE_RECENT_ITEMS), IconMgr::IconType::IT_More);
        menu.AppendItem(EX_ID(ID_MEDIA_LIB), IconMgr::IconType::IT_Media_Lib);
        break;
    case MenuMgr::PlaylistMenu:
        menu.CreateMenu(true, true);
        menu.AppendItem(EX_ID(ID_PLAY_ITEM), IconMgr::IconType::IT_Play);
        menu.SetDefaultItem();
        menu.AppendItem(EX_ID(ID_PLAY_AS_NEXT), IconMgr::IconType::IT_Play_As_Next);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_EXPLORE_ONLINE), IconMgr::IconType::IT_Online);
        menu.AppendItem(EX_ID(ID_FORMAT_CONVERT), IconMgr::IconType::IT_Convert);
        menu.AppendSubMenu(RateMenu, IconMgr::IconType::IT_Star);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_RELOAD_PLAYLIST), IconMgr::IconType::IT_Loop_Playlist);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_REMOVE_FROM_PLAYLIST), IconMgr::IconType::IT_Cancel);
        menu.AppendItem(EX_ID(ID_DELETE_FROM_DISK));
        menu.AppendSeparator();
        menu.AppendSubMenu(AddToPlaylistMenu, IconMgr::IconType::IT_Add);
        menu.AppendItem(EX_ID(ID_COPY_FILE_TO));
        menu.AppendItem(EX_ID(ID_MOVE_FILE_TO));
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_MOVE_PLAYLIST_ITEM_UP));
        menu.AppendItem(EX_ID(ID_MOVE_PLAYLIST_ITEM_DOWN));
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_PLAYLIST_VIEW_ARTIST), IconMgr::IconType::IT_Artist);
        menu.AppendItem(EX_ID(ID_PLAYLIST_VIEW_ALBUM), IconMgr::IconType::IT_Album);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_RENAME), IconMgr::IconType::IT_Rename);
        menu.AppendItem(EX_ID(ID_EXPLORE_TRACK), IconMgr::IconType::IT_Folder_Explore);
        menu.AppendItem(EX_ID(ID_ITEM_PROPERTY), IconMgr::IconType::IT_Info);
        break;
    case MenuMgr::RateMenu:
        menu.CreateMenu(true, true);
        menu.AppendItem(EX_ID(ID_RATING_1));
        menu.AppendItem(EX_ID(ID_RATING_2));
        menu.AppendItem(EX_ID(ID_RATING_3));
        menu.AppendItem(EX_ID(ID_RATING_4));
        menu.AppendItem(EX_ID(ID_RATING_5));
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_RATING_NONE));
        break;
    case MenuMgr::AddToPlaylistMenu:
        menu.CreateMenu(true, true);
        menu.AppendItem(EX_ID(ID_ADD_TO_NEW_PLAYLIST), IconMgr::IconType::IT_Add);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_ADD_TO_DEFAULT_PLAYLIST));
        menu.AppendItem(EX_ID(ID_ADD_TO_MY_FAVOURITE), IconMgr::IconType::IT_Favorite_On);
        menu.SetUpdatePos();            // 设置Update插入点
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_ADD_TO_OTHER_PLAYLIST), IconMgr::IconType::IT_Playlist);
        break;
    case MenuMgr::MiniAreaMenu:
        menu.CreateMenu(true, true);
        menu.AppendItem(EX_ID(ID_SONG_INFO), IconMgr::IconType::IT_Info);
        menu.AppendSubMenu(MainPlayCtrlRepeatModeMenu);
        menu.AppendSubMenu(AddToPlaylistMenu, IconMgr::IconType::IT_Add);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_DOWNLOAD_LYRIC), IconMgr::IconType::IT_Download);
        menu.AppendItem(EX_ID(ID_DOWNLOAD_ALBUM_COVER), IconMgr::IconType::IT_Album_Cover);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_ADD_REMOVE_FROM_FAVOURITE), IconMgr::IconType::IT_Favorite_On);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_MEDIA_LIB), IconMgr::IconType::IT_Media_Lib);
        menu.AppendItem(EX_ID(ID_FIND), IconMgr::IconType::IT_Find);
        menu.AppendItem(EX_ID(ID_EXPLORE_PATH), IconMgr::IconType::IT_Folder_Explore);
        menu.AppendItem(EX_ID(ID_EQUALIZER), IconMgr::IconType::IT_Equalizer);
        menu.AppendItem(EX_ID(ID_OPTION_SETTINGS), IconMgr::IconType::IT_Setting);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_SHOW_PLAY_LIST), is_win11OrLater ? IconMgr::IconType::IT_NO_ICON : IconMgr::IconType::IT_Playlist, L"Ctrl+L");
        menu.AppendItem(EX_ID(ID_MINI_MODE_ALWAYS_ON_TOP), is_win11OrLater ? IconMgr::IconType::IT_NO_ICON : IconMgr::IconType::IT_Pin);
        menu.AppendSubMenu(MiniModeSwitchUiMenu, IconMgr::IconType::IT_Skin);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_MINI_MIDE_MINIMIZE), IconMgr::IconType::IT_Minimize);
        menu.AppendItem(EX_ID(IDOK), IconMgr::IconType::IT_Mini_Off, L"Esc");
        menu.AppendItem(EX_ID(ID_MINI_MODE_EXIT), IconMgr::IconType::IT_Exit, L"Ctrl+X");
        break;
    case MenuMgr::MiniModeSwitchUiMenu:
        menu.CreateMenu(true, true);
        menu.AppendItem(EX_ID(ID_MINIMODE_UI_DEFAULT));
        menu.SetUpdatePos();            // 设置Update插入点
        break;
    case MenuMgr::NotifyMenu:
        menu.CreateMenu(true, false);
        menu.AppendItem(EX_ID(ID_PLAY_PAUSE), IconMgr::IconType::IT_Play_Pause);
        menu.AppendItem(EX_ID(ID_PREVIOUS), IconMgr::IconType::IT_Previous);
        menu.AppendItem(EX_ID(ID_NEXT), IconMgr::IconType::IT_Next);
        menu.AppendSubMenu(MainPlayCtrlRepeatModeMenu);
        menu.AppendItem(EX_ID(ID_MINIMODE_RESTORE), IconMgr::IconType::IT_Mini_Off);
        menu.AppendItem(EX_ID(ID_SHOW_DESKTOP_LYRIC), IconMgr::IconType::IT_Lyric);
        menu.AppendItem(EX_ID(ID_LOCK_DESKTOP_LRYIC), IconMgr::IconType::IT_Lock);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_OPTION_SETTINGS), IconMgr::IconType::IT_Setting);
        menu.AppendItem(EX_ID(ID_MENU_EXIT), IconMgr::IconType::IT_Exit);
        break;
    case MenuMgr::LibSetPathMenu:
        menu.CreateMenu(true, false);
        menu.AppendItem(EX_ID(ID_PLAY_PATH), IconMgr::IconType::IT_Play);
        menu.SetDefaultItem();
        menu.AppendItem(EX_ID(ID_DELETE_PATH), IconMgr::IconType::IT_Cancel);
        menu.AppendItem(EX_ID(ID_BROWSE_PATH), IconMgr::IconType::IT_Folder_Explore);
        menu.AppendItem(EX_ID(ID_CONTAIN_SUB_FOLDER));
        menu.AppendSubMenu(AddToPlaylistMenu, IconMgr::IconType::IT_Add);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_CLEAR_INVALID_PATH));
        menu.AppendItem(EX_ID(ID_FILE_OPEN_FOLDER), IconMgr::IconType::IT_NewFolder);
        menu.AppendItem(EX_ID(ID_LIB_FOLDER_PROPERTIES), IconMgr::IconType::IT_Info);
        break;
    case MenuMgr::LibPlaylistMenu:
        menu.CreateMenu(true, false);
        menu.AppendItem(EX_ID(ID_PLAY_PLAYLIST), IconMgr::IconType::IT_Play);
        menu.SetDefaultItem();
        menu.AppendItem(EX_ID(ID_RENAME_PLAYLIST), IconMgr::IconType::IT_Rename);
        menu.AppendItem(EX_ID(ID_DELETE_PLAYLIST), IconMgr::IconType::IT_Cancel);
        menu.AppendItem(EX_ID(ID_SAVE_AS_NEW_PLAYLIST), IconMgr::IconType::IT_Save);
        menu.AppendItem(EX_ID(ID_PLAYLIST_SAVE_AS), IconMgr::IconType::IT_Save_As);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_PLAYLIST_BROWSE_FILE), IconMgr::IconType::IT_Folder_Explore);
        menu.AppendItem(EX_ID(ID_PLAYLIST_FIX_PATH_ERROR), IconMgr::IconType::IT_Fix);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_NEW_PLAYLIST), IconMgr::IconType::IT_Add);
        menu.AppendItem(EX_ID(ID_LIB_PLAYLIST_PROPERTIES), IconMgr::IconType::IT_Info);
        break;
    case MenuMgr::LibLeftMenu:
        menu.CreateMenu(true, false);
        menu.AppendItem(EX_ID(ID_PLAY_ITEM), IconMgr::IconType::IT_Play);
        menu.SetDefaultItem();
        menu.AppendSubMenu(AddToPlaylistMenu, IconMgr::IconType::IT_Add);
        menu.AppendItem(EX_ID(ID_ADD_TO_NEW_PLAYLIST_AND_PLAY), IconMgr::IconType::IT_Play_In_Playlist);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_COPY_TEXT), IconMgr::IconType::IT_Copy);
        break;
    case MenuMgr::LibRightMenu:
        menu.CreateMenu(true, false);
        menu.AppendItem(EX_ID(ID_PLAY_ITEM), IconMgr::IconType::IT_Play);
        menu.SetDefaultItem();
        menu.AppendItem(EX_ID(ID_PLAY_AS_NEXT), IconMgr::IconType::IT_Play_As_Next);
        menu.AppendItem(EX_ID(ID_PLAY_ITEM_IN_FOLDER_MODE), IconMgr::IconType::IT_Play_In_Folder);
        menu.AppendSeparator();
        menu.AppendSubMenu(AddToPlaylistMenu, IconMgr::IconType::IT_Add);
        menu.AppendItem(EX_ID(ID_ADD_TO_NEW_PLAYLIST_AND_PLAY), IconMgr::IconType::IT_Play_In_Playlist);
        menu.AppendSubMenu(RateMenu, IconMgr::IconType::IT_Star);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_EXPLORE_ONLINE), IconMgr::IconType::IT_Online);
        menu.AppendItem(EX_ID(ID_FORMAT_CONVERT), IconMgr::IconType::IT_Convert);
        menu.AppendItem(EX_ID(ID_EXPLORE_TRACK), IconMgr::IconType::IT_Folder_Explore);
        menu.AppendItem(EX_ID(ID_DELETE_FROM_DISK), IconMgr::IconType::IT_Cancel);
        menu.AppendItem(EX_ID(ID_ITEM_PROPERTY), IconMgr::IconType::IT_Info);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_COPY_TEXT), IconMgr::IconType::IT_Copy);
        break;
    case MenuMgr::LibPlaylistRightMenu:
        menu.CreateMenu(true, false);
        menu.AppendItem(EX_ID(ID_PLAY_ITEM), IconMgr::IconType::IT_Play);
        menu.SetDefaultItem();
        menu.AppendItem(EX_ID(ID_PLAY_AS_NEXT), IconMgr::IconType::IT_Play_As_Next);
        menu.AppendItem(EX_ID(ID_PLAY_ITEM_IN_FOLDER_MODE), IconMgr::IconType::IT_Play_In_Folder);
        menu.AppendSeparator();
        menu.AppendSubMenu(AddToPlaylistMenu, IconMgr::IconType::IT_Add);
        menu.AppendItem(EX_ID(ID_ADD_TO_NEW_PLAYLIST_AND_PLAY), IconMgr::IconType::IT_Play_In_Playlist);
        menu.AppendSubMenu(RateMenu, IconMgr::IconType::IT_Star);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_EXPLORE_ONLINE), IconMgr::IconType::IT_Online);
        menu.AppendItem(EX_ID(ID_FORMAT_CONVERT), IconMgr::IconType::IT_Convert);
        menu.AppendItem(EX_ID(ID_EXPLORE_TRACK), IconMgr::IconType::IT_Folder_Explore);
        menu.AppendItem(EX_ID(ID_REMOVE_FROM_PLAYLIST), IconMgr::IconType::IT_Cancel);
        menu.AppendItem(EX_ID(ID_DELETE_FROM_DISK));
        menu.AppendItem(EX_ID(ID_ITEM_PROPERTY), IconMgr::IconType::IT_Info);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_COPY_TEXT), IconMgr::IconType::IT_Copy);
        break;
    case MenuMgr::LibFolderExploreMenu:
        menu.CreateMenu(true, false);
        menu.AppendItem(EX_ID(ID_PLAY_ITEM), IconMgr::IconType::IT_Play);
        menu.SetDefaultItem();
        menu.AppendSubMenu(AddToPlaylistMenu, IconMgr::IconType::IT_Add);
        menu.AppendItem(EX_ID(ID_ADD_TO_NEW_PLAYLIST_AND_PLAY), IconMgr::IconType::IT_Play_In_Playlist);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_COPY_TEXT), IconMgr::IconType::IT_Copy);
        menu.AppendItem(EX_ID(ID_BROWSE_PATH), IconMgr::IconType::IT_Folder_Explore);
        break;
    case MenuMgr::UiLibLeftMenu:
        menu.CreateMenu(true, false);
        menu.AppendItem(EX_ID(ID_PLAY_ITEM), IconMgr::IconType::IT_Play);
        menu.SetDefaultItem();
        menu.AppendSubMenu(AddToPlaylistMenu, IconMgr::IconType::IT_Add);
        menu.AppendItem(EX_ID(ID_ADD_TO_NEW_PLAYLIST_AND_PLAY), IconMgr::IconType::IT_Play_In_Playlist);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_COPY_TEXT), IconMgr::IconType::IT_Copy);
        menu.AppendItem(EX_ID(ID_VIEW_IN_MEDIA_LIB), IconMgr::IconType::IT_Media_Lib);
        break;
    case MenuMgr::UiRecentPlayedMenu:
        menu.CreateMenu(true, false);
        menu.AppendItem(EX_ID(ID_PLAY_ITEM), IconMgr::IconType::IT_Play);
        menu.SetDefaultItem();
        menu.AppendItem(EX_ID(ID_RECENT_PLAYED_REMOVE), IconMgr::IconType::IT_Close);
        menu.AppendItem(EX_ID(ID_COPY_TEXT), IconMgr::IconType::IT_Copy);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_VIEW_IN_MEDIA_LIB), IconMgr::IconType::IT_Media_Lib);
        menu.AppendItem(EX_ID(ID_LIB_RECENT_PLAYED_ITEM_PROPERTIES), IconMgr::IconType::IT_Info);
        break;
    case MenuMgr::UiMyFavouriteMenu:
        menu.CreateMenu(true, false);
        menu.AppendItem(EX_ID(ID_PLAY_ITEM), IconMgr::IconType::IT_Play);
        menu.SetDefaultItem();
        menu.AppendItem(EX_ID(ID_PLAY_AS_NEXT), IconMgr::IconType::IT_Play_As_Next);
        menu.AppendItem(EX_ID(ID_PLAY_ITEM_IN_FOLDER_MODE), IconMgr::IconType::IT_Play_In_Folder);
        menu.AppendSubMenu(AddToPlaylistMenu, IconMgr::IconType::IT_Add);
        menu.AppendSubMenu(RateMenu, IconMgr::IconType::IT_Star);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_EXPLORE_ONLINE), IconMgr::IconType::IT_Online);
        menu.AppendItem(EX_ID(ID_FORMAT_CONVERT), IconMgr::IconType::IT_Convert);
        menu.AppendItem(EX_ID(ID_EXPLORE_TRACK), IconMgr::IconType::IT_Folder_Explore);
        menu.AppendItem(EX_ID(ID_REMOVE_FROM_PLAYLIST), IconMgr::IconType::IT_Cancel);
        menu.AppendItem(EX_ID(ID_DELETE_FROM_DISK));
        menu.AppendItem(EX_ID(ID_ITEM_PROPERTY), IconMgr::IconType::IT_Info);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_COPY_TEXT), IconMgr::IconType::IT_Copy);
        break;
    case MenuMgr::LibPlaylistSortMenu:
        menu.CreateMenu(true, false);
        menu.AppendItem(EX_ID(ID_LIB_PLAYLIST_SORT_RECENT_PLAYED));
        menu.AppendItem(EX_ID(ID_LIB_PLAYLIST_SORT_RECENT_CREATED));
        menu.AppendItem(EX_ID(ID_LIB_PLAYLIST_SORT_NAME));
        break;
    case MenuMgr::LibFolderSortMenu:
        menu.CreateMenu(true, false);
        menu.AppendItem(EX_ID(ID_LIB_FOLDER_SORT_RECENT_PLAYED));
        menu.AppendItem(EX_ID(ID_LIB_FOLDER_SORT_RECENT_ADDED));
        menu.AppendItem(EX_ID(ID_LIB_FOLDER_SORT_PATH));
        break;
    case MenuMgr::LeMenu:
        menu.CreateMenu(false, false);
        menu.AppendSubMenu(LeFileMenu);
        menu.AppendSubMenu(LeEditMenu);
        menu.AppendSubMenu(LePlayCtrlMenu);
        break;
    case MenuMgr::LeFileMenu:
        menu.CreateMenu(true, false);
        menu.AppendItem(EX_ID(ID_LYRIC_OPEN), IconMgr::IconType::IT_Music);
        menu.AppendItem(EX_ID(ID_LYRIC_SAVE), IconMgr::IconType::IT_Save, L"Ctrl+S");
        menu.AppendItem(EX_ID(ID_LYRIC_SAVE_AS), IconMgr::IconType::IT_Save_As);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(IDCANCEL), IconMgr::IconType::IT_Exit);
        break;
    case MenuMgr::LeEditMenu:
        menu.CreateMenu(true, false);
        menu.AppendItem(EX_ID(ID_LYRIC_INSERT_TAG), IconMgr::IconType::IT_Le_Tag_Insert, L"F8");
        menu.AppendItem(EX_ID(ID_LYRIC_REPLACE_TAG), IconMgr::IconType::IT_Le_Tag_Replace, L"F9");
        menu.AppendItem(EX_ID(ID_LYRIC_DELETE_TAG), IconMgr::IconType::IT_Le_Tag_Delete, L"F10");
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_LYRIC_FIND), IconMgr::IconType::IT_Le_Find, L"Ctrl+F");
        menu.AppendItem(EX_ID(ID_LYRIC_REPLACE), IconMgr::IconType::IT_Le_Replace, L"Ctrl+H");
        menu.AppendItem(EX_ID(ID_FIND_NEXT), IconMgr::IconType::IT_NO_ICON, L"F3");
        menu.AppendSeparator();
        menu.AppendSubMenu(LeEditChConvMenu);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_LRYIC_MERGE_SAME_TIME_TAG));
        menu.AppendItem(EX_ID(ID_LYRIC_SWAP_TEXT_AND_TRANSLATION));
        menu.AppendSubMenu(LeEditTagDislocMenu);
        menu.AppendSubMenu(LeEditTranslateFormatConvertMenu);
        break;
    case MenuMgr::LeEditChConvMenu:
        menu.CreateMenu(true, false);
        menu.AppendItem(EX_ID(ID_LE_TRANSLATE_TO_SIMPLIFIED_CHINESE));
        menu.AppendItem(EX_ID(ID_LE_TRANSLATE_TO_TRANDITIONAL_CHINESE));
        break;
    case MenuMgr::LeEditTagDislocMenu:
        menu.CreateMenu(true, false);
        menu.AppendItem(EX_ID(ID_LYRIC_TIME_TAG_FORWARD));
        menu.AppendItem(EX_ID(ID_LYRIC_TIME_TAG_DELAY));
        break;
    case MenuMgr::LeEditTranslateFormatConvertMenu:
        menu.CreateMenu(true, false);
        menu.AppendItem(EX_ID(ID_LYRIC_AND_TRANSLATION_IN_SAME_LINE));
        menu.AppendItem(EX_ID(ID_LYRIC_AND_TRANSLATION_IN_DIFFERENT_LINE));
        break;
    case MenuMgr::LePlayCtrlMenu:
        menu.CreateMenu(true, false);
        menu.AppendItem(EX_ID(ID_PLAY_PAUSE), IconMgr::IconType::IT_Play_Pause, L"Ctrl+P");
        menu.AppendItem(EX_ID(ID_REW), IconMgr::IconType::IT_Rewind, CAcceleratorRes::Key(VK_LEFT, true).ToString().c_str());
        menu.AppendItem(EX_ID(ID_FF), IconMgr::IconType::IT_Fast_Forward, CAcceleratorRes::Key(VK_RIGHT, true).ToString().c_str());
        menu.AppendItem(EX_ID(ID_SEEK_TO_CUR_LINE), IconMgr::IconType::IT_Locate, L"Ctrl+G");
        break;
    case MenuMgr::FcListMenu:
        menu.CreateMenu(true, false);
        menu.AppendItem(EX_ID(ID_ADD_FILE), IconMgr::IconType::IT_Add);
        menu.AppendItem(EX_ID(ID_CLEAR_LIST));
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_EDIT_TAG_INFO), IconMgr::IconType::IT_Edit);
        menu.AppendItem(EX_ID(ID_DELETE_SELECT), IconMgr::IconType::IT_Cancel);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_MOVE_UP), IconMgr::IconType::IT_NO_ICON, CAcceleratorRes::Key(VK_UP, true).ToString().c_str());
        menu.AppendItem(EX_ID(ID_MOVE_DOWN), IconMgr::IconType::IT_NO_ICON, CAcceleratorRes::Key(VK_DOWN, true).ToString().c_str());
        break;
    case MenuMgr::LdListMenu:
        menu.CreateMenu(true, false);
        menu.AppendItem(EX_ID(ID_LD_LYRIC_DOWNLOAD), IconMgr::IconType::IT_Download);
        menu.SetDefaultItem();
        menu.AppendItem(EX_ID(ID_LD_PREVIEW));
        menu.AppendItem(EX_ID(ID_LD_LYRIC_SAVEAS), IconMgr::IconType::IT_Save_As);
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_LD_COPY_TITLE));
        menu.AppendItem(EX_ID(ID_LD_COPY_ARTIST));
        menu.AppendItem(EX_ID(ID_LD_COPY_ALBUM));
        menu.AppendItem(EX_ID(ID_LD_COPY_ID));
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_LD_RELATE));
        menu.AppendItem(EX_ID(ID_LD_VIEW_ONLINE), IconMgr::IconType::IT_Online);
        break;
    case MenuMgr::OptDlrcDefStyleMenu:
        menu.CreateMenu(true, false);
        menu.AppendItem(EX_ID(ID_LYRIC_DEFAULT_STYLE1));
        menu.AppendItem(EX_ID(ID_LYRIC_DEFAULT_STYLE2));
        menu.AppendItem(EX_ID(ID_LYRIC_DEFAULT_STYLE3));
        menu.AppendSeparator();
        menu.AppendSubMenu(OptDlrcModDefStyleMenu);
        break;
    case MenuMgr::OptDlrcModDefStyleMenu:
        menu.CreateMenu(true, false);
        menu.AppendItem(EX_ID(ID_LYRIC_DEFAULT_STYLE1_MODIFY));
        menu.AppendItem(EX_ID(ID_LYRIC_DEFAULT_STYLE2_MODIFY));
        menu.AppendItem(EX_ID(ID_LYRIC_DEFAULT_STYLE3_MODIFY));
        menu.AppendSeparator();
        menu.AppendItem(EX_ID(ID_RESTORE_DEFAULT_STYLE));
        break;
    case MenuMgr::PropertyAdvMenu:
        menu.CreateMenu(true, false);
        menu.AppendItem(EX_ID(ID_COPY_TEXT));
        menu.AppendItem(EX_ID(ID_COPY_ALL_TEXT));
        break;
    case MenuMgr::PropertyCoverMenu:
        menu.CreateMenu(true, false);
        menu.AppendItem(EX_ID(ID_COVER_BROWSE), IconMgr::IconType::IT_Folder_Explore);
        menu.AppendItem(EX_ID(ID_COVER_DELETE), IconMgr::IconType::IT_Cancel);
        menu.AppendItem(EX_ID(ID_COVER_SAVE_AS), IconMgr::IconType::IT_Save_As);
        menu.AppendItem(EX_ID(ID_COMPRESS_SIZE));
        break;
    default:
        ASSERT(false);                  // 参数错误或缺少case或缺少break
        menu.CreateMenu(true, true);    // 创建一个空的弹出菜单占位
        break;
    }
}
