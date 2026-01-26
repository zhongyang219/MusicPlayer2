#include "stdafx.h"
#include "CPlayerUIBase.h"
#include "MusicPlayerDlg.h"
#include "MiniModeUserUi.h"
#include "SongInfoHelper.h"
#include "UIElement/UIElement.h"
#include "MusicPlayerCmdHelper.h"
#include "UIWindowCmdHelper.h"
#include "UIElement/AbstractListElement.h"
#include "UIElement/TreeElement.h"
#include "UIElement/PlaylistIndicator.h"
#include "UIElement/NavigationBar.h"
#include "UIElement/SearchBox.h"
#include "PlayerFormulaHelper.h"

bool CPlayerUIBase::m_show_ui_tip_info = false;

CPlayerUIBase::CPlayerUIBase(UIData& ui_data, CWnd* pMainWnd)
    : m_ui_data(ui_data), m_pMainWnd(pMainWnd)
{

}


CPlayerUIBase::~CPlayerUIBase()
{
    m_mem_bitmap_static.DeleteObject();
}

void CPlayerUIBase::Init(CDC* pDC)
{
    m_pDC = pDC;
    //m_draw.Create(m_pDC, m_pMainWnd);
    m_draw.Create(m_pDC, &theApp.m_font_set.dlg.GetFont()); // m_draw的字体直接从m_font_set取得，CWnd->GetFont在不保留多语言窗口资源后不再准确

    m_tool_tip.Create(m_pMainWnd, TTS_ALWAYSTIP | TTS_NOPREFIX);
    m_tool_tip.SetMaxTipWidth(theApp.DPI(400));
    //为CToolTipCtrl设置鼠标穿透
    SetWindowLong(m_tool_tip.GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong(m_tool_tip.GetSafeHwnd(), GWL_EXSTYLE) | WS_EX_TRANSPARENT);
    m_first_draw = true;
}

void CPlayerUIBase::DrawInfo(bool reset)
{
    PreDrawInfo();

#if 0
    //双缓冲绘图
    {
        //CDrawDoubleBuffer drawDoubleBuffer(m_pDC, m_draw_rect);
        CDC memDC;
        CBitmap memBitmap;
        CBitmap* pOldBit;
        memDC.CreateCompatibleDC(NULL);
        if (m_mem_bitmap_static.GetSafeHandle() == NULL || reset)
        {
            memBitmap.CreateCompatibleBitmap(m_pDC, m_draw_rect.Width(), m_draw_rect.Height());
            pOldBit = memDC.SelectObject(&memBitmap);
            m_draw.SetDC(&memDC);     //将m_draw中的绘图DC设置为缓冲的DC
            m_draw.SetFont(&theApp.m_font_set.normal.GetFont(theApp.m_ui_data.full_screen));
            //绘制背景
            DrawBackground();
            m_mem_bitmap_static.DeleteObject();
            //bool b = m_mem_bitmap_static.Attach(CDrawCommon::CopyBitmap(memBitmap));
            CDrawCommon::CopyBitmap(m_mem_bitmap_static, memBitmap);
            CDrawCommon::SaveBitmap(memBitmap, L"D:\\Temp\\before.bmp");
            CDrawCommon::SaveBitmap(m_mem_bitmap_static, L"D:\\Temp\\after.bmp");
        }
        else
        {
            //memBitmap.Attach(CDrawCommon::CopyBitmap(m_mem_bitmap_static));
            CDrawCommon::CopyBitmap(memBitmap, m_mem_bitmap_static);
            pOldBit = memDC.SelectObject(&memBitmap);
            m_draw.SetDC(&memDC);     //将m_draw中的绘图DC设置为缓冲的DC
            m_draw.SetFont(&theApp.m_font_set.normal.GetFont(theApp.m_ui_data.full_screen));
        }
        //绘制界面中其他信息
        _DrawInfo(reset);
        ////绘制背景
        //DrawBackground();
        m_pDC->BitBlt(m_draw_rect.left, m_draw_rect.top, m_draw_rect.Width(), m_draw_rect.Height(), &memDC, 0, 0, SRCCOPY);
        memDC.SelectObject(pOldBit);
        memBitmap.DeleteObject();
        memDC.DeleteDC();
    }

#else
    //双缓冲绘图
    {
        CDrawDoubleBuffer drawDoubleBuffer(m_pDC, m_draw_rect);
        m_draw.SetDC(drawDoubleBuffer.GetMemDC());  //将m_draw中的绘图DC设置为缓冲的DC
        m_draw.SetFont(&theApp.m_font_set.GetFontBySize(9).GetFont(theApp.m_ui_data.full_screen));

        //绘制背景
        DrawBackground();

        //计算状态栏、标题栏、菜单栏的位置
        CRect draw_rect = m_draw_rect;
        CRect rc_status_bar;
        CRect rc_title_bar;
        CRect rc_menu_bar;
        //状态栏
        bool draw_status_bar = IsDrawStatusBar();
        if (draw_status_bar)
        {
            rc_status_bar = draw_rect;
            draw_rect.bottom -= DPI(20);
            rc_status_bar.top = draw_rect.bottom;
        }
        //标题栏
        if (IsDrawTitleBar())
        {
            rc_title_bar = draw_rect;
            rc_title_bar.bottom = rc_title_bar.top + m_layout.titlabar_height;
            draw_rect.top = rc_title_bar.bottom;
        }
        //菜单栏
        if (IsDrawMenuBar())
        {
            rc_menu_bar = draw_rect;
            rc_menu_bar.bottom = rc_menu_bar.top + m_layout.menubar_height;
            draw_rect.top = rc_menu_bar.bottom;
        }

        //绘制界面中其他信息
        _DrawInfo(draw_rect, reset);

        //绘制状态栏、标题栏、菜单栏
        if (!rc_status_bar.IsRectEmpty())
            DrawStatusBar(rc_status_bar, reset);
        if (!rc_title_bar.IsRectEmpty())
            DrawTitleBar(rc_title_bar);
        if (!rc_menu_bar.IsRectEmpty())
            DrawUiMenuBar(rc_menu_bar);

        //绘制提示信息
        if (m_show_ui_tip_info && !m_ui_tip_info.empty())
        {
            UiFontGuard guard(this, 10);
            //根据文本的长度计算提示信息的矩形区域
            CSize text_size = m_draw.GetTextExtent(m_ui_tip_info.c_str());
            int tip_max_width = draw_rect.Width() - EdgeMargin() * 2 - DPI(40);
            int lines = text_size.cx / tip_max_width + 1;
            int tip_width = tip_max_width;
            if (lines == 1)
                tip_width = text_size.cx;
            int tip_height = lines * text_size.cy;
            //仅当界面高度不小于提示信息矩形区域高度时才显示提示信息
            if (draw_rect.Height() >= tip_height)
            {
                CRect tip_rect{ draw_rect };
                tip_rect.top += (draw_rect.Height() - tip_height) / 2;
                tip_rect.left += (draw_rect.Width() - tip_width) / 2;
                tip_rect.bottom = tip_rect.top + tip_height;
                tip_rect.right = tip_rect.left + tip_width;
                //画背景
                CRect back_rect{ tip_rect };
                back_rect.InflateRect(DPI(20), DPI(20));
                DrawRectangle(back_rect);
                //画文字
                m_draw.DrawWindowText(tip_rect, m_ui_tip_info.c_str(), m_colors.color_text, Alignment::CENTER, true, true);
            }
        }

        //如果切换了显示/隐藏状态栏，则需要清除之前的鼠标提示
        static bool last_draw_status_bar{ false };
        if (draw_status_bar != last_draw_status_bar)
        {
            last_draw_status_bar = draw_status_bar;
            HideTooltip();
    }

        //如果标题栏显示按钮发生了改变，则需要清除之前的鼠标提示
        static int last_titlebar_btn{};
        int titlebar_btn{ theApp.m_app_setting_data.TitleDisplayItem() };
        if (last_titlebar_btn != titlebar_btn)
        {
            last_titlebar_btn = titlebar_btn;
            HideTooltip();
        }
    }

#endif

    if (m_first_draw)
    {
        AddToolTips();
    }
    else
    {
        static int last_width{}, last_height{}, last_ui_index{};
        //检测到绘图区域变化或界面进行了切换时
        if (last_width != m_draw_rect.Width() || last_height != m_draw_rect.Height()
            || (last_ui_index != GetUiIndex() && GetUiIndex() != 0))
        {
            //清除之前的鼠标提示
            HideTooltip();

            //更新任务栏缩略图区域
            CRect thumbnail_rect = GetThumbnailClipArea();
            CMusicPlayerDlg* pMainWindow = CMusicPlayerDlg::GetInstance();
            if (pMainWindow != nullptr)
            {
                pMainWindow->TaskBarSetClipArea(thumbnail_rect);
            }

            last_width = m_draw_rect.Width();
            last_height = m_draw_rect.Height();
            last_ui_index = GetUiIndex();
        }
    }
    m_first_draw = false;
}

bool CPlayerUIBase::LButtonDown(CPoint point)
{
    for (auto& btn : m_buttons)
    {
        if (btn.second.enable && btn.second.rect.PtInRect(point) != FALSE)
        {
            btn.second.pressed = true;
            return true;
        }
    }
    return false;
}

bool CPlayerUIBase::RButtonUp(CPoint point)
{
    if (!m_draw_rect.PtInRect(point))
        return false;

    if (m_buttons[BTN_VOLUME].rect.PtInRect(point) == FALSE)
        m_show_volume_adj = false;

    for (auto& btn : m_buttons)
    {
        if (btn.second.rect.PtInRect(point))
        {
            ButtonRClicked(btn.first, btn.second);
            return true;
        }
    }

    CPoint point1;
    GetCursorPos(&point1);

    // 其他区域显示主界面区域右键菜单
    theApp.m_menu_mgr.GetMenu(MenuMgr::MainAreaMenu)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, theApp.m_pMainWnd);
    return true;
}

bool CPlayerUIBase::MouseMove(CPoint point)
{
    bool rtn = false;
    for (auto& btn : m_buttons)
    {
        if (btn.second.enable)
        {
            bool hover = (btn.second.rect.PtInRect(point) != FALSE);
            if (!btn.second.hover && hover)
                UpdateMouseToolTipPosition(btn.first, btn.second.rect);

            btn.second.hover = hover;
            rtn |= btn.second.hover;
        }
    }

    TRACKMOUSEEVENT tme;
    tme.cbSize = sizeof(tme);
    tme.hwndTrack = m_pMainWnd->GetSafeHwnd();
    tme.dwFlags = TME_LEAVE | TME_HOVER;
    tme.dwHoverTime = 1;
    _TrackMouseEvent(&tme);
    return rtn;
}

bool CPlayerUIBase::LButtonUp(CPoint point)
{
    if (!m_show_volume_adj)     //如果设有显示音量调整按钮，则点击音量区域就显示音量调整按钮
        m_show_volume_adj = (m_buttons[BTN_VOLUME].rect.PtInRect(point) != FALSE);
    else        //如果已经显示了音量调整按钮，则点击音量调整时保持音量调整按钮的显示
        m_show_volume_adj = (m_buttons[BTN_VOLUME_UP].rect.PtInRect(point) || m_buttons[BTN_VOLUME_DOWN].rect.PtInRect(point));

    auto showMenu = [](const CRect& rect, CMenu* pMenu)
        {
            CPoint point;
            point.x = rect.left;
            point.y = rect.bottom;
            ClientToScreen(AfxGetMainWnd()->GetSafeHwnd(), &point);
            if (pMenu)
                pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
        };

    for (auto& btn : m_buttons)
    {
        bool pressed = btn.second.pressed;
        //if (btn.second.rect.PtInRect(point) != FALSE)
        //{
        //btn.second.hover = false;
        btn.second.pressed = false;
        //}

        if (pressed && btn.second.rect.PtInRect(point) && btn.second.enable)
        {
            if (ButtonClicked(btn.first, btn.second))
                return true;
            switch (btn.first)
            {
            //菜单
            case MENU_FILE:
                showMenu(btn.second.rect, theApp.m_menu_mgr.GetMenu(MenuMgr::MainFileMenu));
                return true;
            case MENU_PLAY_CONTROL:
                showMenu(btn.second.rect, theApp.m_menu_mgr.GetMenu(MenuMgr::MainPlayCtrlMenu));
                return true;
            case MENU_PLAYLIST:
                showMenu(btn.second.rect, theApp.m_menu_mgr.GetMenu(MenuMgr::MainPlaylistMenu));
                return true;
            case MENU_LYRICS:
                showMenu(btn.second.rect, theApp.m_menu_mgr.GetMenu(MenuMgr::MainLyricMenu));
                return true;
            case MENU_VIEW:
                showMenu(btn.second.rect, theApp.m_menu_mgr.GetMenu(MenuMgr::MainViewMenu));
                return true;
            case MENU_TOOLS:
                showMenu(btn.second.rect, theApp.m_menu_mgr.GetMenu(MenuMgr::MainToolMenu));
                return true;
            case MENU_HELP:
                showMenu(btn.second.rect, theApp.m_menu_mgr.GetMenu(MenuMgr::MainHelpMenu));
                return true;
            }
        }
    }
    return false;
}

bool CPlayerUIBase::ButtonClicked(BtnKey btn_type, const UIButton& btn)
{
    switch (btn_type)
    {
    case BTN_APP_CLOSE:
        theApp.m_pMainWnd->SendMessage(WM_CLOSE);
        return true;

    case BTN_MAXIMIZE:
        m_buttons[BTN_MAXIMIZE].hover = false;
        if (theApp.m_pMainWnd->IsZoomed())
            theApp.m_pMainWnd->SendMessage(WM_SYSCOMMAND, SC_RESTORE);
        else
            theApp.m_pMainWnd->SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE);
        return true;

    case BTN_MINIMIZE:
        m_buttons[BTN_MINIMIZE].hover = false;
        theApp.m_pMainWnd->SendMessage(WM_SYSCOMMAND, SC_MINIMIZE);
        return true;

    case BTN_REPETEMODE:
        theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_REPEAT_MODE);
        return true;

    case BTN_VOLUME:
        break;

    case BTN_TRANSLATE:
        theApp.m_lyric_setting_data.show_translate = !theApp.m_lyric_setting_data.show_translate;
        return true;

    case BTN_SKIN:
    case BTN_SKIN_TITLEBAR:
    {
        m_buttons[BTN_SKIN_TITLEBAR].hover = false;
        //theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_SWITCH_UI);
        CPoint point1;
        GetCursorPos(&point1);
        CMenu* pMenu = theApp.m_menu_mgr.GetMenu(MenuMgr::MainViewSwitchUiMenu);
        ASSERT(pMenu != nullptr);
        if (pMenu != nullptr)
            pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, theApp.m_pMainWnd);
        return true;
    }

    case BTN_EQ:
        theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_EQUALIZER);
        return true;

    case BTN_SETTING: case BTN_SETTING_TITLEBAR:
        m_buttons[BTN_SETTING_TITLEBAR].hover = false;
        theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_OPTION_SETTINGS);
        return true;

    case BTN_MINI: case BTN_MINI_TITLEBAR:
        m_buttons[BTN_MINI_TITLEBAR].hover = false;
        theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_MINI_MODE);
        return true;

    case BTN_INFO:
        theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_SONG_INFO);
        return true;

    case BTN_LRYIC:
        theApp.m_lyric_setting_data.show_desktop_lyric = !theApp.m_lyric_setting_data.show_desktop_lyric;
        return true;

    case BTN_AB_REPEAT:
        theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_AB_REPEAT);
        return true;

    case BTN_STOP:
        theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_STOP);
        return true;

    case BTN_PREVIOUS:
        theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_PREVIOUS);
        return true;
    case BTN_PLAY_PAUSE:
        theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_PLAY_PAUSE);
        return true;

    case BTN_NEXT:
        theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_NEXT);
        return true;

    case BTN_SHOW_PLAYLIST:
        if (theApp.m_media_lib_setting_data.playlist_btn_for_float_playlist)
            theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_FLOAT_PLAYLIST);
        else
            theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_SHOW_PLAYLIST);
        return true;

    case BTN_MEDIA_LIB:
        theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_MEDIA_LIB);
        return true;

    case BTN_FAVOURITE:
        theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_ADD_REMOVE_FROM_FAVOURITE);
        return true;

    case BTN_DARK_LIGHT:
        theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_DARK_MODE);
        return true;

    case BTN_DARK_LIGHT_TITLE_BAR:
        theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_DARK_MODE);
        return true;

    case BTN_LOCATE_TO_CURRENT:
        theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_LOCATE_TO_CURRENT);
        return true;

    case BTN_VOLUME_UP:
        if (m_show_volume_adj)
        {
            CPlayer::GetInstance().MusicControl(Command::VOLUME_ADJ, theApp.m_nc_setting_data.volum_step);
            return true;
        }
        break;

    case BTN_VOLUME_DOWN:
        if (m_show_volume_adj)
        {
            CPlayer::GetInstance().MusicControl(Command::VOLUME_ADJ, -theApp.m_nc_setting_data.volum_step);
            return true;
        }
        break;

    return true;

    case BTN_FIND:
        theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_FIND);
        return true;

    case BTN_FULL_SCREEN_TITLEBAR:
        theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_FULL_SCREEN);
        return true;

    case BTN_FULL_SCREEN:
        theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_FULL_SCREEN);
        return true;

    case BTN_MENU_TITLEBAR:
    {
        CPoint point(btn.rect.left, btn.rect.bottom);
        theApp.m_pMainWnd->SendMessage(WM_MAIN_MENU_POPEDUP, (WPARAM)&point);
        return true;
    }

    case BTN_MENU:
    {
        CPoint point(btn.rect.left, btn.rect.bottom);
        theApp.m_pMainWnd->SendMessage(WM_MAIN_MENU_POPEDUP, (WPARAM)&point);
        return true;
    }

    case BTN_ADD_TO_PLAYLIST:
    {
        CPoint point1;
        GetCursorPos(&point1);
        CMenu* add_to_menu = theApp.m_menu_mgr.GetMenu(MenuMgr::AddToPlaylistMenu);
        ASSERT(add_to_menu != nullptr);
        if (add_to_menu != nullptr)
        {
            add_to_menu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, theApp.m_pMainWnd);
        }
        return true;
    }

    case BTN_SWITCH_DISPLAY:
        SwitchStackElement();
        return true;

    case BTN_OPEN_FOLDER:
        theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_FILE_OPEN_FOLDER);
        return true;

    case BTN_NEW_PLAYLIST:
    {
        CMusicPlayerCmdHelper helper;
        helper.OnNewPlaylist();
        return true;
    }
    case BTN_PLAY_MY_FAVOURITE:
    {
        CMusicPlayerCmdHelper helper;
        helper.OnPlayMyFavourite();
        return true;
    }
    case BTN_PLAY_ALL_TRACKS:
    {
        CMusicPlayerCmdHelper helper;
        helper.OnPlayAllTrack();
        return true;
    }
    case BTN_MEDIALIB_FOLDER_SORT:
    {
        CPoint point(btn.rect.left, btn.rect.bottom);
        ClientToScreen(theApp.m_pMainWnd->GetSafeHwnd(), &point);
        CMenu* add_to_menu = theApp.m_menu_mgr.GetMenu(MenuMgr::LibFolderSortMenu);
        ASSERT(add_to_menu != nullptr);
        if (add_to_menu != nullptr)
        {
            CUIWindowCmdHelper helper(nullptr);
            helper.SetMenuState(add_to_menu);
            UINT command = add_to_menu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY, point.x, point.y, theApp.m_pMainWnd);
            helper.OnUiCommand(command);
        }
        return true;
    }
    case BTN_MEDIALIB_PLAYLIST_SORT:
    {
        CPoint point(btn.rect.left, btn.rect.bottom);
        ClientToScreen(theApp.m_pMainWnd->GetSafeHwnd(), &point);
        CMenu* add_to_menu = theApp.m_menu_mgr.GetMenu(MenuMgr::LibPlaylistSortMenu);
        ASSERT(add_to_menu != nullptr);
        if (add_to_menu != nullptr)
        {
            CUIWindowCmdHelper helper(nullptr);
            helper.SetMenuState(add_to_menu);
            UINT command = add_to_menu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY, point.x, point.y, theApp.m_pMainWnd);
            helper.OnUiCommand(command);
        }
        return true;
    }
    case BTN_KARAOKE:
        theApp.m_lyric_setting_data.lyric_karaoke_disp = !theApp.m_lyric_setting_data.lyric_karaoke_disp;
        return true;

    default:
        break;
    }
    return false;
}

bool CPlayerUIBase::ButtonRClicked(BtnKey btn_type, const UIButton& btn)
{
    CPoint point1;
    GetCursorPos(&point1);

    if (btn_type == BTN_REPETEMODE)
    {
        CMenu* pMenu = theApp.m_menu_mgr.GetMenu(MenuMgr::MainPlayCtrlRepeatModeMenu);
        ASSERT(pMenu != nullptr);
        if (pMenu != NULL)
            pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, theApp.m_pMainWnd);
        return true;
    }

    if (btn_type == BTN_SHOW_PLAYLIST)
    {
        CMenu* pMenu = theApp.m_menu_mgr.GetMenu(MenuMgr::MainAreaPlaylistBtnMenu);
        ASSERT(pMenu != nullptr);
        if (pMenu != NULL)
            pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, theApp.m_pMainWnd);
        return true;
    }

    if (btn_type == BTN_AB_REPEAT)
    {
        CMenu* pMenu = theApp.m_menu_mgr.GetMenu(MenuMgr::MainPlayCtrlAbRepeatMenu);
        ASSERT(pMenu != nullptr);
        if (pMenu != nullptr)
            pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, theApp.m_pMainWnd);
        return true;
    }

    if (btn_type == BTN_SKIN)
    {
        CMenu* pMenu = theApp.m_menu_mgr.GetMenu(MenuMgr::MainViewSwitchUiMenu);
        ASSERT(pMenu != nullptr);
        if (pMenu != nullptr)
            pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, theApp.m_pMainWnd);
        return true;
    }

    return false;
}


bool CPlayerUIBase::RButtonDown(CPoint point)
{
    return false;
}

bool CPlayerUIBase::MouseWheel(int delta, CPoint point)
{
    return false;
}

bool CPlayerUIBase::DoubleClick(CPoint point)
{
    return false;
}

CRect CPlayerUIBase::GetThumbnailClipArea()
{
    //获取菜单栏的高度
    int menu_bar_height = 0;
    if (m_ui_data.ShowWindowMenuBar() && theApp.m_app_setting_data.show_window_frame)
    {
        menu_bar_height = CCommon::GetMenuBarHeight(theApp.m_pMainWnd->GetSafeHwnd());
        if (menu_bar_height == 0)
            menu_bar_height = theApp.DPI(20);
        else
            menu_bar_height += theApp.DPI(1);
    }

    CRect thumbnail_rect = m_thumbnail_rect;
    thumbnail_rect.MoveToY(thumbnail_rect.top + menu_bar_height);
    return thumbnail_rect;
}

void CPlayerUIBase::UpdateRepeatModeToolTip()
{
    SetRepeatModeToolTipText();
    UpdateMouseToolTip(BTN_REPETEMODE, m_repeat_mode_tip.c_str());
}

void CPlayerUIBase::UpdateSongInfoToolTip()
{
    SetSongInfoToolTipText();
    UpdateMouseToolTip(BTN_INFO, m_info_tip.c_str());
}

void CPlayerUIBase::UpdatePlayPauseButtonTip()
{
    std::wstring play_pause_tooltip = GetItemTooltip(BTN_PLAY_PAUSE);
    UpdateMouseToolTip(BTN_PLAY_PAUSE, play_pause_tooltip.c_str());
}

void CPlayerUIBase::UpdateFullScreenTip()
{
    std::wstring fullscreen_tooltip = GetItemTooltip(BTN_FULL_SCREEN);
    UpdateMouseToolTip(BTN_FULL_SCREEN, fullscreen_tooltip.c_str());
    UpdateMouseToolTip(BTN_FULL_SCREEN_TITLEBAR, fullscreen_tooltip.c_str());
}

void CPlayerUIBase::UpdateTitlebarBtnToolTip()
{
    if (theApp.m_pMainWnd->IsZoomed())
        UpdateMouseToolTip(BTN_MAXIMIZE, theApp.m_str_table.LoadText(L"UI_TIP_BTN_RESTORE").c_str());
    else
        UpdateMouseToolTip(BTN_MAXIMIZE, theApp.m_str_table.LoadText(L"UI_TIP_BTN_MAXIMIZE").c_str());
}

bool CPlayerUIBase::SetCursor()
{
    return false;
}

bool CPlayerUIBase::MouseLeave()
{
    for (auto& btn : m_buttons)
    {
        btn.second.hover = false;
        btn.second.pressed = false;
    }
    return true;
}

void CPlayerUIBase::ClearBtnRect()
{
    for (auto& btn : m_buttons)
    {
        btn.second.rect = CRect();
    }
}

CPlayerUIBase::UiSize CPlayerUIBase::GetUiSize() const
{
    // 绘图高度
    int draw_height = m_draw_rect.Height();
    if (IsDrawTitleBar())
        draw_height -= m_layout.titlabar_height;

    // 如果是显示播放列表且处于窄模式，返回 SMALL
    if (draw_height <= m_layout.height_threshold || (m_ui_data.show_playlist && m_ui_data.narrow_mode))
    {
        return UiSize::SMALL;
    }

    // 检查是否应该使用大尺寸UI
    const bool is_wide_layout = !m_ui_data.narrow_mode && !m_ui_data.show_playlist;
    const bool is_large_width = m_draw_rect.Width() > m_layout.width_threshold;

    if (is_wide_layout || is_large_width)
    {
        return UiSize::BIG;
    }

    // 其他情况返回 NARROW
    return UiSize::NARROW;
}

IconMgr::IconType CPlayerUIBase::GetBtnIconType(BtnKey key)
{
    switch (key)
    {
    case BTN_REPETEMODE:
        switch (CPlayer::GetInstance().GetRepeatMode())
        {
        case RepeatMode::RM_PLAY_ORDER:
            return IconMgr::IconType::IT_Play_Order;
        case RepeatMode::RM_LOOP_PLAYLIST:
            return IconMgr::IconType::IT_Loop_Playlist;
        case RepeatMode::RM_LOOP_TRACK:
            return IconMgr::IconType::IT_Loop_Track;
        case RepeatMode::RM_PLAY_SHUFFLE:
            return IconMgr::IconType::IT_Play_Shuffle;
        case RepeatMode::RM_PLAY_RANDOM:
            return IconMgr::IconType::IT_Play_Random;
        case RepeatMode::RM_PLAY_TRACK:
            return IconMgr::IconType::IT_Play_Track;
        default:
            return IconMgr::IconType::IT_NO_ICON;
        }
    case BTN_VOLUME:
        if (CPlayer::GetInstance().GetVolume() <= 0)
            return IconMgr::IconType::IT_Volume0;
        else if (CPlayer::GetInstance().GetVolume() >= 66)
            return IconMgr::IconType::IT_Volume3;
        else if (CPlayer::GetInstance().GetVolume() >= 33)
            return IconMgr::IconType::IT_Volume2;
        else
            return IconMgr::IconType::IT_Volume1;
    case BTN_SKIN:
    case BTN_SKIN_TITLEBAR:
        return IconMgr::IconType::IT_Skin;
    case BTN_EQ:
        return IconMgr::IconType::IT_Equalizer;
    case BTN_SETTING:
    case BTN_SETTING_TITLEBAR:
    case BTN_SHOW_SETTINGS_PANEL:
        return IconMgr::IconType::IT_Setting;
    case BTN_MINI:
    case BTN_MINI_TITLEBAR:
        if (IsMiniMode())
            return IconMgr::IconType::IT_Mini_Off;
        else
            return IconMgr::IconType::IT_Mini_On;
    case BTN_INFO:
        return IconMgr::IconType::IT_Info;
    case BTN_FIND:
        return IconMgr::IconType::IT_Find;
    case BTN_STOP:
        return IconMgr::IconType::IT_Stop;
    case BTN_PREVIOUS:
        return IconMgr::IconType::IT_Previous;
    case BTN_PLAY_PAUSE:
        if (CPlayer::GetInstance().IsPlaying())
            return IconMgr::IconType::IT_Pause;
        else
            return IconMgr::IconType::IT_Play;
    case BTN_NEXT:
        return IconMgr::IconType::IT_Next;
    case BTN_SHOW_PLAYLIST:
        return IconMgr::IconType::IT_Playlist;
    case BTN_MEDIA_LIB:
        return IconMgr::IconType::IT_Media_Lib;
    case BTN_FULL_SCREEN:
    case BTN_FULL_SCREEN_TITLEBAR:
        if (m_ui_data.full_screen)
            return IconMgr::IconType::IT_Full_Screen_Off;
        else
            return IconMgr::IconType::IT_Full_Screen_On;
    case BTN_MENU:
    case BTN_MENU_TITLEBAR:
        return IconMgr::IconType::IT_Menu;
    case BTN_FAVOURITE:
        if (CPlayer::GetInstance().IsFavourite())
            return IconMgr::IconType::IT_Favorite_Off;
        else
            return IconMgr::IconType::IT_Favorite_On;
    case BTN_MINIMIZE:
        return IconMgr::IconType::IT_Minimize;
    case BTN_MAXIMIZE:
        if (theApp.m_pMainWnd->IsZoomed())
            return IconMgr::IconType::IT_Maxmize_Off;
        else
            return IconMgr::IconType::IT_Maxmize_On;
    case BTN_CLOSE:
    case BTN_APP_CLOSE:
        return IconMgr::IconType::IT_Close;
    case BTN_ADD_TO_PLAYLIST:
        return IconMgr::IconType::IT_Add;
    case BTN_SWITCH_DISPLAY:
        return IconMgr::IconType::IT_Switch_Display;
    case BTN_DARK_LIGHT: case BTN_DARK_LIGHT_TITLE_BAR:    // 之前是一个IconRes的深浅色，现拆分为两个图标类型，将来如果换图标主题要轻松一些
        if (theApp.m_app_setting_data.dark_mode)
            return IconMgr::IconType::IT_Dark_Mode_Off;
        else
            return IconMgr::IconType::IT_Dark_Mode_On;
    case BTN_LOCATE_TO_CURRENT:
        return IconMgr::IconType::IT_Locate;
    case MENU_FILE:
        return IconMgr::IconType::IT_Folder;
    case MENU_PLAY_CONTROL:
        return IconMgr::IconType::IT_Play;
    case MENU_PLAYLIST:
        return IconMgr::IconType::IT_Playlist;
    case MENU_LYRICS:
        return IconMgr::IconType::IT_Lyric;
    case MENU_VIEW:
        return IconMgr::IconType::IT_Playlist_Dock;
    case MENU_TOOLS:
        return IconMgr::IconType::IT_Setting;
    case MENU_HELP:
        return IconMgr::IconType::IT_Help;
    case BTN_OPEN_FOLDER:
        return IconMgr::IconType::IT_NewFolder;
    case BTN_NEW_PLAYLIST:
        return IconMgr::IconType::IT_Add;
    case BTN_PLAY_MY_FAVOURITE: case BTN_PLAY_ALL_TRACKS:
        return IconMgr::IconType::IT_Play;
    case BTN_MEDIALIB_FOLDER_SORT: case BTN_MEDIALIB_PLAYLIST_SORT:
        return IconMgr::IconType::IT_Sort_Mode;
    case BTN_KARAOKE:
        return IconMgr::IconType::IT_Karaoke;
    case BTN_SHOW_PLAY_QUEUE:
        return IconMgr::IconType::IT_Playlist;
    case BTN_CLOSE_PANEL:
        return IconMgr::IconType::IT_Close;
    case BTN_SHOW_PANEL:
        return IconMgr::IconType::IT_Playlist;
    case BTN_SHOW_HIDE_ELEMENT:
        return IconMgr::IconType::IT_Background;
    case BTN_CLOSE_PANEL_TITLE_BAR:
        return IconMgr::IT_Arrow_Left;
    default:
        //ASSERT(FALSE);
        return IconMgr::IconType::IT_NO_ICON;
    }
}

std::wstring CPlayerUIBase::GetButtonText(BtnKey key_type) const
{
    switch (key_type)
    {
    case BTN_REPETEMODE:
        switch (CPlayer::GetInstance().GetRepeatMode())
        {
        case RM_PLAY_ORDER: return theApp.m_str_table.LoadText(L"UI_TIP_REPEAT_ORDER");
        case RM_PLAY_SHUFFLE: return theApp.m_str_table.LoadText(L"UI_TIP_REPEAT_SHUFFLE");
        case RM_PLAY_RANDOM: return theApp.m_str_table.LoadText(L"UI_TIP_REPEAT_RANDOM");
        case RM_LOOP_PLAYLIST: return theApp.m_str_table.LoadText(L"UI_TIP_REPEAT_PLAYLIST");
        case RM_LOOP_TRACK: return theApp.m_str_table.LoadText(L"UI_TIP_REPEAT_TRACK");
        case RM_PLAY_TRACK: return theApp.m_str_table.LoadText(L"UI_TIP_REPEAT_ONCE");
        }
        break;
    case BTN_SKIN: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_SWITCH_UI");
    case BTN_EQ: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_SOUND_EFFECT_SETTING");
    case BTN_SETTING: case BTN_SHOW_SETTINGS_PANEL: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_OPTION_SETTING");
    case BTN_MINI: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_MINIMODE");
    case BTN_INFO: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_PROPERTY");
    case BTN_FIND: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_FIND_SONGS");
    case BTN_STOP: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_STOP");
    case BTN_PREVIOUS: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_STOP");
    case BTN_PLAY_PAUSE: return theApp.m_str_table.LoadText(CPlayer::GetInstance().IsPlaying() ? L"UI_TIP_BTN_PAUSE" : L"UI_TIP_BTN_PLAY");
    case BTN_NEXT: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_NEXT");
    case BTN_SHOW_PLAYLIST: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_PLAYLIST_SHOW_HIDE");
    case BTN_MEDIA_LIB: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_MEDIA_LIB");
    case BTN_FULL_SCREEN: return theApp.m_str_table.LoadText(m_ui_data.full_screen ? L"UI_TIP_BTN_FULL_SCREEN_EXIT" : L"UI_TIP_BTN_FULL_SCREEN");
    case BTN_MENU: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_MAIN_MENU");
    case BTN_FAVOURITE: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_FAVOURITE");
    case BTN_ADD_TO_PLAYLIST: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_ADD_TO_PLAYLIST");
    case BTN_SWITCH_DISPLAY: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_SWITCH_DISPLAY");
    case BTN_DARK_LIGHT: case BTN_DARK_LIGHT_TITLE_BAR: return theApp.m_str_table.LoadText(theApp.m_app_setting_data.dark_mode ? L"UI_TIP_BTN_DARK_LIGHT_TO_LIGHT_MODE" : L"UI_TIP_BTN_DARK_LIGHT_TO_DARK_MODE");
    case BTN_LOCATE_TO_CURRENT:return theApp.m_str_table.LoadText(L"UI_TIP_BTN_LOCATE_TO_CURRENT");
    case BTN_OPEN_FOLDER: return theApp.m_str_table.LoadText(L"UI_TXT_BTN_OPEN_FOLDER");
    case BTN_NEW_PLAYLIST: return theApp.m_str_table.LoadText(L"UI_TXT_BTN_NEW_PLAYLIST");
    case BTN_PLAY_MY_FAVOURITE: case BTN_PLAY_ALL_TRACKS: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_PLAY");
    case BTN_MEDIALIB_FOLDER_SORT: case BTN_MEDIALIB_PLAYLIST_SORT: return theApp.m_str_table.LoadText(L"TXT_LIB_PLAYLIST_SORT");
    case BTN_CLOSE_PANEL: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_CLOSE");
    case BTN_CLOSE_PANEL_TITLE_BAR: return theApp.m_str_table.LoadText(L"UI_TIP_BTN_BACK");
    }

    return std::wstring();
}

void CPlayerUIBase::PreDrawInfo()
{
    //设置颜色
    m_colors = CPlayerUIHelper::GetUIColors(theApp.m_app_setting_data.dark_mode, IsDrawBackgroundAlpha());

    //设置绘制的矩形区域
    m_draw_rect = CRect(0, 0, m_ui_data.draw_area_width, m_ui_data.draw_area_height);

    //检测到界面布局发生了变化时
    static UiSize last_ui_size{};
    if (last_ui_size != GetUiSize())
    {
        ClearBtnRect();
        UiSizeChanged(last_ui_size);
        last_ui_size = GetUiSize();
    }
}

void CPlayerUIBase::DrawBackground()
{
    CRect draw_rect = m_draw_rect;
    draw_rect.MoveToXY(0, 0);

    //绘制背景
    if (theApp.m_app_setting_data.enable_background)
    {
        if (CPlayer::GetInstance().AlbumCoverExist() && theApp.m_app_setting_data.album_cover_as_background)
        {
            CImage& back_image{ theApp.m_app_setting_data.background_gauss_blur ? CPlayer::GetInstance().GetAlbumCoverBlur() : CPlayer::GetInstance().GetAlbumCover() };
            m_draw.DrawBitmap(back_image, CPoint(0, 0), m_draw_rect.Size(), CDrawCommon::StretchMode::FILL);
        }
        else
        {
            CSingleLock sync(&m_ui_data.default_background_sync, TRUE);
            //MemDC.FillSolidRect(0, 0, m_draw_rect.Width(), m_draw_rect.Height(), GetSysColor(COLOR_BTNFACE)); //给缓冲DC的绘图区域填充对话框的背景颜色
            m_draw.DrawBitmap(m_ui_data.default_background, CPoint(0, 0), m_draw_rect.Size(), CDrawCommon::StretchMode::FILL);
        }
    }

    //填充背景颜色
    if (IsDrawBackgroundAlpha())
        m_draw.FillAlphaRect(draw_rect, m_colors.color_back, ALPHA_CHG(theApp.m_app_setting_data.background_transparency));
    else
        m_draw.FillRect(draw_rect, m_colors.color_back);
}

void CPlayerUIBase::DrawSongInfo(CRect rect, int font_size, bool reset)
{
    //设置字体
    UiFontGuard set_font(this, font_size);

    //绘制播放状态
    CRect rc_tmp{ rect };
    if (!theApp.m_app_setting_data.always_show_statusbar)
    {
        CString play_state_str = CPlayer::GetInstance().GetPlayingState().c_str();
        //m_draw.GetDC()->SelectObject(&theApp.m_font_set.dlg.GetFont());
        rc_tmp.right = rc_tmp.left + m_draw.GetTextExtent(play_state_str).cx + DPI(4);
        m_draw.DrawWindowText(rc_tmp, play_state_str, m_colors.color_text_lable);
    }
    else     //如果总是显示状态栏，由于状态栏中已经有播放状态了，因此这里不显示播放状态
    {
        rc_tmp.right = rc_tmp.left;
    }

    //绘制歌曲序号
    rc_tmp.MoveToX(rc_tmp.right);
    wchar_t buff[128];
    swprintf_s(buff, sizeof(buff) / 2, L"%.3d", CPlayer::GetInstance().GetIndex() + 1);
    int index_width = m_draw.GetTextExtent(buff).cx;
    rc_tmp.right = rc_tmp.left + index_width + DPI(4);
    m_draw.DrawWindowText(rc_tmp, buff, m_colors.color_text_2);

    //绘制标识
    wstring tag_str;
    if (CPlayer::GetInstance().GetSafeCurrentSongInfo().is_cue)
        tag_str = L"cue";
    else if (CPlayer::GetInstance().IsMidi())
        tag_str = L"midi";
    else if (CPlayer::GetInstance().IsOsuFile())
        tag_str = L"osu!";
    int available_width = rect.right - rc_tmp.right;
    if (available_width >= DPI(50) && !tag_str.empty())
    {
        int width = m_draw.GetTextExtent(tag_str.c_str()).cx + DPI(8);
        rc_tmp.MoveToX(rc_tmp.right);
        rc_tmp.right = rc_tmp.left + width;
        DrawPlayTag(rc_tmp, tag_str.c_str());
    }

    //绘制播放速度、播放变调
    DrawPlayEffectTag(rect, rc_tmp);

    //绘制文件名
    rc_tmp.MoveToX(rc_tmp.right + (tag_str.empty() ? 0 : DPI(4)));
    rc_tmp.right = rect.right;
    if (rc_tmp.Width() >= DPI(4))
    {
        static CDrawCommon::ScrollInfo scroll_info1;
        m_draw.DrawScrollText(rc_tmp, CPlayer::GetInstance().GetDisplayName().c_str(), m_colors.color_text, GetScrollTextPixel(), false, scroll_info1, reset);
    }
}

void CPlayerUIBase::DrawPlayEffectTag(CRect parent_rect, CRect& previous_item_rect)
{
    wstring tag_str;
    int available_width = parent_rect.right - previous_item_rect.right;
    //绘制播放速度
    if (CPlayer::GetInstance().IsSpeedAvailable() && std::fabs(CPlayer::GetInstance().GetSpeed() - 1) > 1e-3)
    {
        wchar_t buff[64];
        swprintf_s(buff, L"%.2f", CPlayer::GetInstance().GetSpeed());
        tag_str = buff;
        if (!tag_str.empty() && tag_str.back() == L'0')
            tag_str.pop_back();
        tag_str.push_back(L'X');
        available_width = parent_rect.right - previous_item_rect.right;
        if (available_width >= DPI(50))
        {
            int width = m_draw.GetTextExtent(tag_str.c_str()).cx + DPI(8);
            previous_item_rect.MoveToX(previous_item_rect.right + DPI(2));
            previous_item_rect.right = previous_item_rect.left + width;
            DrawPlayTag(previous_item_rect, tag_str.c_str());
        }
    }

    // 绘制播放变调
    if (CPlayer::GetInstance().IsPitchAvailable() && CPlayer::GetInstance().GetPitch() != 0)
    {
        wchar_t buff[64];
        int pitch = CPlayer::GetInstance().GetPitch();
        wchar_t sign = pitch > 0 ? L'+' : L'-';
        swprintf_s(buff, L"K%c%d", sign, std::abs(pitch));
        tag_str = buff;
        available_width = parent_rect.right - previous_item_rect.right;
        if (available_width >= DPI(50))
        {
            int width = m_draw.GetTextExtent(tag_str.c_str()).cx + DPI(8);
            previous_item_rect.MoveToX(previous_item_rect.right + DPI(2));
            previous_item_rect.right = previous_item_rect.left + width;
            DrawPlayTag(previous_item_rect, tag_str.c_str());
        }
    }

}

void CPlayerUIBase::DrawPlayTag(CRect rect, LPCTSTR str_text)
{
    DrawAreaGuard guard(&m_draw, rect);
    //CRect rc_border = rect;
    //rc_border.top += DPI(2);
    //rc_border.bottom -= DPI(1);
    //m_draw.DrawRectOutLine(rc_border, m_colors.color_text, DPI(1), false);
    DrawRectangle(rect);
    m_draw.DrawWindowText(rect, str_text, m_colors.color_text, Alignment::CENTER);
}

void CPlayerUIBase::DrawRectangle(const CRect& rect, bool no_corner_radius, bool theme_color, ColorMode color_mode)
{
    bool draw_background{ IsDrawBackgroundAlpha() };
    //绘制背景
    BYTE alpha;
    if (!draw_background)
        alpha = 255;
    else if (theApp.m_app_setting_data.dark_mode)
        alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency) * 2 / 3;
    else
        alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency);

    COLORREF fill_color{};
    if (color_mode == RCM_DARK)
    {
        fill_color = CColorConvert::m_gray_color.dark3;
        if (draw_background)
            alpha = 108;
    }
    else if (color_mode == RCM_LIGHT)
    {
        if (theme_color)
            fill_color = theApp.m_app_setting_data.theme_color.light3;
        else
            fill_color = CColorConvert::m_gray_color.light4;
    }
    else
    {
        if (theme_color)
        {
            fill_color = m_colors.color_control_bar_back;
        }
        else
        {
            if (theApp.m_app_setting_data.dark_mode)
                fill_color = CColorConvert::m_gray_color.dark3;
            else
                fill_color = CColorConvert::m_gray_color.light4;
        }
    }

    if (!theApp.m_app_setting_data.button_round_corners || no_corner_radius)
        m_draw.FillAlphaRect(rect, fill_color, alpha);
    else
    {
        m_draw.DrawRoundRect(rect, fill_color, CalculateRoundRectRadius(rect), alpha);
    }
}

void CPlayerUIBase::DrawRectangle(CRect rect, COLORREF color)
{
    BYTE alpha;
    if (IsDrawBackgroundAlpha())
        alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency) * 2 / 3;
    else
        alpha = 255;
    if (!theApp.m_app_setting_data.button_round_corners)
        m_draw.FillAlphaRect(rect, color, alpha, true);
    else
        m_draw.DrawRoundRect(rect, color, CalculateRoundRectRadius(rect), alpha);

}

void CPlayerUIBase::DrawBeatIndicator(CRect rect)
{
    int progress;
    CPlayTime time{ CPlayer::GetInstance().GetCurrentPosition() };
    if (CPlayer::GetInstance().IsMidi())
    {
        ////progress = (CPlayer::GetInstance().GetMidiInfo().midi_position % 16 + 1) *1000 / 16;
        //if (CPlayer::GetInstance().GetMidiInfo().tempo == 0)
        //  progress = 0;
        //else
        //  progress = (time.toInt() * 1000 / CPlayer::GetInstance().GetMidiInfo().tempo % 4 + 1) * 250;
        progress = (CPlayer::GetInstance().GetMidiInfo().midi_position % 4 + 1) * 250;
        if (progress == 1000)
            progress = 999;
    }
    else
    {
        progress = (time.sec % 4 * 1000 + time.msec) / 4;
    }
    m_draw.DrawWindowText(rect, _T("<<<<"), m_colors.color_text, m_colors.color_text_2, progress);
}

void CPlayerUIBase::DrawUIButton(const CRect& rect, BtnKey key_type, bool big_icon, bool show_text, int font_size, bool checked)
{
    auto& btn = m_buttons[key_type];
    std::wstring text;
    if (show_text)
    {
        text = GetButtonText(key_type);
    }
    DrawUIButton(rect, btn, GetBtnIconType(key_type), big_icon, text, font_size, checked);
}

void CPlayerUIBase::DrawUIButton(const CRect& rect, BtnKey key_type, UIButton& btn, bool big_icon, bool show_text, int font_size, bool checked)
{
    std::wstring text;
    if (show_text)
    {
        text = GetButtonText(key_type);
    }
    DrawUIButton(rect, btn, GetBtnIconType(key_type), big_icon, text, font_size, checked);
}

void CPlayerUIBase::DrawUIButton(const CRect& rect, UIButton& btn, IconMgr::IconType icon_type, bool big_icon, const std::wstring& text, int font_size, bool checked)
{
    btn.rect = rect;

    CRect rc_tmp = rect;
    if (btn.pressed && btn.enable)
        rc_tmp.MoveToXY(rect.left + theApp.DPI(1), rect.top + theApp.DPI(1));

    //rc_tmp.DeflateRect(DPI(2), DPI(2));
    //m_draw.SetDrawArea(rc_tmp);

    //绘制的是否为关闭按钮（关闭按钮需要特别处理）
    bool is_close_btn = (&btn == &m_buttons[BTN_CLOSE] || &btn == &m_buttons[BTN_APP_CLOSE]);

    //绘制背景
    if (btn.enable && (btn.pressed || btn.hover || checked))
    {
        BYTE alpha;
        if (!is_close_btn && IsDrawBackgroundAlpha())
            alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency) * 2 / 3;
        else
            alpha = 255;
        COLORREF back_color{};
        if (is_close_btn)
        {
            if (btn.pressed)
                back_color = RGB(173, 17, 29);
            else
                back_color = RGB(232, 17, 35);
        }
        else
        {
            if (btn.pressed)
                back_color = m_colors.color_button_pressed;
            else if (btn.hover)
                back_color = m_colors.color_button_hover;
            else if (checked)
                back_color = m_colors.color_button_checked;
        }
        if (!theApp.m_app_setting_data.button_round_corners)
            m_draw.FillAlphaRect(rc_tmp, back_color, alpha, true);
        else
            m_draw.DrawRoundRect(rc_tmp, back_color, CalculateRoundRectRadius(rc_tmp), alpha);
    }

    CRect rect_icon{ rc_tmp };
    if (!text.empty())
        rect_icon.right = rect_icon.left + rect_icon.Height();      //如果要显示文本，则图标显示矩形左侧的正方形区域

    IconMgr::IconStyle icon_style = (is_close_btn && (btn.pressed || btn.hover)) ? IconMgr::IconStyle::IS_OutlinedLight : IconMgr::IconStyle::IS_Auto;
    IconMgr::IconSize icon_size = big_icon ? IconMgr::IconSize::IS_DPI_20 : IconMgr::IconSize::IS_DPI_16;
    DrawUiIcon(rect_icon, icon_type, icon_style, icon_size);

    //绘制文本
    if (!text.empty())
    {
        UiFontGuard set_font(this, font_size);

        CRect rect_text{ rc_tmp };
        rect_text.left = rect_icon.right;
        int right_space = (rc_tmp.Height() - DPI(16)) / 2;
        rect_text.right -= right_space;
        m_draw.DrawWindowText(rect_text, text.c_str(), m_colors.color_text, Alignment::LEFT, true);

    }
}

void CPlayerUIBase::DrawTextButton(CRect rect, BtnKey btn_type, LPCTSTR text, bool checked)
{
    auto& btn = m_buttons[btn_type];
    DrawTextButton(rect, btn, text, checked);
}

void CPlayerUIBase::DrawTextButton(CRect rect, UIButton& btn, LPCTSTR text, bool checked)
{
    if (btn.enable)
    {
        if (btn.pressed)
            rect.MoveToXY(rect.left + theApp.DPI(1), rect.top + theApp.DPI(1));

        //绘制的是否为关闭按钮（关闭按钮需要特别处理）
        bool is_close_btn = (&btn == &m_buttons[BTN_CLOSE] || &btn == &m_buttons[BTN_APP_CLOSE]);

        BYTE alpha;
        if (!is_close_btn && IsDrawBackgroundAlpha())
            alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency) * 2 / 3;
        else
            alpha = 255;
        if (btn.pressed || btn.hover || checked)
        {
            COLORREF background_color{};
            if (is_close_btn)
            {
                if (btn.pressed)
                {
                    background_color = RGB(173, 17, 29);
                }
                else if (btn.hover)
                {
                    background_color = RGB(232, 17, 35);
                }
            }
            else
            {
                if (btn.pressed)
                {
                    background_color = m_colors.color_button_pressed;
                }
                else if (btn.hover)
                {
                    background_color = m_colors.color_button_hover;
                }
                else if (checked)
                {
                    background_color = m_colors.color_button_checked;
                }
            }

            if (!theApp.m_app_setting_data.button_round_corners)
            {
                m_draw.FillAlphaRect(rect, background_color, alpha, true);
            }
            else
            {
                m_draw.DrawRoundRect(rect, background_color, CalculateRoundRectRadius(rect), alpha);
            }
        }
        COLORREF front_color{ m_colors.color_text };
        if (is_close_btn && (btn.hover || btn.pressed))
            front_color = RGB(255, 255, 255);
        m_draw.DrawWindowText(rect, text, front_color, Alignment::CENTER, true);
    }
    else
    {
        m_draw.DrawWindowText(rect, text, GRAY(200), Alignment::CENTER);
    }
    btn.rect = rect;
}

void CPlayerUIBase::AddMouseToolTip(int btn, LPCTSTR str)
{
    CRect rect;
    auto iter = m_buttons.find(static_cast<BtnKey>(btn));
    if (iter != m_buttons.end())
        rect = iter->second.rect;
    m_tool_tip.AddTool(m_pMainWnd, str, rect, btn + GetToolTipIdOffset());
}

void CPlayerUIBase::UpdateMouseToolTip(int btn, LPCTSTR str)
{
    if (m_tool_tip.m_hWnd != NULL)
        m_tool_tip.UpdateTipText(str, m_pMainWnd, btn + GetToolTipIdOffset());
}

void CPlayerUIBase::UpdateMouseToolTipPosition(int btn, CRect rect)
{
    if (m_tool_tip.m_hWnd != NULL)
        m_tool_tip.SetToolRect(m_pMainWnd, btn + GetToolTipIdOffset(), rect);
}

void CPlayerUIBase::HideTooltip()
{
    for (const auto& btn : m_buttons)
        UpdateMouseToolTipPosition(btn.first, CRect());
}

void CPlayerUIBase::UpdateVolumeToolTip()
{
    if (m_tool_tip.m_hWnd != NULL)
        m_tool_tip.UpdateTipText(GetVolumeTooltipString(), m_pMainWnd, BTN_VOLUME + GetToolTipIdOffset());
}

void CPlayerUIBase::UpdatePlaylistBtnToolTip()
{
    wstring tool_tip = theApp.m_str_table.LoadText(L"UI_TIP_BTN_PLAYLIST_SHOW_HIDE");
    if (theApp.m_media_lib_setting_data.playlist_btn_for_float_playlist)
        tool_tip += GetCmdShortcutKeyForTooltips(ID_FLOAT_PLAYLIST).GetString();
    else
        tool_tip += GetCmdShortcutKeyForTooltips(ID_SHOW_PLAYLIST).GetString();
    UpdateMouseToolTip(BTN_SHOW_PLAYLIST, tool_tip.c_str());
}

void CPlayerUIBase::UpdateDarkLightModeBtnToolTip()
{
    wstring tip_str;
    if (theApp.m_app_setting_data.dark_mode)
        tip_str = theApp.m_str_table.LoadText(L"UI_TIP_BTN_DARK_LIGHT_TO_LIGHT_MODE");
    else
        tip_str = theApp.m_str_table.LoadText(L"UI_TIP_BTN_DARK_LIGHT_TO_DARK_MODE");
    tip_str += GetCmdShortcutKeyForTooltips(ID_DARK_MODE).GetString();
    UpdateMouseToolTip(BTN_DARK_LIGHT, tip_str.c_str());
    UpdateMouseToolTip(BTN_DARK_LIGHT_TITLE_BAR, tip_str.c_str());
}

void CPlayerUIBase::ShowUiTipInfo(const std::wstring& info)
{
    m_show_ui_tip_info = true;
    m_ui_tip_info = info;
    KillTimer(theApp.m_pMainWnd->GetSafeHwnd(), UI_TIP_INFO_TIMER_ID);

    //设置一个定时器，5秒后m_show_ui_tip_info变为false
    SetTimer(theApp.m_pMainWnd->GetSafeHwnd(), UI_TIP_INFO_TIMER_ID, 5000, [](HWND Arg1, UINT Arg2, UINT_PTR Arg3, DWORD Arg4) {
        KillTimer(theApp.m_pMainWnd->GetSafeHwnd(), UI_TIP_INFO_TIMER_ID);
        m_show_ui_tip_info = false;
    });
}

void CPlayerUIBase::SetRepeatModeToolTipText()
{
    wstring mode_str;
    auto repeat_mode = CPlayer::GetInstance().GetRepeatMode();
    switch (repeat_mode)
    {
    case RM_PLAY_ORDER: mode_str = theApp.m_str_table.LoadText(L"UI_TIP_REPEAT_ORDER"); break;
    case RM_PLAY_SHUFFLE:  mode_str = theApp.m_str_table.LoadText(L"UI_TIP_REPEAT_SHUFFLE"); break;
    case RM_PLAY_RANDOM: mode_str = theApp.m_str_table.LoadText(L"UI_TIP_REPEAT_RANDOM"); break;
    case RM_LOOP_PLAYLIST: mode_str = theApp.m_str_table.LoadText(L"UI_TIP_REPEAT_PLAYLIST"); break;
    case RM_LOOP_TRACK: mode_str = theApp.m_str_table.LoadText(L"UI_TIP_REPEAT_TRACK"); break;
    case RM_PLAY_TRACK: mode_str = theApp.m_str_table.LoadText(L"UI_TIP_REPEAT_ONCE"); break;
    default: mode_str = L"<repeat mode error>";
    }
    m_repeat_mode_tip = theApp.m_str_table.LoadText(L"UI_TIP_REPEAT_MODE") + L" (M): " + mode_str;
}

void CPlayerUIBase::SetSongInfoToolTipText()
{
    const SongInfo& songInfo = CPlayer::GetInstance().GetCurrentSongInfo();

    m_info_tip = theApp.m_str_table.LoadText(L"UI_TIP_BTN_PROPERTY") + GetCmdShortcutKeyForTooltips(ID_SONG_INFO).GetString() + L"\r\n";
    m_info_tip += theApp.m_str_table.LoadText(L"TXT_TITLE") + L": " + songInfo.GetTitle() + L"\r\n";
    m_info_tip += theApp.m_str_table.LoadText(L"TXT_ARTIST") + L": " + songInfo.GetArtist() + L"\r\n";
    m_info_tip += theApp.m_str_table.LoadText(L"TXT_ALBUM") + L": " + songInfo.GetAlbum();
}

int CPlayerUIBase::Margin() const
{
    int margin = m_layout.margin;
    if (m_ui_data.full_screen && IsDrawLargeIcon())
        margin = static_cast<int>(margin * CONSTVAL::FULL_SCREEN_ZOOM_FACTOR * 1.5);

    return margin;
}

int CPlayerUIBase::EdgeMargin(bool x) const
{
    /*全屏时界面外侧边距的计算
    边距 = 屏幕像素值 x 屏幕宽度的英寸值 / 常数
    此计算方法可以确保边距占屏幕宽度的比例与屏幕宽度的英寸值成正比，即界面看起来越空旷，外侧边距就越大
    最后取以上值和theApp.DPI(40)两者中较大的值。
    */
    if (m_ui_data.full_screen && IsDrawLargeIcon())
    {
        int draw_size = (x ? m_draw_rect.Width() : m_draw_rect.Height());
        int margin = draw_size * draw_size / theApp.GetDPI() / 300;
        return max(theApp.DPI(40), margin);
    }
    else
        return m_layout.margin;
}

int CPlayerUIBase::WidthThreshold() const
{
    int width = m_layout.width_threshold;
    if (m_ui_data.full_screen)
        width = static_cast<int>(width * CONSTVAL::FULL_SCREEN_ZOOM_FACTOR);

    return width;
}

int CPlayerUIBase::DrawAreaHeight() const
{
    int info_height = m_layout.info_height;
    if (m_ui_data.full_screen)
        info_height = static_cast<int>(info_height * CONSTVAL::FULL_SCREEN_ZOOM_FACTOR) + 2 * EdgeMargin();
    else if (!theApp.m_app_setting_data.show_window_frame)
        info_height += m_layout.titlabar_height;

    return info_height;
}

int CPlayerUIBase::TopRightButtonsWidth() const
{
    return m_top_right_buttons_width;
}

bool CPlayerUIBase::PointInControlArea(CPoint point) const
{
    bool point_in_control = false;
    for (const auto& btn : m_buttons)
    {
        point_in_control |= (btn.second.rect.PtInRect(point) != FALSE);
    }
    return point_in_control;
}

bool CPlayerUIBase::PointInTitlebarArea(CPoint point) const
{
    if (!IsDrawTitleBar())
        return false;

    CRect rect_titlebar = m_draw_rect;
    rect_titlebar.bottom = rect_titlebar.top + m_layout.titlabar_height;
    return (rect_titlebar.PtInRect(point) != FALSE);
}

bool CPlayerUIBase::PointInAppIconArea(CPoint point) const
{
    if (!IsDrawTitleBar())
        return false;

    return (m_app_icon_rect.PtInRect(point) != FALSE);
}

bool CPlayerUIBase::PointInMenubarArea(CPoint point) const
{
    if (!IsDrawMenuBar())
        return false;

    CRect rect_menubar = m_draw_rect;
    rect_menubar.top = m_draw_rect.top + m_layout.titlabar_height;
    rect_menubar.bottom = rect_menubar.top + m_layout.menubar_height;
    return (rect_menubar.PtInRect(point) != FALSE);

}

bool CPlayerUIBase::IsDrawBackgroundAlpha() const
{
    return theApp.m_app_setting_data.enable_background && (CPlayer::GetInstance().AlbumCoverExist() || !m_ui_data.default_background.IsNull());
}

bool CPlayerUIBase::IsDrawStatusBar() const
{
    return CPlayerUIHelper::IsDrawStatusBar();
}

bool CPlayerUIBase::IsDrawTitleBar() const
{
    return !theApp.m_app_setting_data.show_window_frame && !m_ui_data.full_screen;
}

bool CPlayerUIBase::IsDrawMenuBar() const
{
    return m_ui_data.ShowUiMenuBar();
}

wstring CPlayerUIBase::GetDisplayFormatString()
{
    wstring result;
    int chans = CPlayer::GetInstance().GetChannels();
    int freq = CPlayer::GetInstance().GetFreq();
    wstring chans_str = CSongInfoHelper::GetChannelsString(static_cast<BYTE>(chans));
    wchar_t buff[64];
    if (!CPlayer::GetInstance().IsMidi())
        swprintf_s(buff, L"%s %.1fkHz %dkbps %s", CPlayer::GetInstance().GetCurrentFileType().c_str(), freq / 1000.0f, CPlayer::GetInstance().GetSafeCurrentSongInfo().bitrate, chans_str.c_str());
    else
        swprintf_s(buff, L"%s %.1fkHz %s", CPlayer::GetInstance().GetCurrentFileType().c_str(), freq / 1000.0f, chans_str.c_str());
    result = buff;
    if (CPlayer::GetInstance().IsMidi())
    {
        const MidiInfo& midi_info{ CPlayer::GetInstance().GetMidiInfo() };
        swprintf_s(buff, L" %dbpm (%d/%d)", midi_info.speed, midi_info.midi_position, midi_info.midi_length);
        result += buff;
    }
    return result;

}

CString CPlayerUIBase::GetVolumeTooltipString()
{
    static const wstring& mute_str = theApp.m_str_table.LoadText(L"UI_TXT_VOLUME_MUTE");
    static const wstring& volume_adjust = theApp.m_str_table.LoadText(L"UI_TIP_VOLUME_MOUSE_WHEEL_ADJUST");
    wstring tooltip;
    if (CPlayer::GetInstance().GetVolume() <= 0)
        tooltip = theApp.m_str_table.LoadTextFormat(L"UI_TXT_VOLUME", { mute_str, L"" });
    else
        tooltip = theApp.m_str_table.LoadTextFormat(L"UI_TXT_VOLUME", { CPlayer::GetInstance().GetVolume(), L"%" });
    tooltip += L"\r\n" + volume_adjust;
    return CString(tooltip.c_str());
}

int CPlayerUIBase::DPI(int pixel) const
{
    if (m_ui_data.full_screen && IsDrawLargeIcon())
        return static_cast<int>(theApp.DPI(pixel) * CONSTVAL::FULL_SCREEN_ZOOM_FACTOR);
    else
        return theApp.DPI(pixel);
}

int CPlayerUIBase::DPI(double pixel) const
{
    if (m_ui_data.full_screen && IsDrawLargeIcon())
        return static_cast<int>(theApp.DPI(pixel) * CONSTVAL::FULL_SCREEN_ZOOM_FACTOR);
    else
        return theApp.DPI(pixel);
}

double CPlayerUIBase::DPIDouble(double pixel)
{
    double rtn_val = static_cast<double>(theApp.GetDPI()) * pixel / 96;
    if (m_ui_data.full_screen && IsDrawLargeIcon())
        rtn_val *= CONSTVAL::FULL_SCREEN_ZOOM_FACTOR;
    return rtn_val;
}

double CPlayerUIBase::GetScrollTextPixel(bool slower)
{
    //界面刷新频率越高，即界面刷新时间间隔越小，则每次滚动的像素值就要越小
    double pixel{};
    if (slower)
        pixel = static_cast<double>(theApp.m_app_setting_data.ui_refresh_interval) * 0.01 + 0.2;
    else
        pixel = static_cast<double>(theApp.m_app_setting_data.ui_refresh_interval) * 0.0125 + 0.2;
    pixel = DPIDouble(pixel);
    if (pixel < 0.1)
        pixel = 0.1;
    if (pixel > 0.5 && pixel < 1)
        pixel = 1;
    return pixel;
}

int CPlayerUIBase::CalculateRoundRectRadius(const CRect& rect)
{
    int radius{ min(rect.Width(), rect.Height()) / 6 };
    if (radius < DPI(3))
        radius = DPI(3);
    if (radius > DPI(8))
        radius = DPI(8);
    return radius;
}

bool CPlayerUIBase::IsDrawLargeIcon() const
{
    return theApp.m_ui_data.full_screen;
}

bool CPlayerUIBase::IsMiniMode() const
{
    return dynamic_cast<const CMiniModeUserUi*>(this) != nullptr;
}

void CPlayerUIBase::DrawVolumnAdjBtn()
{
    if (m_show_volume_adj)
    {
        CRect& volume_down_rect = m_buttons[BTN_VOLUME_DOWN].rect;
        CRect& volume_up_rect = m_buttons[BTN_VOLUME_UP].rect;

        //判断音量调整按钮是否会超出界面之外，如果是，则将其移动至界面内
        int x_offset{}, y_offset{};     //移动的x和y偏移量
        CRect rect_text;                //音量文本的区域
        CString volume_str{};
        if (!m_show_volume_text)
        {
            //如果不显示音量文本，则在音量调整按钮旁边显示音量。在这里计算文本的位置
            rect_text = m_buttons[BTN_VOLUME_UP].rect;
            if (CPlayer::GetInstance().GetVolume() <= 0)
                volume_str = theApp.m_str_table.LoadText(L"UI_TXT_VOLUME_MUTE").c_str();
            else
                volume_str.Format(_T("%d%%"), CPlayer::GetInstance().GetVolume());
            int width{ m_draw.GetTextExtent(volume_str).cx };
            rect_text.left = rect_text.right + DPI(2);
            rect_text.right = rect_text.left + width;

            if (rect_text.right > m_draw_rect.right)
                x_offset = m_draw_rect.right - rect_text.right;
            if (rect_text.bottom > m_draw_rect.bottom)
                y_offset = m_draw_rect.bottom - rect_text.bottom;
        }
        else
        {
            if (volume_up_rect.right > m_draw_rect.right)
                x_offset = m_draw_rect.right - volume_up_rect.right;
            if (volume_up_rect.bottom > m_draw_rect.bottom)
                y_offset = m_draw_rect.bottom - volume_up_rect.bottom;
        }

        if (x_offset != 0)
        {
            volume_up_rect.MoveToX(volume_up_rect.left + x_offset);
            volume_down_rect.MoveToX(volume_down_rect.left + x_offset);
            rect_text.MoveToX(rect_text.left + x_offset);
        }
        if (y_offset != 0)
        {
            volume_up_rect.MoveToY(volume_up_rect.top + y_offset);
            volume_down_rect.MoveToY(volume_down_rect.top + y_offset);
            rect_text.MoveToY(rect_text.top + y_offset);
        }

        BYTE alpha;
        if (IsDrawBackgroundAlpha())
            alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency);
        else
            alpha = 255;

        COLORREF btn_up_back_color, btn_down_back_color;

        if (m_buttons[BTN_VOLUME_UP].pressed && m_buttons[BTN_VOLUME_UP].hover)
            btn_up_back_color = m_colors.color_button_pressed;
        else if (m_buttons[BTN_VOLUME_UP].hover)
            btn_up_back_color = m_colors.color_button_hover;
        else
            btn_up_back_color = m_colors.color_text_2;

        if (m_buttons[BTN_VOLUME_DOWN].pressed && m_buttons[BTN_VOLUME_DOWN].hover)
            btn_down_back_color = m_colors.color_button_pressed;
        else if (m_buttons[BTN_VOLUME_DOWN].hover)
            btn_down_back_color = m_colors.color_button_hover;
        else
            btn_down_back_color = m_colors.color_text_2;

        if (!theApp.m_app_setting_data.button_round_corners)
        {
            m_draw.FillAlphaRect(volume_up_rect, btn_up_back_color, alpha);
            m_draw.FillAlphaRect(volume_down_rect, btn_down_back_color, alpha);
        }
        else
        {
            CRect rc_buttons{ volume_up_rect | volume_down_rect };
            DrawAreaGuard guard(&m_draw, rc_buttons);
            m_draw.DrawRoundRect(rc_buttons, m_colors.color_text_2, CalculateRoundRectRadius(rc_buttons), alpha);
            if (m_buttons[BTN_VOLUME_UP].pressed || m_buttons[BTN_VOLUME_UP].hover)
                m_draw.DrawRoundRect(volume_up_rect, btn_up_back_color, CalculateRoundRectRadius(volume_up_rect), alpha);
            if (m_buttons[BTN_VOLUME_DOWN].pressed || m_buttons[BTN_VOLUME_DOWN].hover)
                m_draw.DrawRoundRect(volume_down_rect, btn_down_back_color, CalculateRoundRectRadius(volume_down_rect), alpha);
        }

        if (m_buttons[BTN_VOLUME_DOWN].pressed)
            volume_down_rect.MoveToXY(volume_down_rect.left + theApp.DPI(1), volume_down_rect.top + theApp.DPI(1));
        if (m_buttons[BTN_VOLUME_UP].pressed)
            volume_up_rect.MoveToXY(volume_up_rect.left + theApp.DPI(1), volume_up_rect.top + theApp.DPI(1));

        m_draw.DrawWindowText(volume_down_rect, L"-", ColorTable::WHITE, Alignment::CENTER);
        m_draw.DrawWindowText(volume_up_rect, L"+", ColorTable::WHITE, Alignment::CENTER);

        //如果不显示音量文本且显示了音量调整按钮，则在按钮旁边显示音量
        if (!m_show_volume_text)
        {
            m_draw.DrawWindowText(rect_text, volume_str, m_colors.color_text);
        }
    }
    else
    {
        m_buttons[BTN_VOLUME_UP].rect = CRect();
        m_buttons[BTN_VOLUME_DOWN].rect = CRect();
    }
}

CRect CPlayerUIBase::DrawProgressBar(CRect rect, bool play_time_both_side)
{
    //绘制播放时间
    bool draw_progress_time{ rect.Width() > DPI(110) };
    CRect progress_rect{ rect };
    if (draw_progress_time)
    {
        UiFontGuard set_font(this, 8);
        if (play_time_both_side)
        {
            CRect rc_time_left{ rect }, rc_time_right{ rect };
            std::wstring str_cur_time = CPlayTime(CPlayer::GetInstance().GetCurrentPosition()).toString(false);
            std::wstring str_song_length = CPlayTime(CPlayer::GetInstance().GetSongLength()).toString(false);
            int left_width = m_draw.GetTextExtent(str_cur_time.c_str()).cx;
            int right_width = m_draw.GetTextExtent(str_song_length.c_str()).cx;
            rc_time_left.right = rc_time_left.left + left_width;
            rc_time_right.left = rc_time_right.right - right_width;
            m_draw.DrawWindowText(rc_time_left, str_cur_time.c_str(), m_colors.color_text);
            m_draw.DrawWindowText(rc_time_right, str_song_length.c_str(), m_colors.color_text);
            progress_rect.left += (left_width + Margin());
            progress_rect.right -= (right_width + Margin());
        }
        else
        {
            CRect rc_time = rect;
            wstring strTime = CPlayer::GetInstance().GetTimeString();

            CSize strSize = m_draw.GetTextExtent(strTime.c_str());
            rc_time.left = rc_time.right - strSize.cx;
            //rc_time.InflateRect(0, DPI(2));
            rc_time.top -= DPI(1);
            m_draw.DrawWindowText(rc_time, strTime.c_str(), m_colors.color_text);
            progress_rect.right = rc_time.left - Margin();
        }
    }

    //绘制进度条
    return DrawProgess(progress_rect);
}

CRect CPlayerUIBase::DrawProgess(CRect rect)
{
    //进度条的高度
    int progress_height = min(DPI(4), rect.Height());
    rect.top = rect.top + (rect.Height() - progress_height) / 2;
    rect.bottom = rect.top + progress_height;

    if (IsDrawBackgroundAlpha())
        m_draw.FillAlphaRect(rect, m_colors.color_progress_back, ALPHA_CHG(theApp.m_app_setting_data.background_transparency) * 2 / 3);
    else
        m_draw.FillRect(rect, m_colors.color_progress_back);

    double progress = static_cast<double>(CPlayer::GetInstance().GetCurrentPosition()) / CPlayer::GetInstance().GetSongLength();
    if (progress > 1)
        progress = 1;
    double progress_width_double{ progress * rect.Width() };
    int progress_width{ static_cast<int>(progress_width_double) };
    CRect played_rect = rect;
    played_rect.right = played_rect.left + progress_width;
    if (played_rect.right > played_rect.left)
        m_draw.FillRect(played_rect, m_colors.color_spectrum);
    //绘制进度条最右侧一像素
    //进度条最右侧一像素根据当前进度计算出透明度，以使得进度条的变化更加平滑
    BYTE alpha{ static_cast<BYTE>((progress_width_double - progress_width) * 256) };
    played_rect.left = played_rect.right;
    played_rect.right = played_rect.left + 1;
    m_draw.FillAlphaRect(played_rect, m_colors.color_spectrum, alpha);

    //绘制AB重复的标记
    auto ab_repeat_mode = CPlayer::GetInstance().GetABRepeatMode();
    if (ab_repeat_mode == CPlayer::AM_A_SELECTED || ab_repeat_mode == CPlayer::AM_AB_REPEAT)
    {
        CFont* pOldFont = m_draw.GetFont();
        //设置字体
        m_draw.SetFont(&theApp.m_font_set.GetFontBySize(8).GetFont(theApp.m_ui_data.full_screen));      //AB重复使用小一号字体，即播放时间的字体

        double a_point_progres = static_cast<double>(CPlayer::GetInstance().GetARepeatPosition().toInt()) / CPlayer::GetInstance().GetSongLength();
        double b_point_progres = static_cast<double>(CPlayer::GetInstance().GetBRepeatPosition().toInt()) / CPlayer::GetInstance().GetSongLength();
        CRect rect_draw = rect;
        rect_draw.bottom += DPI(12);
        DrawAreaGuard guard(&m_draw, rect_draw);
        CPoint point1, point2;
        //绘制A点标记
        point1.x = point2.x = rect.left + static_cast<int>(a_point_progres * rect.Width());
        point1.y = rect.top - DPI(2);
        point2.y = rect.bottom + DPI(2);
        m_draw.DrawLine(point1, point2, m_colors.color_text, DPI(1), false);
        CRect rect_text;
        rect_text.top = point2.y;
        rect_text.left = point1.x - DPI(8);
        rect_text.right = point1.x + DPI(8);
        rect_text.bottom = rect_text.top + DPI(12);
        m_draw.DrawWindowText(rect_text, _T("A"), m_colors.color_text, Alignment::CENTER, true);
        //绘制B点标记
        if (ab_repeat_mode == CPlayer::AM_AB_REPEAT)
        {
            point1.x = point2.x = rect.left + static_cast<int>(b_point_progres * rect.Width());
            m_draw.DrawLine(point1, point2, m_colors.color_text, DPI(1), false);
            rect_text.MoveToX(point1.x - DPI(8));
            m_draw.DrawWindowText(rect_text, _T("B"), m_colors.color_text, Alignment::CENTER, true);
        }
        //恢复字体
        m_draw.SetFont(pOldFont);
    }

    //进度条能响应鼠标消息的区域（上下各增加3像素）
    CRect progress_rect = rect;
    progress_rect.InflateRect(0, DPI(3));
    return progress_rect;
}

void CPlayerUIBase::DrawTopRightIcons()
{
    if (!theApp.m_app_setting_data.show_window_frame && !m_ui_data.full_screen)
        return;

    const int icon_size = DPI(28);
    CRect rect_btn;
    rect_btn.right = m_draw_rect.right - EdgeMargin(true);
    rect_btn.top = EdgeMargin(false);
    rect_btn.bottom = rect_btn.top + icon_size;
    rect_btn.left = rect_btn.right - icon_size;
    int icon_num{};
    //绘制“全屏”图标（全屏模式下总是显示“退出全屏”图标）
    if (theApp.m_app_setting_data.show_fullscreen_btn_in_titlebar || m_ui_data.full_screen)
    {
        DrawUIButton(rect_btn, BTN_FULL_SCREEN_TITLEBAR);
        icon_num++;
    }
    else
    {
        m_buttons[BTN_FULL_SCREEN_TITLEBAR].rect.SetRectEmpty();
        rect_btn.MoveToX(rect_btn.left + icon_size + DPI(4));   //右侧第一个图标不显示，则将按钮的位置向右移动一个图标的位置
    }

    //迷你模式图标
    if (theApp.m_app_setting_data.show_minimode_btn_in_titlebar)
    {
        rect_btn.MoveToX(rect_btn.left - icon_size - DPI(4));
        DrawUIButton(rect_btn, BTN_MINI_TITLEBAR);
        icon_num++;
    }
    else
    {
        m_buttons[BTN_MINI_TITLEBAR].rect.SetRectEmpty();
    }
    //“深色模式/浅色模式”按钮
    if (theApp.m_app_setting_data.show_dark_light_btn_in_titlebar)
    {
        rect_btn.MoveToX(rect_btn.left - icon_size - DPI(4));
        DrawUIButton(rect_btn, BTN_DARK_LIGHT_TITLE_BAR);
        icon_num++;
    }
    else
    {
        m_buttons[BTN_DARK_LIGHT_TITLE_BAR].rect.SetRectEmpty();
    }
    
    //切换界面图标
    if (theApp.m_app_setting_data.show_skin_btn_in_titlebar)
    {
        rect_btn.MoveToX(rect_btn.left - icon_size - DPI(4));
        DrawUIButton(rect_btn, BTN_SKIN_TITLEBAR);
        icon_num++;
    }
    else
    {
        m_buttons[BTN_SKIN_TITLEBAR].rect.SetRectEmpty();
    }
    //设置图标
    if (theApp.m_app_setting_data.show_settings_btn_in_titlebar)
    {
        rect_btn.MoveToX(rect_btn.left - icon_size - DPI(4));
        DrawUIButton(rect_btn, BTN_SETTING_TITLEBAR);
        icon_num++;
    }
    else
    {
        m_buttons[BTN_SETTING_TITLEBAR].rect.SetRectEmpty();
    }
    //绘制“主菜单”图标
    if (!m_ui_data.show_menu_bar || m_ui_data.full_screen)
    {
        rect_btn.MoveToX(rect_btn.left - icon_size - DPI(4));
        DrawUIButton(rect_btn, BTN_MENU_TITLEBAR);
        icon_num++;
    }
    else
    {
        m_buttons[BTN_MENU_TITLEBAR].rect.SetRectEmpty();
    }
    m_top_right_buttons_width = icon_num * (icon_size + DPI(4));

    //绘制左上角返回图标
    if (IsDrawTitlebarLeftBtn())
    {
        rect_btn.MoveToXY(0, 0);
        DrawUIButton(rect_btn, BTN_CLOSE_PANEL_TITLE_BAR);
    }
    else
    {
        m_buttons[BTN_CLOSE_PANEL_TITLE_BAR].rect = CRect();
    }

}

void CPlayerUIBase::DrawCurrentTime()
{
    wchar_t buff[64];
    CRect rc_tmp;
    SYSTEMTIME curTime;
    GetLocalTime(&curTime);
    std::swprintf(buff, 64, L"%d:%.2d", curTime.wHour, curTime.wMinute);
    CSize size = m_draw.GetTextExtent(buff);
    rc_tmp.top = theApp.DPI(4);
    rc_tmp.right = m_draw_rect.right - theApp.DPI(4);
    rc_tmp.bottom = rc_tmp.top + size.cy;
    rc_tmp.left = rc_tmp.right - size.cx;
    m_draw.SetFont(&theApp.m_font_set.GetFontBySize(8).GetFont(m_ui_data.full_screen));
    m_draw.DrawWindowText(rc_tmp, buff, m_colors.color_text);
    m_draw.SetFont(&theApp.m_font_set.GetFontBySize(9).GetFont(theApp.m_ui_data.full_screen));
}

void CPlayerUIBase::DrawStatusBar(CRect rect, bool reset)
{
    bool draw_background{ IsDrawBackgroundAlpha() };
    //绘制背景
    BYTE alpha;
    if (theApp.m_app_setting_data.dark_mode)
        alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency) / 2;
    else
        alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency) * 2 / 3;

    if (draw_background)
        m_draw.FillAlphaRect(rect, m_colors.color_control_bar_back, alpha);
    else
        m_draw.FillRect(rect, m_colors.color_control_bar_back);

    //显示帧率
    if (theApp.m_app_setting_data.show_fps)
    {
        CRect rc_fps{ rect };
        rc_fps.right = rect.right - DPI(4);
        rc_fps.left = rc_fps.right - DPI(40);
        CFont* pOldFont = m_draw.SetFont(&theApp.m_font_set.GetFontBySize(8).GetFont(theApp.m_ui_data.full_screen));
        CString str_info;
        str_info.Format(_T("%dFPS"), theApp.m_fps);
        m_draw.DrawWindowText(rc_fps, str_info, m_colors.color_text, Alignment::RIGHT);
        m_draw.SetFont(pOldFont);

        rect.right = rc_fps.left;
    }

    rect.DeflateRect(DPI(4), 0);

    //显示播放列表载入状态
    if (CPlayer::GetInstance().m_loading)
    {
        int progress_percent;
        if (CPlayer::GetInstance().GetThreadInfo().merge_same_songs)
            progress_percent = CPlayer::GetInstance().GetThreadInfo().merge_same_songs_percent;
        else
            progress_percent = CPlayer::GetInstance().GetThreadInfo().process_percent;
        //绘制进度右侧的进度百分比
        CRect rc_percent{ rect };
        rc_percent.left = rc_percent.right - DPI(32);
        CFont* pOldFont = m_draw.SetFont(&theApp.m_font_set.GetFontBySize(8).GetFont(theApp.m_ui_data.full_screen));
        CString str_info;
        str_info.Format(_T("%d%%"), progress_percent);
        m_draw.DrawWindowText(rc_percent, str_info, m_colors.color_text);
        m_draw.SetFont(pOldFont);

        rect.right = rc_percent.left - DPI(4);
        CRect rc_tmp{ rect };
        //绘制进度条（进度条里面包含10格）
        int bar_width = DPI(4);     //每一格的宽度
        int progress_width = (bar_width + DPI(2)) * 10 + DPI(2) * 2;
        rc_tmp.left = rect.right - progress_width;
        CRect rc_progress{ rc_tmp };
        rc_progress.DeflateRect(0, DPI(4));
        DrawAreaGuard guard(&m_draw, rc_progress);
        //m_draw.DrawRectOutLine(rc_progress, m_colors.color_text, DPI(1), false);
        DrawRectangle(rc_progress, m_colors.color_statusbar_progress_back);
        int bar_cnt = progress_percent / 10 + 1;        //格子数
        int last_bar_percent = progress_percent % 10;
        CRect rc_bar{ rc_progress };
        rc_bar.DeflateRect(DPI(2), DPI(2));
        rc_bar.right = rc_bar.left + bar_width;
        int start_x_pos = rc_bar.left;
        for (int i = 0; i < bar_cnt; i++)
        {
            rc_bar.MoveToX(start_x_pos + i * (bar_width + DPI(2)));
            if (i != bar_cnt - 1)
            {
                m_draw.FillRect(rc_bar, m_colors.color_text, true);
            }
            else
            {
                BYTE alpha = ALPHA_CHG(last_bar_percent * 10);
                m_draw.FillAlphaRect(rc_bar, m_colors.color_text, alpha, true);
            }
        }

        //绘制文字
        rc_tmp.right = rc_tmp.left - DPI(4);
        rc_tmp.left = rect.left;
        if (CPlayer::GetInstance().GetThreadInfo().merge_same_songs)
        {
            //绘制文字
            static CDrawCommon::ScrollInfo scroll_info1;
            wstring info = theApp.m_str_table.LoadText(L"UI_TXT_PLAYLIST_MERGING_SAME_SONG_INFO");
            m_draw.DrawScrollText(rect, info.c_str(), m_colors.color_text, GetScrollTextPixel(), false, scroll_info1, reset);
        }
        else
        {
            static CDrawCommon::ScrollInfo scroll_info0;
            wstring info = theApp.m_str_table.LoadTextFormat(L"UI_TXT_PLAYLIST_INIT_INFO", { CPlayer::GetInstance().GetSongNum() });
            m_draw.DrawScrollText(rc_tmp, info.c_str(), m_colors.color_text, GetScrollTextPixel(), false, scroll_info0, reset);
        }
    }
    //显示AB重复状态
    else if (CPlayer::GetInstance().GetABRepeatMode() != CPlayer::AM_NONE)
    {
        wstring info;
        if (CPlayer::GetInstance().GetABRepeatMode() == CPlayer::AM_A_SELECTED)
            info = theApp.m_str_table.LoadTextFormat(L"UI_TXT_AB_REPEAT_A_SELECTED", { CPlayer::GetInstance().GetARepeatPosition().toString(false) });
        else if (CPlayer::GetInstance().GetABRepeatMode() == CPlayer::AM_AB_REPEAT)
            info = theApp.m_str_table.LoadTextFormat(L"UI_TXT_AB_REPEAT_ON", { CPlayer::GetInstance().GetARepeatPosition().toString(false), CPlayer::GetInstance().GetBRepeatPosition().toString(false) });
        m_draw.DrawWindowText(rect, info.c_str(), m_colors.color_text);
    }
    //显示媒体库更新状态
    else if (theApp.IsMeidaLibUpdating() && theApp.m_media_update_para.num_added > 0)
    {
        wstring info = theApp.m_str_table.LoadTextFormat(L"UI_TXT_MEDIA_LIB_UPDATING_INFO",
            { theApp.m_media_update_para.num_added, theApp.m_media_update_para.process_percent });
        static CDrawCommon::ScrollInfo scroll_info2;
        m_draw.DrawScrollText(rect, info.c_str(), m_colors.color_text, GetScrollTextPixel(), false, scroll_info2, reset);
    }
    else
    {
        //显示播放信息
        wstring str_info;
        if (CPlayer::GetInstance().IsError())
            str_info = CPlayer::GetInstance().GetErrorInfo();
        else
            str_info = CPlayer::GetInstance().GetPlayingState();
        int text_width = m_draw.GetTextExtent(str_info.c_str()).cx;
        CRect rect_play{ rect };
        rect_play.right = rect_play.left + text_width + DPI(4);
        m_draw.DrawWindowText(rect_play, str_info.c_str(), m_colors.color_text);

        //绘制播放速度、播放变调
        DrawPlayEffectTag(rect, rect_play);
        rect_play.right += DPI(8);

        //显示下一个播放曲目
        if (theApp.m_app_setting_data.show_next_track)
        {
            static wstring str_next_track_label = theApp.m_str_table.LoadText(L"UI_TXT_NEXT_TRACK");
            CRect rect_next_track{ rect };
            rect_next_track.left = rect_play.right;
            rect_next_track.right = rect_next_track.left + m_draw.GetTextExtent(str_next_track_label.c_str()).cx;
            m_draw.DrawWindowText(rect_next_track, str_next_track_label.c_str(), m_colors.color_text);

            rect_next_track.left = rect_next_track.right;
            rect_next_track.right = rect.right;
            wstring str_next_song;
            SongInfo next_song = CPlayer::GetInstance().GetNextTrack();
            if (next_song.IsEmpty())
            {
                if (CPlayer::GetInstance().GetRepeatMode() == RM_PLAY_RANDOM || CPlayer::GetInstance().GetRepeatMode() == RM_PLAY_SHUFFLE)
                    str_next_song += theApp.m_str_table.LoadText(L"UI_TXT_NEXT_TRACK_RANDOM");
                else
                    str_next_song += theApp.m_str_table.LoadText(L"UI_TXT_NEXT_TRACK_NONE");
            }
            else
            {
                str_next_song += CSongInfoHelper::GetDisplayStr(next_song, theApp.m_media_lib_setting_data.display_format);
            }
            static CDrawCommon::ScrollInfo scroll_info3;
            m_draw.DrawScrollText(rect_next_track, str_next_song.c_str(), m_colors.color_text_lable, GetScrollTextPixel(), false, scroll_info3, reset);

        }
    }
}

void CPlayerUIBase::DrawTitleBar(CRect rect)
{
    //填充标题栏背景
    bool draw_background{ IsDrawBackgroundAlpha() };
    //绘制背景
    BYTE alpha;
    if (theApp.m_app_setting_data.dark_mode)
        alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency) / 2;
    else
        alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency) * 2 / 3;

    if (draw_background)
        m_draw.FillAlphaRect(rect, m_colors.color_control_bar_back, alpha);
    else
        m_draw.FillRect(rect, m_colors.color_control_bar_back);

    CRect rect_temp = rect;
    //绘制左侧图标
    if (IsDrawTitlebarLeftBtn())
    {
        rect_temp.right = rect_temp.left + m_layout.titlabar_height;
        DrawUIButton(rect_temp, BTN_CLOSE_PANEL_TITLE_BAR);
        rect_temp.left = rect_temp.right;
    }
    else
    {
        m_buttons[BTN_CLOSE_PANEL_TITLE_BAR].rect = CRect();
    }

    //绘制应用图标
    rect_temp.right = rect_temp.left + m_layout.titlabar_height;
    DrawUiIcon(rect_temp, IconMgr::IconType::IT_App);
    m_app_icon_rect = rect_temp;

    int text_left_pos = rect_temp.right;

    //绘制右侧图标
    rect_temp = rect;
    rect_temp.left = rect_temp.right - theApp.DPI(30);
    //关闭图标
    DrawUIButton(rect_temp, BTN_APP_CLOSE);
    //最大化/还原图标
    if (theApp.m_app_setting_data.show_maximize_btn_in_titlebar)
    {
        rect_temp.MoveToX(rect_temp.left - rect_temp.Width());
        DrawUIButton(rect_temp, BTN_MAXIMIZE);
    }
    else
    {
        m_buttons[BTN_MAXIMIZE].rect = CRect();
    }
    //最小化图标
    if (theApp.m_app_setting_data.show_minimize_btn_in_titlebar)
    {
        rect_temp.MoveToX(rect_temp.left - rect_temp.Width());
        DrawUIButton(rect_temp, BTN_MINIMIZE);
    }
    else
    {
        m_buttons[BTN_MINIMIZE].rect = CRect();
    }
    //全屏模式图标
    if (theApp.m_app_setting_data.show_fullscreen_btn_in_titlebar)
    {
        rect_temp.MoveToX(rect_temp.left - rect_temp.Width());
        DrawUIButton(rect_temp, BTN_FULL_SCREEN_TITLEBAR);
    }
    else
    {
        m_buttons[BTN_FULL_SCREEN_TITLEBAR].rect = CRect();
    }
    //迷你模式图标
    if (theApp.m_app_setting_data.show_minimode_btn_in_titlebar)
    {
        rect_temp.MoveToX(rect_temp.left - rect_temp.Width());
        DrawUIButton(rect_temp, BTN_MINI_TITLEBAR);
    }
    else
    {
        m_buttons[BTN_MINI_TITLEBAR].rect = CRect();
    }
    //“深色模式/浅色模式”按钮
    if (theApp.m_app_setting_data.show_dark_light_btn_in_titlebar)
    {
        rect_temp.MoveToX(rect_temp.left - rect_temp.Width());
        DrawUIButton(rect_temp, BTN_DARK_LIGHT_TITLE_BAR);
    }
    else
    {
        m_buttons[BTN_DARK_LIGHT_TITLE_BAR].rect = CRect();
    }
    //切换界面图标
    if (theApp.m_app_setting_data.show_skin_btn_in_titlebar)
    {
        rect_temp.MoveToX(rect_temp.left - rect_temp.Width());
        DrawUIButton(rect_temp, BTN_SKIN_TITLEBAR);
    }
    else
    {
        m_buttons[BTN_SKIN_TITLEBAR].rect = CRect();
    }
    //设置图标
    if (theApp.m_app_setting_data.show_settings_btn_in_titlebar)
    {
        rect_temp.MoveToX(rect_temp.left - rect_temp.Width());
        DrawUIButton(rect_temp, BTN_SETTING_TITLEBAR);
    }
    else
    {
        m_buttons[BTN_SETTING_TITLEBAR].rect = CRect();
    }
    //主菜单图标
    if (!m_ui_data.ShowUiMenuBar())
    {
        rect_temp.MoveToX(rect_temp.left - rect_temp.Width());
        DrawUIButton(rect_temp, BTN_MENU_TITLEBAR);
    }
    else
    {
        m_buttons[BTN_MENU_TITLEBAR].rect = CRect();
    }

    //绘制标题栏文本
    rect_temp.right = rect_temp.left;
    rect_temp.left = text_left_pos;
    static CDrawCommon::ScrollInfo scroll_info{};
    m_draw.DrawScrollText(rect_temp, theApp.m_window_title.c_str(), m_colors.color_text, GetScrollTextPixel(), false, scroll_info);
    //m_draw.DrawWindowText(rect_temp, title.GetString(), m_colors.color_text);
}

int CPlayerUIBase::GetToolTipIdOffset()
{
    return GetUiIndex() * PlayerUiConstVal::BTN_MAX_NUM;
}

CString CPlayerUIBase::GetCmdShortcutKeyForTooltips(UINT id)
{
    std::wstring shortcut_key{ theApp.m_accelerator_res.GetShortcutDescriptionById(id) };
    if (!shortcut_key.empty())
    {
        CString shortcut_key_tip;
        shortcut_key_tip.Format(_T(" (%s)"), shortcut_key.c_str());
        return shortcut_key_tip;
    }
    return CString();
}

CRect CPlayerUIBase::GetVolumeRect() const
{
    auto iter = m_buttons.find(BTN_VOLUME);
    if (iter != m_buttons.end())
        return iter->second.rect;
    return CRect();
}

CRect CPlayerUIBase::GetDrawRect() const
{
    CRect draw_rect = m_draw_rect;
    if (m_ui_data.full_screen)
    {
        draw_rect.DeflateRect(EdgeMargin(true), EdgeMargin(false));
    }
    return draw_rect;
}

CRect CPlayerUIBase::GetClientDrawRect() const
{
    CRect client_rect = GetDrawRect();
    if (IsDrawTitleBar())
    {
        client_rect.top += m_layout.titlabar_height;
    }
    if (IsDrawMenuBar())
    {
        client_rect.top += m_layout.menubar_height;
    }
    if (IsDrawStatusBar())
    {
        client_rect.bottom -= DPI(20);
    }
    return client_rect;
}

CRect CPlayerUIBase::GetAppIconRect() const
{
    return m_app_icon_rect;
}

void CPlayerUIBase::ReplaceUiStringRes(wstring& str)
{
    size_t index{};
    while ((index = str.find(L"%(", index)) != wstring::npos)
    {
        size_t right_bracket_index = str.find(L')', index + 2);
        if (right_bracket_index == wstring::npos)
            break;
        wstring key_str{ str.begin() + index + 2 , str.begin() + right_bracket_index };
        //忽略字符串中的变量，避免被当作字符串表中无效的key
        if (!CPlayerFormulaHelper::IsValidVariable(key_str))
        {
            const wstring& value_str = theApp.m_str_table.LoadText(key_str);
            if (value_str == StrTable::error_str)   // LoadText内部已记录错误日志
                break;
            str.replace(index, right_bracket_index - index + 1, value_str);
        }
        index = right_bracket_index + 1;
    }
}

void CPlayerUIBase::DrawAlbumCover(CRect rect)
{
    DrawAreaGuard guard(&m_draw, rect);
    if (theApp.m_app_setting_data.show_album_cover && CPlayer::GetInstance().AlbumCoverExist())
    {
        if (theApp.m_app_setting_data.draw_album_high_quality)
        {
            if (theApp.m_app_setting_data.button_round_corners)
                m_draw.DrawRoundImage(CPlayer::GetInstance().GetAlbumCover(), CalculateRoundRectRadius(rect), rect.TopLeft(), rect.Size(), theApp.m_app_setting_data.album_cover_fit);
            else
                m_draw.DrawImage(CPlayer::GetInstance().GetAlbumCover(), rect.TopLeft(), rect.Size(), theApp.m_app_setting_data.album_cover_fit);
        }
        else
        {
            m_draw.DrawBitmap(CPlayer::GetInstance().GetAlbumCover(), rect.TopLeft(), rect.Size(), theApp.m_app_setting_data.album_cover_fit);
        }
    }
    else
    {
        CRect rc_temp = rect;
        int cover_margin = static_cast<int>(rect.Width() * 0.08);
        rc_temp.DeflateRect(cover_margin, cover_margin);
        if (theApp.m_app_setting_data.draw_album_high_quality)
        {
            Gdiplus::Image* image{ CPlayer::GetInstance().IsPlaying() ? theApp.m_image_set.default_cover_img : theApp.m_image_set.default_cover_not_played_img };
            m_draw.DrawImage(image, rc_temp.TopLeft(), rc_temp.Size(), CDrawCommon::StretchMode::FIT);
        }
        else
        {
            int cover_side = min(rc_temp.Width(), rc_temp.Height());
            int x = rc_temp.left + (rc_temp.Width() - cover_side) / 2;
            int y = rc_temp.top + (rc_temp.Height() - cover_side) / 2;

            IconMgr::IconType icon_type = IconMgr::IconType::IT_Default_Cover_Stopped;
            if (CPlayer::GetInstance().IsPlaying())
                icon_type = IconMgr::IconType::IT_Default_Cover_Playing;
            HICON hIcon = theApp.m_icon_mgr.GetHICON(icon_type, IconMgr::IconStyle::IS_Color, IconMgr::IconSize::IS_ORG_512);

            m_draw.DrawIcon(hIcon, CPoint(x, y), CSize(cover_side, cover_side));
        }
    }
}

void CPlayerUIBase::DrawAlbumCoverWithInfo(CRect rect)
{
    const int info_height{ DPI(60) };   //歌曲信息区域的高度
    if (IsDrawBackgroundAlpha())
    {
        DrawAlbumCover(rect);
    }
    else        //如果不绘制透明背景，则将专辑封面显示在歌曲信息区域的上方
    {
        CRect rect_temp{ rect };
        rect_temp.bottom -= info_height;
        DrawAlbumCover(rect_temp);
    }

    //绘制信息信息
    CRect rect_info{ rect };
    rect_info.top = rect_info.bottom - info_height;
    if (rect_info.top < m_draw_rect.top)
        rect_info.top = m_draw_rect.top;

    COLORREF text_color;
    COLORREF back_color;
    BYTE alpha;
    //if (theApp.m_app_setting_data.dark_mode)
    //{
    //    text_color = theApp.m_app_setting_data.theme_color.dark2;
    //    back_color = ColorTable::WHITE;
    //    alpha = 204;
    //}
    //else
    //{
    text_color = ColorTable::WHITE;
    back_color = GRAY(64);
    alpha = 108;
    //}

    if (IsDrawBackgroundAlpha())
        m_draw.FillAlphaRect(rect_info, back_color, alpha /** 3 / 4*/);
    else
        m_draw.FillRect(rect_info, back_color);

    rect_info.DeflateRect(DPI(24), DPI(8));
    //绘制艺术家
    CRect rect_artist{ rect_info };
    rect_artist.bottom = rect_artist.top + DPI(16);
    static CDrawCommon::ScrollInfo scroll_info_artist;
    m_draw.DrawScrollText(rect_artist, CPlayer::GetInstance().GetSafeCurrentSongInfo().GetArtist().c_str(), text_color, GetScrollTextPixel(true), false, scroll_info_artist);
    //绘制歌曲标题
    CRect rect_title{ rect_info };
    rect_title.top = rect_artist.bottom;
    wstring str_title;
    if (CPlayer::GetInstance().GetSafeCurrentSongInfo().IsTitleEmpty())             //如果标题为空，则显示文件名
        str_title = CPlayer::GetInstance().GetSafeCurrentSongInfo().GetFileName();
    else
        str_title = CPlayer::GetInstance().GetSafeCurrentSongInfo().GetTitle();
    CFont* pOldFont = m_draw.SetFont(&theApp.m_font_set.GetFontBySize(12).GetFont(theApp.m_ui_data.full_screen));
    static CDrawCommon::ScrollInfo scroll_info_title;
    m_draw.DrawScrollText(rect_title, str_title.c_str(), text_color, GetScrollTextPixel(true), false, scroll_info_title);
    m_draw.SetFont(pOldFont);

}

void CPlayerUIBase::DrawVolumeButton(CRect rect, bool adj_btn_top, bool show_text)
{
    m_show_volume_text = show_text;

    auto& btn{ m_buttons[BTN_VOLUME] };
    if (btn.pressed)
        rect.MoveToXY(rect.left + theApp.DPI(1), rect.top + theApp.DPI(1));

    DrawAreaGuard guard(&m_draw, rect);
    //绘制背景
    //if (btn.pressed || btn.hover)
    //{
    //    CRect rect_back{ rect };
    //    rect_back.DeflateRect(DPI(2), DPI(2));
    //    BYTE alpha;
    //    if (IsDrawBackgroundAlpha())
    //        alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency) * 2 / 3;
    //    else
    //        alpha = 255;
    //    COLORREF back_color{};
    //        if (btn.pressed)
    //            back_color = m_colors.color_button_pressed;
    //        else
    //            back_color = m_colors.color_button_hover;
    //    if (!theApp.m_app_setting_data.button_round_corners)
    //        m_draw.FillAlphaRect(rect_back, back_color, alpha);
    //    else
    //        m_draw.DrawRoundRect(rect_back, back_color, theApp.DPI(3), alpha);
    //}

    //绘制图标
    CRect rect_icon{ rect };
    rect_icon.right = rect_icon.left + rect_icon.Height();
    DrawUiIcon(rect_icon, GetBtnIconType(BTN_VOLUME));

    //绘制文本
    if (show_text && rect_icon.right < rect.right)
    {
        CRect rect_text{ rect };
        rect_text.left = rect_icon.right;
        CString str;
        if (CPlayer::GetInstance().GetVolume() <= 0)
            str = theApp.m_str_table.LoadText(L"UI_TXT_VOLUME_MUTE").c_str();
        else
            str.Format(_T("%d%%"), CPlayer::GetInstance().GetVolume());
        if (m_buttons[BTN_VOLUME].hover)        //鼠标指向音量区域时，以另外一种颜色显示
            m_draw.DrawWindowText(rect_text, str, m_colors.color_text_heighlight);
        else
            m_draw.DrawWindowText(rect_text, str, m_colors.color_text);
    }
    //设置音量调整按钮的位置
    CRect rc_tmp = rect;
    rc_tmp.bottom = rc_tmp.top + DPI(24);
    m_buttons[BTN_VOLUME].rect = rc_tmp;
    m_buttons[BTN_VOLUME].rect.DeflateRect(0, DPI(4));
    m_buttons[BTN_VOLUME_DOWN].rect = m_buttons[BTN_VOLUME].rect;
    m_buttons[BTN_VOLUME_DOWN].rect.bottom += DPI(4);
    if (adj_btn_top)
    {
        m_buttons[BTN_VOLUME_DOWN].rect.MoveToY(m_buttons[BTN_VOLUME].rect.top - m_buttons[BTN_VOLUME_DOWN].rect.Height());
    }
    else
    {
        m_buttons[BTN_VOLUME_DOWN].rect.MoveToY(m_buttons[BTN_VOLUME].rect.bottom);
    }
    m_buttons[BTN_VOLUME_DOWN].rect.right = m_buttons[BTN_VOLUME].rect.left + DPI(27);      //设置单个音量调整按钮的宽度
    m_buttons[BTN_VOLUME_UP].rect = m_buttons[BTN_VOLUME_DOWN].rect;
    m_buttons[BTN_VOLUME_UP].rect.MoveToX(m_buttons[BTN_VOLUME_DOWN].rect.right);
}

void CPlayerUIBase::DrawLyrics(CRect rect, CFont* lyric_font, CFont* lyric_tr_font, bool with_background, bool show_song_info)
{
    if (rect.Height() < DPI(4))
        return;

    //填充歌词区域背景
    if (with_background && theApp.m_app_setting_data.lyric_background)
    {
        BYTE alpha = 255;
        if (IsDrawBackgroundAlpha())
            alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency) * 3 / 5;
        if (theApp.m_app_setting_data.button_round_corners)
        {
            DrawAreaGuard guard(&m_draw, rect);
            m_draw.DrawRoundRect(rect, m_colors.color_lyric_back, CalculateRoundRectRadius(rect), alpha);
        }
        else
        {
            m_draw.FillAlphaRect(rect, m_colors.color_lyric_back, alpha);
        }
    }
    //设置歌词文字区域
    int margin = Margin();
    rect.DeflateRect(margin, margin);

    //绘制歌词文本
    m_draw.SetLyricFont(lyric_font, lyric_tr_font);
    m_draw.DrawLryicCommon(rect, theApp.m_lyric_setting_data.lyric_align, show_song_info);
}

void CPlayerUIBase::DrawCurrentPlaylistIndicator(CRect rect, UiElement::PlaylistIndicator* playlist_indicator)
{
    // 此m_list_cache为UI线程缓存当前列表，只有at(0)是有效的
    ASSERT(playlist_indicator->m_list_cache.size() == 1);
    const ListItem& list_item = playlist_indicator->m_list_cache.at(0);

    IconMgr::IconType icon_type = list_item.GetTypeIcon();
    wstring str = list_item.GetTypeDisplayName();
    //绘制图标
    CRect rect_icon{ rect };
    rect_icon.right = rect_icon.left + DPI(26);
    DrawUiIcon(rect_icon, icon_type);
    //设置字体
    UiFontGuard set_font(this, playlist_indicator->font_size);
    //绘制文本
    CRect rect_text{ rect };
    rect_text.left = rect_icon.right;
    rect_text.right = rect_text.left + m_draw.GetTextExtent(str.c_str()).cx;
    m_draw.DrawWindowText(rect_text, str.c_str(), m_colors.color_text, Alignment::LEFT, true);
    //绘制菜单按钮
    CRect menu_btn_rect{ rect };
    menu_btn_rect.left = rect.right - DPI(26);
    const int icon_size{ (std::min)(DPI(24), rect.Height()) };
    CRect menu_btn_icon_rect = CDrawCommon::CalculateCenterIconRect(menu_btn_rect, icon_size);
    DrawUIButton(menu_btn_icon_rect, playlist_indicator->btn_menu, IconMgr::IconType::IT_Menu);
    //绘制当前播放列表名称
    CRect rect_name{ rect };
    rect_name.left = rect_text.right + DPI(8);
    rect_name.right = menu_btn_rect.left - DPI(4);
    BYTE alpha{ 255 };
    if (IsDrawBackgroundAlpha())
        alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency) / 2;
    if (theApp.m_app_setting_data.button_round_corners)
        m_draw.DrawRoundRect(rect_name, m_colors.color_control_bar_back, DPI(4), alpha);
    else
        m_draw.FillAlphaRect(rect_name, m_colors.color_control_bar_back, alpha);
    playlist_indicator->rect_name = rect_name;
    rect_name.left += DPI(6);
    rect_name.right -= DPI(30);
    static CDrawCommon::ScrollInfo name_scroll_info;
    m_draw.DrawScrollText(rect_name, list_item.GetDisplayName().c_str(), m_colors.color_text_heighlight, GetScrollTextPixel(), false, name_scroll_info);
    //绘制下拉按钮
    CRect rect_drop_down{ rect };
    rect_drop_down.left = rect_name.right + DPI(2);
    rect_drop_down.right = menu_btn_rect.left - DPI(6);
    CRect rect_drop_down_btn = CDrawCommon::CalculateCenterIconRect(rect_drop_down, icon_size);
    DrawUIButton(rect_drop_down_btn, playlist_indicator->btn_drop_down, IconMgr::IconType::IT_DropDown);
}

void CPlayerUIBase::DrawStackIndicator(UIButton indicator, int num, int index)
{
    //绘制背景
    if (indicator.pressed || indicator.hover)
    {
        BYTE alpha;
        alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency) * 2 / 3;
        COLORREF back_color{};
        if (indicator.pressed)
            back_color = m_colors.color_button_pressed;
        else
            back_color = m_colors.color_button_hover;
        //if (!theApp.m_app_setting_data.button_round_corners)
        //    m_draw.FillAlphaRect(indicator.rect, back_color, alpha);
        //else
        m_draw.DrawRoundRect(indicator.rect, back_color, indicator.rect.Height() / 2, alpha);
    }

    //绘制圆
    int start_x = indicator.rect.left;
    start_x += (indicator.rect.Width() - num * DPI(12)) / 2;
    for (int i = 0; i < num; i++)
    {
        CRect rect_dot{ indicator.rect };
        rect_dot.left = start_x + i * DPI(12);
        rect_dot.right = rect_dot.left + DPI(12);
        int dot_size{ DPI(5) };
        rect_dot.left += (rect_dot.Width() - dot_size) / 2;
        rect_dot.top += (rect_dot.Height() - dot_size) / 2;
        rect_dot.right = rect_dot.left + dot_size;
        rect_dot.bottom = rect_dot.top + dot_size;
        COLORREF dot_color{};
        BYTE alpha{ 255 };
        if (i == index)
        {
            dot_color = m_colors.color_text;
        }
        else
        {
            dot_color = m_colors.color_stack_indicator;
            alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency) * 2 / 3;
        }
        m_draw.DrawEllipse(rect_dot, dot_color, alpha);
    }
}

void CPlayerUIBase::DrawUiMenuBar(CRect rect)
{
    //m_draw.DrawWindowText(rect, L"menu bar", m_colors.color_text);

    //绘制背景
    bool draw_background{ IsDrawBackgroundAlpha() };
    BYTE alpha;
    if (theApp.m_app_setting_data.dark_mode)
        alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency) / 2;
    else
        alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency) * 2 / 3;

    if (draw_background)
        m_draw.FillAlphaRect(rect, m_colors.color_control_bar_back, alpha);
    else
        m_draw.FillRect(rect, m_colors.color_control_bar_back);

    CRect rc_item{ rect };
    rc_item.bottom = rc_item.top + DPI(20);
    rc_item.left += DPI(4);
    auto drawMenuItem = [&](BtnKey key, LPCTSTR menu_str)
        {
            UIButton& btn{ m_buttons[key] };
            btn.rect = rc_item;
            btn.rect.right = btn.rect.left + btn.rect.Height() + m_draw.GetTextExtent(menu_str).cx + DPI(6);

            CRect rc_cur_item{ btn.rect };
            if (btn.pressed && btn.enable)
                rc_cur_item.OffsetRect(theApp.DPI(1), theApp.DPI(1));

            //绘制背景
            if (btn.pressed || btn.hover)
            {
                BYTE alpha;
                if (draw_background)
                    alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency) * 2 / 3;
                else
                    alpha = 255;
                COLORREF back_color{};
                if (btn.pressed)
                    back_color = m_colors.color_button_pressed;
                else
                    back_color = m_colors.color_button_hover;

                if (!theApp.m_app_setting_data.button_round_corners)
                    m_draw.FillAlphaRect(rc_cur_item, back_color, alpha);
                else
                    m_draw.DrawRoundRect(rc_cur_item, back_color, CalculateRoundRectRadius(rect), alpha);
            }

            //绘制图标
            CRect rc_icon{ rc_cur_item };
            rc_icon.left += DPI(2);
            rc_icon.right = rc_icon.left + rc_icon.Height();
            HICON hIcon = theApp.m_icon_mgr.GetHICON(GetBtnIconType(key),
                theApp.m_app_setting_data.dark_mode ? IconMgr::IconStyle::IS_OutlinedLight : IconMgr::IconStyle::IS_OutlinedDark,
                IsDrawLargeIcon() ? IconMgr::IconSize::IS_DPI_16_Full_Screen : IconMgr::IconSize::IS_DPI_16);
            m_draw.DrawIcon(hIcon, rc_icon, DPI(16));

            //绘制文本
            CRect rc_text{ rc_cur_item };
            rc_text.left = rc_icon.right;
            rc_text.right = btn.rect.right;
            m_draw.DrawWindowText(rc_text, menu_str, m_colors.color_text);

            //下一个矩形的位置
            rc_item.left = rc_text.right + DPI(2);
        };
    // 自绘的菜单栏不支持助记键，显示的字符串与系统菜单栏不同，所以这里的字符串使用LoadText而不是LoadMenuText
    static const wstring& menu_name_file = theApp.m_str_table.LoadText(L"UI_TXT_MENU_FILE");
    static const wstring& menu_name_play_control = theApp.m_str_table.LoadText(L"UI_TXT_MENU_PLAY_CONTROL");
    static const wstring& menu_name_playlist = theApp.m_str_table.LoadText(L"UI_TXT_MENU_PLAYLIST");
    static const wstring& menu_name_lyrics = theApp.m_str_table.LoadText(L"UI_TXT_MENU_LYRICS");
    static const wstring& menu_name_view = theApp.m_str_table.LoadText(L"UI_TXT_MENU_VIEW");
    static const wstring& menu_name_tools = theApp.m_str_table.LoadText(L"UI_TXT_MENU_TOOLS");
    static const wstring& menu_name_help = theApp.m_str_table.LoadText(L"UI_TXT_MENU_HELP");
    drawMenuItem(MENU_FILE, menu_name_file.c_str());                            //文件
    drawMenuItem(MENU_PLAY_CONTROL, menu_name_play_control.c_str());            //播放控制
    drawMenuItem(MENU_PLAYLIST, menu_name_playlist.c_str());                    //播放列表
    drawMenuItem(MENU_LYRICS, menu_name_lyrics.c_str());                        //歌词
    drawMenuItem(MENU_VIEW, menu_name_view.c_str());                            //视图
    drawMenuItem(MENU_TOOLS, menu_name_tools.c_str());                          //工具
    drawMenuItem(MENU_HELP, menu_name_help.c_str());                           //帮助
}

void CPlayerUIBase::DrawNavigationBar(CRect rect, UiElement::NavigationBar* tab_element)
{
    DrawAreaGuard guard(&m_draw, rect);
    bool draw_icon{ tab_element->icon_type == UiElement::NavigationBar::ICON_AND_TEXT || tab_element->icon_type == UiElement::NavigationBar::ICON_ONLY };
    bool draw_text{ tab_element->icon_type == UiElement::NavigationBar::ICON_AND_TEXT || tab_element->icon_type == UiElement::NavigationBar::TEXT_ONLY };
    int x_pos{ rect.left };
    int y_pos{ rect.top };
    int index{};
    tab_element->item_rects.resize(tab_element->tab_list.size());
    for (const auto& navigation_item : tab_element->tab_list)
    {
        //计算矩形区域
        int icon_width{};
        int text_width{};
        if (draw_icon)
        {
            int item_height{ tab_element->orientation == UiElement::NavigationBar::Horizontal ? rect.Height() : DPI(tab_element->item_height) };
            icon_width = (std::max)(DPI(24), item_height - DPI(4));
        }
        if (draw_text)
            text_width = m_draw.GetTextExtent(navigation_item.text.c_str()).cx;
        CRect item_rect{ rect };
        if (tab_element->orientation == UiElement::NavigationBar::Horizontal)
        {
            item_rect.left = x_pos;
            item_rect.right = item_rect.left + icon_width + text_width + DPI(4);
            if (tab_element->icon_type == UiElement::NavigationBar::TEXT_ONLY)
                item_rect.right += DPI(4);
            else if (tab_element->icon_type == UiElement::NavigationBar::ICON_AND_TEXT && tab_element->item_height > 20)
                item_rect.right += DPI(tab_element->item_height - 20) / 2;      //基于高度值增加一些右侧的边距
        }
        else
        {
            item_rect.top = y_pos;
            item_rect.bottom = item_rect.top + DPI(tab_element->item_height);
        }
        tab_element->item_rects[index] = item_rect;

        if ((rect & item_rect).IsRectEmpty())
            continue;

        //绘制背景
        if (tab_element->hover_index == index)
        {
            DrawAreaGuard guard(&m_draw, rect);
            DrawRectangle(item_rect, tab_element->pressed ? m_colors.color_button_pressed : m_colors.color_button_hover);
        }

        //绘制图标
        CRect icon_rect{ item_rect };
        if (draw_icon)
        {
            DrawAreaGuard guard(&m_draw, rect);
            if (tab_element->icon_type != UiElement::NavigationBar::ICON_ONLY)
            {
                icon_rect.right = icon_rect.left + icon_width;
                if (tab_element->orientation == UiElement::NavigationBar::Vertical)
                    icon_rect.MoveToX(icon_rect.left + DPI(tab_element->item_left_space));
            }
            //使用跳动的频谱代替正在播放图标
            if (navigation_item.icon == IconMgr::IT_NowPlaying && CPlayer::GetInstance().GetPlayingState2() != PS_STOPED && !CPlayer::GetInstance().IsMciCore())
            {
                DrawMiniSpectrum(icon_rect);
            }
            else
            {
                DrawUiIcon(icon_rect, navigation_item.icon);
            }
        }
        else
        {
            icon_rect.right = icon_rect.left;
        }

        //绘制文本
        if (draw_text)
        {
            DrawAreaGuard guard(&m_draw, rect);
            CRect text_rect{ item_rect };
            if (tab_element->icon_type != UiElement::NavigationBar::TEXT_ONLY)
            {
                text_rect.left = icon_rect.right;
            }
            else
            {
                text_rect.MoveToX(text_rect.left + DPI(4));
                if (tab_element->orientation == UiElement::NavigationBar::Vertical)
                    text_rect.left += DPI(8);
            }
            CFont* old_font{};  //原先的字体
            bool big_font{ m_ui_data.full_screen && IsDrawLargeIcon() };
            old_font = m_draw.SetFont(&theApp.m_font_set.GetFontBySize(tab_element->font_size).GetFont(big_font));
            m_draw.DrawWindowText(text_rect, navigation_item.text.c_str(), m_colors.color_text, Alignment::LEFT, true);
            m_draw.SetFont(old_font);
        }

        //绘制选中指示
        if (tab_element->SelectedIndex() == index)
        {
            DrawAreaGuard guard(&m_draw, rect);
            CRect selected_indicator_rect{ item_rect };
            //水平排列时选中指示在底部
            if (tab_element->orientation == UiElement::NavigationBar::Horizontal)
            {
                selected_indicator_rect.left += DPI(4);
                selected_indicator_rect.right -= DPI(4);
                selected_indicator_rect.top = selected_indicator_rect.bottom - DPI(4);
            }
            //垂直排列时选中指示在左侧
            else
            {
                selected_indicator_rect.top += DPI(4);
                selected_indicator_rect.bottom -= DPI(4);
                selected_indicator_rect.right = selected_indicator_rect.left + DPI(4);
            }
            if (theApp.m_app_setting_data.button_round_corners)
                m_draw.DrawRoundRect(selected_indicator_rect, m_colors.color_text_heighlight, DPI(2));
            else
                m_draw.FillRect(selected_indicator_rect, m_colors.color_text_heighlight, true);

        }

        if (tab_element->orientation == UiElement::NavigationBar::Horizontal)
            x_pos = item_rect.right + DPI(tab_element->item_space);
        else
            y_pos = item_rect.bottom + DPI(tab_element->item_space);
        index++;
    }
}

void CPlayerUIBase::DrawMiniSpectrum(CRect rect)
{
    COLORREF icon_color{ theApp.m_app_setting_data.dark_mode ? RGB(255, 255, 255) : RGB(110, 110, 110) };
    CSize size_icon = IconMgr::GetIconSize(IsDrawLargeIcon() ? IconMgr::IS_DPI_16_Full_Screen : IconMgr::IS_DPI_16);
    CPoint pos_icon{ rect.left + (rect.Width() - size_icon.cx) / 2 , rect.top + (rect.Height() - size_icon.cy) / 2 };
    const int spectrum_unit_width = DPI(4);     //柱形+间隙的宽度
    int col_width = DPI(2);                     //柱形的宽度
    int gap_width = spectrum_unit_width - col_width; //间隙的宽度

    const int COLS = 4;
    CRect rc_spectrum_top = CRect(pos_icon, size_icon);

    CRect rects[COLS];
    rects[0] = rc_spectrum_top;
    rects[0].right = rects[0].left + col_width;

    //频谱的实际宽度
    int width_actrual{ col_width * COLS + gap_width * (COLS - 1) };

    for (int i{ 1 }; i < COLS; i++)
    {
        rects[i] = rects[0];
        rects[i].left += (i * (col_width + gap_width));
        rects[i].right += (i * (col_width + gap_width));
    }

    float spetral_data = 0;
    int col_index = 0;
    //仅取中间一段频谱
    int COL_MIN = SPECTRUM_COL / 4;
    int COL_MAX = SPECTRUM_COL * 3 / 4;
    for (int i{ COL_MIN }; i < COL_MAX; i++)
    {
        spetral_data += CPlayer::GetInstance().GetSpectralData()[i];
        if ((i + 1 - COL_MIN) % ((COL_MAX - COL_MIN) / COLS) == 0)
        {
            CRect rect_tmp{ rects[col_index] };
            int spetral_height = static_cast<int>(spetral_data * rects[0].Height() * 15.0 / FFT_SAMPLE / COLS);  //调整这里常数的值可以调整频谱的整体高度
            if (spetral_height <= 0 || CPlayer::GetInstance().IsError())
                spetral_height = 1;
            rect_tmp.top = rect_tmp.bottom - spetral_height;
            if (rect_tmp.top < rects[0].top)
                rect_tmp.top = rects[0].top;
            m_draw.FillRect(rect_tmp, icon_color, true);

            col_index++;
            spetral_data = 0;
        }
    }
}

void CPlayerUIBase::DrawSearchBox(CRect rect, UiElement::SearchBox* search_box)
{
    //绘制背景
    COLORREF back_color;
    if (search_box->hover)
        back_color = m_colors.color_button_hover;
    else
        back_color = m_colors.color_control_bar_back;
    bool draw_background{ IsDrawBackgroundAlpha() };
    BYTE alpha;
    if (!draw_background)
        alpha = 255;
    else if (theApp.m_app_setting_data.dark_mode || search_box->hover)
        alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency) * 2 / 3;
    else
        alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency);
    if (!theApp.m_app_setting_data.button_round_corners)
        m_draw.FillAlphaRect(rect, back_color, alpha);
    else
        m_draw.DrawRoundRect(rect, back_color, CalculateRoundRectRadius(rect), alpha);
    //绘制文本
    CRect rect_text{ rect };
    rect_text.left += DPI(4);
    rect_text.right -= rect.Height();
    std::wstring text = search_box->key_word;
    COLORREF text_color = m_colors.color_text;
    if (text.empty())
    {
        text = theApp.m_str_table.LoadText(L"TXT_SEARCH_PROMPT");
        text_color = m_colors.color_text_heighlight;
    }
    m_draw.DrawWindowText(rect_text, text.c_str(), text_color, Alignment::LEFT, true);
    //绘制图标
    search_box->icon_rect = rect;;
    search_box->icon_rect.left = rect_text.right;
    if (search_box->key_word.empty())
    {
        DrawUiIcon(search_box->icon_rect, IconMgr::IT_Find);
    }
    else
    {
        CRect btn_rect{ search_box->icon_rect };
        btn_rect.DeflateRect(DPI(2), DPI(2));
        DrawUIButton(btn_rect, search_box->clear_btn, IconMgr::IT_Close);
    }
}

void CPlayerUIBase::DrawUiIcon(const CRect& rect, IconMgr::IconType icon_type, IconMgr::IconStyle icon_style, IconMgr::IconSize icon_size)
{
    // style为IS_Auto时根据深色模式设置向IconMgr要求深色/浅色图标，没有对应风格图标时IconMgr会自行fallback
    if (icon_style == IconMgr::IconStyle::IS_Auto)
        icon_style = theApp.m_app_setting_data.dark_mode ? IconMgr::IconStyle::IS_OutlinedLight : IconMgr::IconStyle::IS_OutlinedDark;
    // 要求大图标时在icon_size基础上再进行放大（×全屏缩放系数）
    if (IsDrawLargeIcon() && icon_size == IconMgr::IconSize::IS_DPI_16)
        icon_size = IconMgr::IconSize::IS_DPI_16_Full_Screen;
    if (IsDrawLargeIcon() && icon_size == IconMgr::IconSize::IS_DPI_20)
        icon_size = IconMgr::IconSize::IS_DPI_20_Full_Screen;
    if (IsDrawLargeIcon() && icon_size == IconMgr::IconSize::IS_DPI_32)
        icon_size = IconMgr::IconSize::IS_DPI_32_Full_Screen;
    // 使图标在矩形中居中
    CSize size_icon = IconMgr::GetIconSize(icon_size);
    CPoint pos_icon{ rect.left + (rect.Width() - size_icon.cx) / 2 , rect.top + (rect.Height() - size_icon.cy) / 2 };
    // 绘制图标
    HICON hIcon = theApp.m_icon_mgr.GetHICON(icon_type, icon_style, icon_size);
    m_draw.DrawIcon(hIcon, pos_icon, size_icon);
}

std::wstring CPlayerUIBase::GetItemTooltip(int tooltip_index)
{
    bool is_mini_mode{ IsMiniMode() };
    switch (tooltip_index)
    {
    // 翻译
    case BTN_TRANSLATE:
        return theApp.m_str_table.LoadText(L"UI_TIP_BTN_TRANSLATION");
    // 音量
    case BTN_VOLUME:
        return GetVolumeTooltipString().GetString();
    // 切换界面
    case BTN_SKIN: case BTN_SKIN_TITLEBAR:
    {
        wstring tip_str = theApp.m_str_table.LoadText(L"UI_TIP_BTN_SWITCH_UI");
        if (!is_mini_mode) tip_str += GetCmdShortcutKeyForTooltips(ID_SWITCH_UI);
        return tip_str;
    }
    // 音效
    case BTN_EQ:
        return theApp.m_str_table.LoadText(L"UI_TIP_BTN_SOUND_EFFECT_SETTING") + GetCmdShortcutKeyForTooltips(ID_EQUALIZER).GetString();
    // 设置
    case BTN_SETTING: case BTN_SETTING_TITLEBAR:
        return theApp.m_str_table.LoadText(L"UI_TIP_BTN_OPTION_SETTING") + GetCmdShortcutKeyForTooltips(ID_OPTION_SETTINGS).GetString();
    // MINI模式
    case BTN_MINI: case BTN_MINI_TITLEBAR:
    {
        wstring tip_str;
        if (is_mini_mode)
            tip_str = theApp.m_str_table.LoadText(L"UI_TIP_BTN_MINIMODE_RTN");
        else
            tip_str = theApp.m_str_table.LoadText(L"UI_TIP_BTN_MINIMODE");
        tip_str += GetCmdShortcutKeyForTooltips(ID_MINI_MODE);
        return tip_str;
    }
    // 曲目信息（属性）
    case BTN_INFO:
        SetSongInfoToolTipText();
        return m_info_tip;
    // 停止
    case BTN_STOP:
        return theApp.m_str_table.LoadText(L"UI_TIP_BTN_STOP");
    // 上一曲
    case BTN_PREVIOUS:
        return theApp.m_str_table.LoadText(L"UI_TIP_BTN_PREVIOUS");
    // 下一曲
    case BTN_NEXT:
        return theApp.m_str_table.LoadText(L"UI_TIP_BTN_NEXT");
    // 播放/暂停
    case BTN_PLAY_PAUSE:
        if (CPlayer::GetInstance().IsPlaying() && !CPlayer::GetInstance().IsError())
            return theApp.m_str_table.LoadText(L"UI_TIP_BTN_PAUSE");
        else
           return theApp.m_str_table.LoadText(L"UI_TIP_BTN_PLAY");
    // 进度条
    case UiElement::TooltipIndex::PROGRESS_BAR:
        return theApp.m_str_table.LoadTextFormat(L"UI_TIP_SEEK_TO_MINUTE_SECOND", { L"0", L"00" });
    // 显示/隐藏播放列表
    case BTN_SHOW_PLAYLIST:
    {
        wstring tip_str = theApp.m_str_table.LoadText(L"UI_TIP_BTN_PLAYLIST_SHOW_HIDE");
        if (!is_mini_mode)
        {
            if (theApp.m_media_lib_setting_data.playlist_btn_for_float_playlist)
                tip_str += GetCmdShortcutKeyForTooltips(ID_FLOAT_PLAYLIST);
            else
                tip_str += GetCmdShortcutKeyForTooltips(ID_SHOW_PLAYLIST);
        }
        return tip_str;
    }
    // 媒体库
    case BTN_MEDIA_LIB:
        return theApp.m_str_table.LoadText(L"UI_TIP_BTN_MEDIA_LIB") + GetCmdShortcutKeyForTooltips(ID_MEDIA_LIB).GetString();
    // 查找歌曲
    case BTN_FIND:
        return theApp.m_str_table.LoadText(L"UI_TIP_BTN_FIND_SONGS") + GetCmdShortcutKeyForTooltips(ID_FIND).GetString();
    // 全屏
    case BTN_FULL_SCREEN: case BTN_FULL_SCREEN_TITLEBAR:
    {
        wstring tip_str;
        if (m_ui_data.full_screen)
            tip_str = theApp.m_str_table.LoadText(L"UI_TIP_BTN_FULL_SCREEN_EXIT");
        else
            tip_str = theApp.m_str_table.LoadText(L"UI_TIP_BTN_FULL_SCREEN");
        tip_str += GetCmdShortcutKeyForTooltips(ID_FULL_SCREEN);
        return tip_str;
    }
    // 主菜单
    case BTN_MENU: case BTN_MENU_TITLEBAR:
        return theApp.m_str_table.LoadText(L"UI_TIP_BTN_MAIN_MENU");
    // “我喜欢的音乐”
    case BTN_FAVOURITE:
        return theApp.m_str_table.LoadText(L"UI_TIP_BTN_FAVOURITE");
    // 桌面歌词
    case BTN_LRYIC:
        return theApp.m_str_table.LoadText(L"UI_TIP_BTN_DESKTOP_LYRIC");
    // AB重复
    case BTN_AB_REPEAT:
        return theApp.m_str_table.LoadText(L"UI_TIP_BTN_AB_REPEAT");
    // 关闭
    case BTN_CLOSE: case BTN_APP_CLOSE:
        return theApp.m_str_table.LoadText(L"UI_TIP_BTN_CLOSE");
    // 最小化
    case BTN_MINIMIZE:
        return theApp.m_str_table.LoadText(L"UI_TIP_BTN_MINIMIZE");
    // 最大化
    case BTN_MAXIMIZE:
        return theApp.m_str_table.LoadText(L"UI_TIP_BTN_MAXIMIZE");
    // 添加到播放列表
    case BTN_ADD_TO_PLAYLIST:
        return theApp.m_str_table.LoadText(L"UI_TIP_BTN_ADD_TO_PLAYLIST");
    // 切换界面
    case BTN_SWITCH_DISPLAY:
        return theApp.m_str_table.LoadText(L"UI_TIP_BTN_SWITCH_DISPLAY");
    // 深色/浅色模式
    case BTN_DARK_LIGHT: case BTN_DARK_LIGHT_TITLE_BAR:
    {
        wstring tip_str;
        if (theApp.m_app_setting_data.dark_mode)
            tip_str = theApp.m_str_table.LoadText(L"UI_TIP_BTN_DARK_LIGHT_TO_LIGHT_MODE");
        else
            tip_str = theApp.m_str_table.LoadText(L"UI_TIP_BTN_DARK_LIGHT_TO_DARK_MODE");
        tip_str += GetCmdShortcutKeyForTooltips(ID_DARK_MODE);
        return tip_str;
    }
    // 播放列表定位到当前播放
    case BTN_LOCATE_TO_CURRENT:
        return theApp.m_str_table.LoadText(L"UI_TIP_BTN_LOCATE_TO_CURRENT") + GetCmdShortcutKeyForTooltips(ID_LOCATE_TO_CURRENT).GetString();
    // 播放列表菜单按钮
    case UiElement::TooltipIndex::PLAYLIST_MENU_BTN:
        return theApp.m_str_table.LoadText(L"UI_TIP_BTN_PLAYLIST_MENU");
    // 播放列表下拉菜单按钮
    case UiElement::TooltipIndex::PLAYLIST_DROP_DOWN_BTN:
        return theApp.m_str_table.LoadText(L"UI_TIP_BTN_RECENT_FOLDER_OR_PLAYLIST");
    // 循环模式
    case BTN_REPETEMODE:
        SetRepeatModeToolTipText();
        return m_repeat_mode_tip;
    // "播放“我喜欢的音乐”"
    case BTN_PLAY_MY_FAVOURITE:
        return theApp.m_str_table.LoadText(L"UI_TIP_BTN_PLAY_MY_FAVOURITE");
    // "播放“所有曲目”"
    case BTN_PLAY_ALL_TRACKS:
        return theApp.m_str_table.LoadText(L"UI_TIP_BTN_PLAY_ALL_TRACKS");
    //歌词卡拉OK样式显示
    case BTN_KARAOKE:
        return theApp.m_str_table.LoadText(L"UI_TIP_BTN_KARAOKE");
    //搜索框清除按钮
    case UiElement::TooltipIndex::SEARCHBOX_CLEAR_BTN:
        return theApp.m_str_table.LoadText(L"TIP_SEARCH_EDIT_CLEAN");
    //显示播放队列
    case BTN_SHOW_PLAY_QUEUE:
        return theApp.m_str_table.LoadText(L"UI_TIP_BTN_PLAY_QUEUE");
    //关闭面板
    case BTN_CLOSE_PANEL:
        return theApp.m_str_table.LoadText(L"UI_TIP_BTN_CLOSE");
    //标题栏中的关闭面板
    case BTN_CLOSE_PANEL_TITLE_BAR:
        return theApp.m_str_table.LoadText(L"UI_TIP_BTN_BACK");
    //显示设置面板
    case BTN_SHOW_SETTINGS_PANEL:
        return theApp.m_str_table.LoadText(L"UI_TIP_BTN_OPTION_SETTING");
 }
    return std::wstring();
}

void CPlayerUIBase::AddToolTips()
{
    //添加所有按钮鼠标提示
    for (int i = 0; i < BTN_MAX; i++)
    {
        std::wstring str_tooltip = GetItemTooltip(i);
        AddMouseToolTip(i, str_tooltip.c_str());
    }
    for (int i = UiElement::TooltipIndex::INDEX_MIN; i < UiElement::TooltipIndex::INDEX_MAX; i++)
    {
        std::wstring str_tooltip = GetItemTooltip(i);
        AddMouseToolTip(i, str_tooltip.c_str());
    }
}
