#include "stdafx.h"
#include "CPlayerUIBase.h"
#include "MusicPlayerDlg.h"


CPlayerUIBase::CPlayerUIBase(UIData& ui_data, CWnd* pMainWnd)
    : m_ui_data(ui_data), m_pMainWnd(pMainWnd)
{

    //m_font_time.CreatePointFont(80, CCommon::LoadText(IDS_DEFAULT_FONT));
}


CPlayerUIBase::~CPlayerUIBase()
{
    m_mem_bitmap_static.DeleteObject();
}

void CPlayerUIBase::Init(CDC* pDC)
{
    m_pDC = pDC;
    m_draw.Create(m_pDC, m_pMainWnd);

    m_tool_tip.Create(m_pMainWnd, TTS_ALWAYSTIP);
    m_tool_tip.SetMaxTipWidth(theApp.DPI(400));
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
        m_draw.SetFont(&theApp.m_font_set.font9.GetFont(theApp.m_ui_data.full_screen));

        //绘制背景
        DrawBackground();

        //绘制状态栏
        CRect draw_rect = m_draw_rect;
        bool draw_status_bar = IsDrawStatusBar();
        if (draw_status_bar)
        {
            CRect rc_status_bar = draw_rect;
            draw_rect.bottom -= DPI(20);
            rc_status_bar.top = draw_rect.bottom;
            DrawStatusBar(rc_status_bar, reset);
        }

        //如果不显示Windows标准标题栏，则绘制标题栏
        if (IsDrawTitleBar())
        {
            CRect rc_title_bar = draw_rect;
            rc_title_bar.bottom = rc_title_bar.top + m_layout.titlabar_height;
            draw_rect.top = rc_title_bar.bottom;
            DrawTitleBar(rc_title_bar);
        }

        //绘制界面中其他信息
        _DrawInfo(draw_rect, reset);

        //如果切换了显示/隐藏状态栏，则需要更新鼠标提示的位置
        static bool last_draw_status_bar{ false };
        if (draw_status_bar != last_draw_status_bar)
        {
            last_draw_status_bar = draw_status_bar;
            UpdateToolTipPosition();
        }

    }

#endif

    if (m_first_draw)
    {
        AddToolTips();
    }
    else
    {
        static int last_width{}, last_height{}, last_class_id{};
        //检测到绘图区域变化或界面进行了切换时
        if (last_width != m_draw_rect.Width() || last_height != m_draw_rect.Height()
            || (last_class_id != GetClassId() && GetClassId() != 0))
        {
            //更新工具提示的位置
            UpdateToolTipPosition();

            //更新任务栏缩略图区域
            CRect thumbnail_rect = GetThumbnailClipArea();
            CMusicPlayerDlg* pMainWindow = dynamic_cast<CMusicPlayerDlg*>(theApp.m_pMainWnd);
            if (pMainWindow != nullptr)
            {
                pMainWindow->SetThumbnailClipArea(thumbnail_rect);
            }

            last_width = m_draw_rect.Width();
            last_height = m_draw_rect.Height();
            last_class_id = GetClassId();
        }
    }
    m_first_draw = false;
}

void CPlayerUIBase::ClearInfo()
{
    PreDrawInfo();
    m_pDC->FillSolidRect(m_draw_rect, CONSTVAL::BACKGROUND_COLOR);
}

void CPlayerUIBase::LButtonDown(CPoint point)
{
    for (auto& btn : m_buttons)
    {
        if (btn.second.enable && btn.second.rect.PtInRect(point) != FALSE)
            btn.second.pressed = true;
    }
}

void CPlayerUIBase::RButtonUp(CPoint point)
{
    if (!m_draw_rect.PtInRect(point))
        return;

    if (m_buttons[BTN_VOLUME].rect.PtInRect(point) == FALSE)
        m_show_volume_adj = false;

    CPoint point1;      //定义一个用于确定光标位置的位置
    GetCursorPos(&point1);  //获取当前光标的位置，以便使得菜单可以跟随光标，该位置以屏幕左上角点为原点，point则以客户区左上角为原点
    if (m_buttons[BTN_REPETEMODE].rect.PtInRect(point))     //如果在“循环模式”的矩形区域内点击鼠标右键，则弹出“循环模式”的子菜单
    {
        CMenu* pMenu = theApp.m_menu_set.m_main_popup_menu.GetSubMenu(0)->GetSubMenu(1);
        ASSERT(pMenu != nullptr);
        if (pMenu != NULL)
            pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, theApp.m_pMainWnd);
        return;
    }

    if (m_buttons[BTN_SHOW_PLAYLIST].rect.PtInRect(point))
    {
        CMenu* pMenu = theApp.m_menu_set.m_playlist_btn_menu.GetSubMenu(0);
        ASSERT(pMenu != nullptr);
        if (pMenu != NULL)
            pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, theApp.m_pMainWnd);
        return;
    }

    if (m_buttons[BTN_AB_REPEAT].rect.PtInRect(point))
    {
        CMenu* pMenu = theApp.m_menu_set.m_main_menu.GetSubMenu(1)->GetSubMenu(13);
        ASSERT(pMenu != nullptr);
        if (pMenu != nullptr)
            pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, theApp.m_pMainWnd);
        return;
    }

    if (m_buttons[BTN_SKIN].rect.PtInRect(point))
    {
        CMenu* pMenu = theApp.m_menu_set.m_main_menu.GetSubMenu(4)->GetSubMenu(11);
        ASSERT(pMenu != nullptr);
        if (pMenu != nullptr)
            pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, theApp.m_pMainWnd);
        return;
    }

    for (auto& btn : m_buttons)
    {
        //按钮上点击右键不弹出菜单
        if (btn.first != BTN_COVER && btn.second.rect.PtInRect(point) != FALSE)
            return;
    }

    if (!m_draw_data.lyric_rect.PtInRect(point))    //如果在歌词区域点击了鼠标右键
    {
        theApp.m_menu_set.m_main_popup_menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, theApp.m_pMainWnd);
    }
    else
    {
        theApp.m_menu_set.m_popup_menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, theApp.m_pMainWnd);
    }

}

void CPlayerUIBase::MouseMove(CPoint point)
{
    for (auto& btn : m_buttons)
    {
        if (btn.second.enable)
            btn.second.hover = (btn.second.rect.PtInRect(point) != FALSE);
    }

    m_buttons[BTN_PROGRESS].hover = m_buttons[BTN_PROGRESS].hover && !(m_show_volume_adj && (m_buttons[BTN_VOLUME_UP].rect.PtInRect(point) || m_buttons[BTN_VOLUME_DOWN].rect.PtInRect(point)));

    //鼠标指向进度条时显示定位到几分几秒
    if (m_buttons[BTN_PROGRESS].hover)
    {
        __int64 song_pos;
        song_pos = static_cast<__int64>(point.x - m_buttons[BTN_PROGRESS].rect.left) * CPlayer::GetInstance().GetSongLength() / m_buttons[BTN_PROGRESS].rect.Width();
        Time song_pos_time;
        song_pos_time.fromInt(static_cast<int>(song_pos));
        CString str;
        static int last_sec{};
        if (last_sec != song_pos_time.sec)      //只有鼠标指向位置对应的秒数变化了才更新鼠标提示
        {
            str.Format(CCommon::LoadText(IDS_SEEK_TO_MINUTE_SECOND), song_pos_time.min, song_pos_time.sec);
            UpdateMouseToolTip(BTN_PROGRESS, str);
            last_sec = song_pos_time.sec;
        }
    }

    TRACKMOUSEEVENT tme;
    tme.cbSize = sizeof(tme);
    tme.hwndTrack = m_pMainWnd->GetSafeHwnd();
    tme.dwFlags = TME_LEAVE | TME_HOVER;
    tme.dwHoverTime = 1;
    _TrackMouseEvent(&tme);
}

void CPlayerUIBase::LButtonUp(CPoint point)
{
    if (!m_show_volume_adj)     //如果设有显示音量调整按钮，则点击音量区域就显示音量调整按钮
        m_show_volume_adj = (m_buttons[BTN_VOLUME].rect.PtInRect(point) != FALSE);
    else        //如果已经显示了音量调整按钮，则点击音量调整时保持音量调整按钮的显示
        m_show_volume_adj = (m_buttons[BTN_VOLUME_UP].rect.PtInRect(point) || m_buttons[BTN_VOLUME_DOWN].rect.PtInRect(point));

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
            switch (btn.first)
            {
            case BTN_APP_CLOSE:
                theApp.m_pMainWnd->SendMessage(WM_CLOSE);
                return;

            case BTN_MAXIMIZE:
                m_buttons[BTN_MAXIMIZE].hover = false;
                if (theApp.m_pMainWnd->IsZoomed())
                    theApp.m_pMainWnd->SendMessage(WM_SYSCOMMAND, SC_RESTORE);
                else
                    theApp.m_pMainWnd->SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE);
                return;

            case BTN_MINIMIZE:
                m_buttons[BTN_MINIMIZE].hover = false;
                theApp.m_pMainWnd->SendMessage(WM_SYSCOMMAND, SC_MINIMIZE);
                return;

            case BTN_REPETEMODE:
                CPlayer::GetInstance().SetRepeatMode();
                UpdateRepeatModeToolTip();
                return;

            case BTN_VOLUME:
                break;

            case BTN_TRANSLATE:
                m_ui_data.show_translate = !m_ui_data.show_translate;
                return;

            case BTN_SKIN:
                m_buttons[BTN_SKIN].hover = false;
                theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_SWITCH_UI);
                return;

            case BTN_EQ:
                m_buttons[BTN_EQ].hover = false;
                theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_EQUALIZER);
                return;

            case BTN_SETTING:
                m_buttons[BTN_SETTING].hover = false;
                theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_OPTION_SETTINGS);
                return;

            case BTN_MINI:
                m_buttons[BTN_MINI].hover = false;
                theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_MINI_MODE);
                return;

            case BTN_MINI1:
                m_buttons[BTN_MINI1].hover = false;
                theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_MINI_MODE);
                return;

            case BTN_INFO:
                m_buttons[BTN_INFO].hover = false;
                theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_SONG_INFO);
                return;

            case BTN_LRYIC:
                theApp.m_lyric_setting_data.show_desktop_lyric = !theApp.m_lyric_setting_data.show_desktop_lyric;
                return;

            case BTN_AB_REPEAT:
                theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_AB_REPEAT);
                return;

            case BTN_STOP:
                theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_STOP);
                return;

            case BTN_PREVIOUS:
                theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_PREVIOUS);
                return;
            case BTN_PLAY_PAUSE:
                theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_PLAY_PAUSE);
                return;

            case BTN_NEXT:
                theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_NEXT);
                return;

            case BTN_SHOW_PLAYLIST:
                m_buttons[BTN_SHOW_PLAYLIST].hover = false;
                if (theApp.m_nc_setting_data.playlist_btn_for_float_playlist)
                    theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_FLOAT_PLAYLIST);
                else
                    theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_SHOW_PLAYLIST);
                return;

            case BTN_SELECT_FOLDER:
                m_buttons[BTN_SELECT_FOLDER].hover = false;
                theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_SET_PATH);
                return;

            case BTN_FAVOURITE:
                m_buttons[BTN_FAVOURITE].hover = false;
                theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_ADD_REMOVE_FROM_FAVOURITE);
                return;

            case BTN_VOLUME_UP:
                if (m_show_volume_adj)
                {
                    CPlayer::GetInstance().MusicControl(Command::VOLUME_UP, theApp.m_nc_setting_data.volum_step);
                    return;
                }
                break;

            case BTN_VOLUME_DOWN:
                if (m_show_volume_adj)
                {
                    CPlayer::GetInstance().MusicControl(Command::VOLUME_DOWN, theApp.m_nc_setting_data.volum_step);
                    return;
                }
                break;

            case BTN_PROGRESS:
            {
                int ckick_pos = point.x - m_buttons[BTN_PROGRESS].rect.left;
                double progress = static_cast<double>(ckick_pos) / m_buttons[BTN_PROGRESS].rect.Width();
                CPlayer::GetInstance().SeekTo(progress);
            }
            return;

            case BTN_FIND:
                m_buttons[BTN_FIND].hover = false;
                theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_FIND);
                return;
                break;

            case BTN_FULL_SCREEN:
                m_buttons[BTN_FULL_SCREEN].hover = false;
                theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_FULL_SCREEN);
                return;

            case BTN_MENU:
            {
                CPoint point(m_buttons[BTN_MENU].rect.left, m_buttons[BTN_MENU].rect.bottom);
                theApp.m_pMainWnd->SendMessage(WM_MAIN_MENU_POPEDUP, (WPARAM)&point);
            }

            default:
                break;
            }
        }
    }

}

CRect CPlayerUIBase::GetThumbnailClipArea()
{
    //获取菜单栏的高度
    int menu_bar_height = 0;
    if (m_ui_data.show_menu_bar && m_ui_data.show_window_frame)
    {
        menu_bar_height = CCommon::GetMenuBarHeight(theApp.m_pMainWnd->GetSafeHwnd());
        if (menu_bar_height == 0)
            menu_bar_height = theApp.DPI(20);
        else
            menu_bar_height += theApp.DPI(1);
    }

    CRect thumbnail_rect = DrawAreaToClient(m_draw_data.thumbnail_rect, m_draw_rect);
    thumbnail_rect.MoveToY(thumbnail_rect.top + menu_bar_height);
    return thumbnail_rect;
}

void CPlayerUIBase::UpdateRepeatModeToolTip()
{
    SetRepeatModeToolTipText();
    UpdateMouseToolTip(BTN_REPETEMODE, m_repeat_mode_tip);
}

void CPlayerUIBase::UpdateSongInfoToolTip()
{
    SetSongInfoToolTipText();
    UpdateMouseToolTip(BTN_INFO, m_info_tip);

    SetCoverToolTipText();
    UpdateMouseToolTip(BTN_COVER, m_cover_tip);
}

void CPlayerUIBase::UpdatePlayPauseButtonTip()
{
    if (CPlayer::GetInstance().IsPlaying() && !CPlayer::GetInstance().IsError())
        UpdateMouseToolTip(BTN_PLAY_PAUSE, CCommon::LoadText(IDS_PAUSE));
    else
        UpdateMouseToolTip(BTN_PLAY_PAUSE, CCommon::LoadText(IDS_PLAY));
}

void CPlayerUIBase::UpdateFullScreenTip()
{
    if (m_ui_data.full_screen)
        UpdateMouseToolTip(BTN_FULL_SCREEN, CCommon::LoadText(IDS_EXIT_FULL_SCREEN, _T(" (F11)")));
    else
        UpdateMouseToolTip(BTN_FULL_SCREEN, CCommon::LoadText(IDS_FULL_SCREEN, _T(" (F11)")));
}

void CPlayerUIBase::UpdateTitlebarBtnToolTip()
{
    if (theApp.m_pMainWnd->IsZoomed())
        UpdateMouseToolTip(BTN_MAXIMIZE, CCommon::LoadText(IDS_RESTORE));
    else
        UpdateMouseToolTip(BTN_MAXIMIZE, CCommon::LoadText(IDS_MAXIMIZE));
}

bool CPlayerUIBase::SetCursor()
{
    if (m_buttons[BTN_PROGRESS].hover)
    {
        ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(32649)));
        return true;
    }
    return false;
}

void CPlayerUIBase::MouseLeave()
{
    for (auto& btn : m_buttons)
    {
        btn.second.hover = false;
        btn.second.pressed = false;
    }
}

void CPlayerUIBase::ClearBtnRect()
{
    for (auto& btn : m_buttons)
    {
        btn.second.rect = CRect();
    }
}

void CPlayerUIBase::PreDrawInfo()
{
    //设置颜色
    m_colors = CPlayerUIHelper::GetUIColors(theApp.m_app_setting_data.theme_color, theApp.m_app_setting_data.dark_mode, IsDrawBackgroundAlpha());

    //设置绘制的矩形区域
    SetDrawRect();
}

void CPlayerUIBase::SetDrawRect()
{
    m_draw_rect = CRect(0, 0, m_ui_data.draw_area_width, m_ui_data.draw_area_height);
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

void CPlayerUIBase::DrawSongInfo(CRect rect, bool reset)
{
    //绘制播放状态
    CRect rc_tmp{ rect };
    if (!theApp.m_ui_data.always_show_statusbar)
    {
        CString play_state_str = CPlayer::GetInstance().GetPlayingState().c_str();
        //m_draw.GetDC()->SelectObject(theApp.m_pMainWnd->GetFont());
        rc_tmp.right = rc_tmp.left + m_draw.GetTextExtent(play_state_str).cx + DPI(4);
        m_draw.DrawWindowText(rc_tmp, play_state_str, m_colors.color_text_lable);
    }
    else     //如果总是显示状态栏，由于状态栏中已经有播放状态了，因此这里不显示播放状态
    {
        rc_tmp.right = rc_tmp.left;
    }

    //绘制歌曲序号
    rc_tmp.MoveToX(rc_tmp.right);
    rc_tmp.right = rc_tmp.left + DPI(30);
    wchar_t buff[128];
    swprintf_s(buff, sizeof(buff) / 2, L"%.3d", CPlayer::GetInstance().GetIndex() + 1);
    m_draw.DrawWindowText(rc_tmp, buff, m_colors.color_text_2);

    //绘制标识
    wstring tag_str;
    if (CPlayer::GetInstance().GetCurrentSongInfo().is_cue)
        tag_str = L"cue";
    else if (CPlayer::GetInstance().IsMidi())
        tag_str = L"midi";
    else if (CPlayer::GetInstance().IsOsuFile())
        tag_str = L"osu!";
    int available_width = rect.right - rc_tmp.right;
    if (available_width >= DPI(50) && !tag_str.empty())
    {
        int width = m_draw.GetTextExtent(tag_str.c_str()).cx + DPI(4);
        rc_tmp.MoveToX(rc_tmp.right);
        rc_tmp.right = rc_tmp.left + width;
        DrawPlayTag(rc_tmp, tag_str.c_str());
    }

    //绘制播放速度
    if (std::fabs(CPlayer::GetInstance().GetSpeed() - 1) > 1e-3)
    {
        wchar_t buff[64];
        swprintf_s(buff, L"%.2f", CPlayer::GetInstance().GetSpeed());
        tag_str = buff;
        if (!tag_str.empty() && tag_str.back() == L'0')
            tag_str.pop_back();
        tag_str.push_back(L'X');
        available_width = rect.right - rc_tmp.right;
        if (available_width >= DPI(50))
        {
            int width = m_draw.GetTextExtent(tag_str.c_str()).cx + DPI(4);
            rc_tmp.MoveToX(rc_tmp.right + DPI(2));
            rc_tmp.right = rc_tmp.left + width;
            DrawPlayTag(rc_tmp, tag_str.c_str());
        }
    }

    //绘制文件名
    rc_tmp.MoveToX(rc_tmp.right + DPI(4));
    rc_tmp.right = rect.right;
    if (rc_tmp.Width() >= DPI(4))
    {
        static CDrawCommon::ScrollInfo scroll_info1;
        m_draw.DrawScrollText(rc_tmp, CPlayer::GetInstance().GetDisplayName().c_str(), m_colors.color_text, GetScrollTextPixel(), false, scroll_info1, reset);
    }
}

void CPlayerUIBase::DrawControlBarBtn(CRect rect, UIButton& btn, const IconRes& icon)
{
    rect.DeflateRect(DPI(2), DPI(2));
    DrawUIButton(rect, btn, icon);
}

void CPlayerUIBase::DrawPlayTag(CRect rect, LPCTSTR str_text)
{
    m_draw.SetDrawArea(rect);
    CRect rc_border = rect;
    rc_border.top += DPI(2);
    rc_border.bottom -= DPI(1);
    m_draw.DrawRectOutLine(rc_border, m_colors.color_text, DPI(1), false);
    m_draw.DrawWindowText(rect, str_text, m_colors.color_text, Alignment::CENTER);
}

void CPlayerUIBase::DrawToolBar(CRect rect, bool draw_translate_button)
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

    if (!theApp.m_app_setting_data.button_round_corners)
        m_draw.FillAlphaRect(rect, m_colors.color_control_bar_back, alpha);
    else
    {
        m_draw.SetDrawArea(rect);
        m_draw.DrawRoundRect(rect, m_colors.color_control_bar_back, theApp.DPI(4), alpha);
    }

    CRect rc_tmp = rect;

    //绘制循环模式
    rc_tmp.right = rect.left + rect.Height();
    IconRes* pIcon = GetRepeatModeIcon();
    if (pIcon != nullptr)
        DrawControlBarBtn(rc_tmp, m_buttons[BTN_REPETEMODE], *pIcon);

    //绘制设置按钮
    rc_tmp.MoveToX(rc_tmp.right);
    DrawControlBarBtn(rc_tmp, m_buttons[BTN_SETTING], theApp.m_icon_set.setting);

    //绘制均衡器按钮
    rc_tmp.MoveToX(rc_tmp.right);
    DrawControlBarBtn(rc_tmp, m_buttons[BTN_EQ], theApp.m_icon_set.eq);

    //绘制切换界面按钮
    rc_tmp.MoveToX(rc_tmp.right);
    DrawControlBarBtn(rc_tmp, m_buttons[BTN_SKIN], theApp.m_icon_set.skin);

    //绘制迷你模式按钮
    if (rect.Width() >= DPI(190))
    {
        rc_tmp.MoveToX(rc_tmp.right);
        //m_buttons[BTN_MINI].enable = !theApp.m_ui_data.full_screen;
        DrawControlBarBtn(rc_tmp, m_buttons[BTN_MINI], theApp.m_icon_set.mini);
    }
    else
    {
        m_buttons[BTN_MINI].rect = CRect();
    }

    //绘制曲目信息按钮
    if (rect.Width() >= DPI(214))
    {
        rc_tmp.MoveToX(rc_tmp.right);
        DrawControlBarBtn(rc_tmp, m_buttons[BTN_INFO], theApp.m_icon_set.info);
    }
    else
    {
        m_buttons[BTN_INFO].rect = CRect();
    }

    //绘制查找按钮
    if (rect.Width() >= DPI(238))
    {
        rc_tmp.MoveToX(rc_tmp.right);
        DrawControlBarBtn(rc_tmp, m_buttons[BTN_FIND], theApp.m_icon_set.find_songs);
    }
    else
    {
        m_buttons[BTN_FIND].rect = CRect();
    }

    //绘制AB重复按钮
    if (rect.Width() >= DPI(262))
    {
        rc_tmp.MoveToX(rc_tmp.right);
        CRect rc_btn = rc_tmp;
        rc_btn.DeflateRect(DPI(2), DPI(2));
        DrawABRepeatButton(rc_btn);
    }
    else
    {
        m_buttons[BTN_AB_REPEAT].rect = CRect();
    }

    //绘制翻译按钮
    if (draw_translate_button && rect.Width() >= DPI(286))
    {
        rc_tmp.MoveToX(rc_tmp.right);
        CRect translate_rect = rc_tmp;
        translate_rect.DeflateRect(DPI(2), DPI(2));
        DrawTranslateButton(translate_rect);
    }
    else
    {
        m_buttons[BTN_TRANSLATE].rect = CRect();
    }

    //绘制桌面歌词按钮
    if (rect.Width() >= DPI(310))
    {
        rc_tmp.MoveToX(rc_tmp.right);
        CRect translate_rect = rc_tmp;
        translate_rect.DeflateRect(DPI(2), DPI(2));
        DrawTextButton(translate_rect, m_buttons[BTN_LRYIC], CCommon::LoadText(IDS_LRC), theApp.m_lyric_setting_data.show_desktop_lyric);
    }
    else
    {
        m_buttons[BTN_LRYIC].rect = CRect();
    }

    rc_tmp.left = rc_tmp.right = rect.right;

    //显示<<<<
    if (rect.Width() >= DPI(361))
    {
        int progress;
        Time time{ CPlayer::GetInstance().GetCurrentPosition() };
        if (CPlayer::GetInstance().IsMidi())
        {
            ////progress = (CPlayer::GetInstance().GetMidiInfo().midi_position % 16 + 1) *1000 / 16;
            //if (CPlayer::GetInstance().GetMidiInfo().tempo == 0)
            //  progress = 0;
            //else
            //  progress = (time.toInt() * 1000 / CPlayer::GetInstance().GetMidiInfo().tempo % 4 + 1) * 250;
            progress = (CPlayer::GetInstance().GetMidiInfo().midi_position % 4 + 1) * 250;
        }
        else
        {
            progress = (time.sec % 4 * 1000 + time.msec) / 4;
        }
        rc_tmp.right = rc_tmp.left;
        rc_tmp.left = rc_tmp.right - DPI(44);
        m_draw.DrawWindowText(rc_tmp, _T("<<<<"), m_colors.color_text, m_colors.color_text_2, progress);
    }


    //显示音量
    CString vol_str;
    vol_str.Format(_T(": %d%%"), CPlayer::GetInstance().GetVolume());
    vol_str = CCommon::LoadText(IDS_VOLUME) + vol_str;
    int width = m_draw.GetTextExtent(vol_str).cx;
    if (width <= 0 || width > DPI(72))
        width = DPI(72);

    rc_tmp.right = rc_tmp.left;
    rc_tmp.left = rc_tmp.right - width - Margin();
    DrawVolumeButton(rc_tmp, vol_str);
}

CRect CPlayerUIBase::DrawAreaToClient(CRect rect, CRect draw_area)
{
    //rect.MoveToXY(rect.left + draw_area.left, rect.top + draw_area.top);
    return rect;
}

CRect CPlayerUIBase::ClientAreaToDraw(CRect rect, CRect draw_area)
{
    //rect.MoveToXY(rect.left - draw_area.left, rect.top - draw_area.top);
    return rect;
}

void CPlayerUIBase::DrawUIButton(CRect rect, UIButton& btn, const IconRes& icon)
{
    btn.rect = DrawAreaToClient(rect, m_draw_rect);

    if (btn.pressed && btn.enable)
        rect.MoveToXY(rect.left + theApp.DPI(1), rect.top + theApp.DPI(1));

    CRect rc_tmp = rect;
    //rc_tmp.DeflateRect(DPI(2), DPI(2));
    m_draw.SetDrawArea(rc_tmp);

    //绘制的是否为关闭按钮（关闭按钮需要特别处理）
    bool is_close_btn = (&btn == &m_buttons[BTN_APP_CLOSE]);

    //绘制背景
    if (btn.pressed || btn.hover)
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
            else
                back_color = m_colors.color_button_hover;
        }
        if (!theApp.m_app_setting_data.button_round_corners)
            m_draw.FillAlphaRect(rc_tmp, back_color, alpha);
        else
            m_draw.DrawRoundRect(rc_tmp, back_color, theApp.DPI(3), alpha);
    }

    rc_tmp = rect;
    //使图标在矩形中居中
    CSize icon_size = icon.GetSize(IsDrawLargeIcon());
    rc_tmp.left = rect.left + (rect.Width() - icon_size.cx) / 2;
    rc_tmp.top = rect.top + (rect.Height() - icon_size.cy) / 2;
    rc_tmp.right = rc_tmp.left + icon_size.cx;
    rc_tmp.bottom = rc_tmp.top + icon_size.cy;

    bool is_light_icon = (theApp.m_app_setting_data.dark_mode || (is_close_btn && (btn.pressed || btn.hover)));
    const HICON& hIcon = icon.GetIcon(!is_light_icon, IsDrawLargeIcon());
    m_draw.DrawIcon(hIcon, rc_tmp.TopLeft(), rc_tmp.Size());

}

void CPlayerUIBase::DrawControlButton(CRect rect, UIButton& btn, const IconRes& icon)
{
    if (btn.pressed)
        rect.MoveToXY(rect.left + theApp.DPI(1), rect.top + theApp.DPI(1));

    CRect rc_tmp = rect;
    m_draw.SetDrawArea(rc_tmp);

    if (btn.pressed || btn.hover)
    {
        BYTE alpha;
        if (IsDrawBackgroundAlpha())
            alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency) * 2 / 3;
        else
            alpha = 255;
        COLORREF back_color{};
        if (btn.pressed)
            back_color = m_colors.color_button_pressed;
        else
            back_color = m_colors.color_button_hover;
        if (!theApp.m_app_setting_data.button_round_corners)
            m_draw.FillAlphaRect(rc_tmp, back_color, alpha);
        else
            m_draw.DrawRoundRect(rc_tmp, back_color, theApp.DPI(4), alpha);
    }

    //else if (!theApp.m_app_setting_data.dark_mode)
    //  m_draw.FillAlphaRect(rc_tmp, m_colors.color_button_back, alpha);

    btn.rect = DrawAreaToClient(rc_tmp, m_draw_rect);

    rc_tmp = rect;
    //使图标在矩形中居中
    CSize icon_size = icon.GetSize(IsDrawLargeIcon());
    rc_tmp.left = rect.left + (rect.Width() - icon_size.cx) / 2;
    rc_tmp.top = rect.top + (rect.Height() - icon_size.cy) / 2;
    rc_tmp.right = rc_tmp.left + icon_size.cx;
    rc_tmp.bottom = rc_tmp.top + icon_size.cy;

    const HICON& hIcon = icon.GetIcon(!theApp.m_app_setting_data.dark_mode, IsDrawLargeIcon());
    m_draw.DrawIcon(hIcon, rc_tmp.TopLeft(), rc_tmp.Size());
}

void CPlayerUIBase::DrawTextButton(CRect rect, UIButton& btn, LPCTSTR text, bool back_color)
{
    if (btn.enable)
    {
        if (btn.pressed)
            rect.MoveToXY(rect.left + theApp.DPI(1), rect.top + theApp.DPI(1));

        BYTE alpha;
        if (IsDrawBackgroundAlpha())
            alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency) * 2 / 3;
        else
            alpha = 255;
        if (btn.pressed || btn.hover || back_color)
        {
            COLORREF background_color{};
            if (btn.pressed)
            {
                background_color = m_colors.color_button_pressed;
            }
            else if (btn.hover)
            {
                background_color = m_colors.color_button_hover;
            }
            else if (back_color)
            {
                background_color = m_colors.color_button_back;
            }

            if (!theApp.m_app_setting_data.button_round_corners)
            {
                m_draw.FillAlphaRect(rect, background_color, alpha);
            }
            else
            {
                m_draw.SetDrawArea(rect);
                m_draw.DrawRoundRect(rect, background_color, theApp.DPI(3), alpha);
            }
        }
        m_draw.DrawWindowText(rect, text, m_colors.color_text, Alignment::CENTER);
    }
    else
    {
        m_draw.DrawWindowText(rect, text, GRAY(200), Alignment::CENTER);
    }
    btn.rect = DrawAreaToClient(rect, m_draw_rect);
}

void CPlayerUIBase::AddMouseToolTip(BtnKey btn, LPCTSTR str)
{
    m_tool_tip.AddTool(m_pMainWnd, str, m_buttons[btn].rect, btn + GetClassId());
}

void CPlayerUIBase::UpdateMouseToolTip(BtnKey btn, LPCTSTR str)
{
    m_tool_tip.UpdateTipText(str, m_pMainWnd, btn + GetClassId());
}

void CPlayerUIBase::UpdateToolTipPosition()
{
    for (const auto& btn : m_buttons)
    {
        m_tool_tip.SetToolRect(m_pMainWnd, btn.first + GetClassId(), btn.second.rect);
    }
}

void CPlayerUIBase::SetRepeatModeToolTipText()
{
    m_repeat_mode_tip = CCommon::LoadText(IDS_REPEAT_MODE, _T(" (M): "));
    switch (CPlayer::GetInstance().GetRepeatMode())
    {
    case RepeatMode::RM_PLAY_ORDER:
        m_repeat_mode_tip += CCommon::LoadText(IDS_PLAY_ODER);
        break;
    case RepeatMode::RM_LOOP_PLAYLIST:
        m_repeat_mode_tip += CCommon::LoadText(IDS_LOOP_PLAYLIST);
        break;
    case RepeatMode::RM_LOOP_TRACK:
        m_repeat_mode_tip += CCommon::LoadText(IDS_LOOP_TRACK);
        break;
    case RepeatMode::RM_PLAY_SHUFFLE:
        m_repeat_mode_tip += CCommon::LoadText(IDS_PLAY_SHUFFLE);
        break;
    case RepeatMode::RM_PLAY_RANDOM:
        m_repeat_mode_tip += CCommon::LoadText(IDS_PLAY_RANDOM);
        break;
    case RepeatMode::RM_PLAY_TRACK:
        m_repeat_mode_tip += CCommon::LoadText(IDS_PLAY_TRACK);
        break;
    }
}

void CPlayerUIBase::SetSongInfoToolTipText()
{
    const SongInfo& songInfo = CPlayer::GetInstance().GetCurrentSongInfo();

    m_info_tip = CCommon::LoadText(IDS_SONG_INFO, _T(" (Ctrl+N)\r\n"));

    m_info_tip += CCommon::LoadText(IDS_TITLE, _T(": "));
    m_info_tip += songInfo.GetTitle().c_str();
    m_info_tip += _T("\r\n");

    m_info_tip += CCommon::LoadText(IDS_ARTIST, _T(": "));
    m_info_tip += songInfo.GetArtist().c_str();
    m_info_tip += _T("\r\n");

    m_info_tip += CCommon::LoadText(IDS_ALBUM, _T(": "));
    m_info_tip += songInfo.GetAlbum().c_str();
    //m_info_tip += _T("\r\n");

    //m_info_tip += CCommon::LoadText(IDS_BITRATE, _T(": "));
    //CString strTmp;
    //strTmp.Format(_T("%d kbps"), songInfo.bitrate);
    //m_info_tip += strTmp;
}

void CPlayerUIBase::SetCoverToolTipText()
{
    if (theApp.m_nc_setting_data.show_cover_tip && theApp.m_app_setting_data.show_album_cover && CPlayer::GetInstance().AlbumCoverExist())
    {
        m_cover_tip = CCommon::LoadText(IDS_ALBUM_COVER, _T(": "));

        if (CPlayer::GetInstance().IsInnerCover())
        {
            m_cover_tip += CCommon::LoadText(IDS_INNER_ALBUM_COVER_TIP_INFO);
            switch (CPlayer::GetInstance().GetAlbumCoverType())
            {
            case 0:
                m_cover_tip += _T("jpg");
                break;
            case 1:
                m_cover_tip += _T("png");
                break;
            case 2:
                m_cover_tip += _T("gif");
                break;
            }
        }
        else
        {
            m_cover_tip += CCommon::LoadText(IDS_OUT_IMAGE, _T("\r\n"));
            m_cover_tip += CPlayer::GetInstance().GetAlbumCoverPath().c_str();
        }
    }
    else
    {
        m_cover_tip.Empty();
    }
}

int CPlayerUIBase::Margin() const
{
    int margin = m_layout.margin;
    if (m_ui_data.full_screen)
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
    if (m_ui_data.full_screen)
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
    else if (!m_ui_data.show_window_frame)
        info_height += m_layout.titlabar_height;

    return info_height;
}

bool CPlayerUIBase::PointInControlArea(CPoint point) const
{
    bool point_in_control = false;
    for (const auto& btn : m_buttons)
    {
        if (btn.first != BTN_COVER)
            point_in_control |= (btn.second.rect.PtInRect(point) != FALSE);
    }
    return point_in_control;
}

bool CPlayerUIBase::PointInTitlebarArea(CPoint point) const
{
    if (m_ui_data.show_window_frame || m_ui_data.full_screen)
        return false;

    CRect rect_titlebar = m_draw_rect;
    rect_titlebar.bottom = rect_titlebar.top + m_layout.titlabar_height;
    return (rect_titlebar.PtInRect(point) != FALSE);
}

bool CPlayerUIBase::PointInAppIconArea(CPoint point) const
{
    if (m_ui_data.show_window_frame || m_ui_data.full_screen)
        return false;

    CRect rect_app_icon{};
    rect_app_icon.right = rect_app_icon.bottom = m_layout.titlabar_height;
    return (rect_app_icon.PtInRect(point) != FALSE);
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
    return !m_ui_data.show_window_frame && !m_ui_data.full_screen;
}

wstring CPlayerUIBase::GetDisplayFormatString()
{
    int chans = CPlayer::GetInstance().GetChannels();
    int freq = CPlayer::GetInstance().GetFreq();
    CString chans_str;
    if (chans == 1)
        chans_str = CCommon::LoadText(IDS_MONO);
    else if (chans == 2)
        chans_str = CCommon::LoadText(IDS_STEREO);
    else if (chans == 6)
        chans_str = CCommon::LoadText(_T("5.1 "), IDS_CHANNEL);
    else if (chans == 8)
        chans_str = CCommon::LoadText(_T("7.1 "), IDS_CHANNEL);
    else if (chans > 2)
        chans_str.Format(CCommon::LoadText(_T("%d "), IDS_CHANNEL), chans);
    wchar_t buff[64];
    if (!CPlayer::GetInstance().IsMidi())
        swprintf_s(buff, L"%s %.1fkHz %dkbps %s", CPlayer::GetInstance().GetCurrentFileType().c_str(), freq / 1000.0f, CPlayer::GetInstance().GetCurrentSongInfo().bitrate, chans_str.GetString());
    else
        swprintf_s(buff, L"%s %.1fkHz %s", CPlayer::GetInstance().GetCurrentFileType().c_str(), freq / 1000.0f, chans_str.GetString());
    return buff;

}

int CPlayerUIBase::DPI(int pixel)
{
    if (m_ui_data.full_screen)
        return static_cast<int>(theApp.DPI(pixel) * CONSTVAL::FULL_SCREEN_ZOOM_FACTOR);
    else
        return theApp.DPI(pixel);
}

int CPlayerUIBase::DPI(double pixel)
{
    if (m_ui_data.full_screen)
        return static_cast<int>(theApp.DPI(pixel) * CONSTVAL::FULL_SCREEN_ZOOM_FACTOR);
    else
        return theApp.DPI(pixel);
}

double CPlayerUIBase::DPIDouble(double pixel)
{
    double rtn_val = static_cast<double>(theApp.GetDPI()) * pixel / 96;
    if (m_ui_data.full_screen)
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

bool CPlayerUIBase::IsDrawLargeIcon()
{
    return theApp.m_ui_data.full_screen;
}

bool CPlayerUIBase::IsDrawNarrowMode()
{
    if (!m_ui_data.show_playlist)
        return false;
    else
        return m_ui_data.narrow_mode;
}

void CPlayerUIBase::DrawVolumnAdjBtn()
{
    if (m_show_volume_adj)
    {
        CRect volume_down_rect = ClientAreaToDraw(m_buttons[BTN_VOLUME_DOWN].rect, m_draw_rect);
        CRect volume_up_rect = ClientAreaToDraw(m_buttons[BTN_VOLUME_UP].rect, m_draw_rect);

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
            m_draw.SetDrawArea(rc_buttons);
            m_draw.DrawRoundRect(rc_buttons, m_colors.color_text_2, theApp.DPI(3), alpha);
            if (m_buttons[BTN_VOLUME_UP].pressed || m_buttons[BTN_VOLUME_UP].hover)
                m_draw.DrawRoundRect(volume_up_rect, btn_up_back_color, theApp.DPI(3), alpha);
            if (m_buttons[BTN_VOLUME_DOWN].pressed || m_buttons[BTN_VOLUME_DOWN].hover)
                m_draw.DrawRoundRect(volume_down_rect, btn_down_back_color, theApp.DPI(3), alpha);
        }

        if (m_buttons[BTN_VOLUME_DOWN].pressed)
            volume_down_rect.MoveToXY(volume_down_rect.left + theApp.DPI(1), volume_down_rect.top + theApp.DPI(1));
        if (m_buttons[BTN_VOLUME_UP].pressed)
            volume_up_rect.MoveToXY(volume_up_rect.left + theApp.DPI(1), volume_up_rect.top + theApp.DPI(1));

        m_draw.DrawWindowText(volume_down_rect, L"-", ColorTable::WHITE, Alignment::CENTER);
        m_draw.DrawWindowText(volume_up_rect, L"+", ColorTable::WHITE, Alignment::CENTER);
    }
}

void CPlayerUIBase::DrawControlBar(CRect rect)
{
    bool progress_on_top = rect.Width() < m_progress_on_top_threshold;
    const int progress_height = DPI(4);
    CRect progress_rect;
    if (progress_on_top)
    {
        progress_rect = rect;
        int progressbar_height = rect.Height() / 3;
        progress_rect.bottom = progress_rect.top + progressbar_height;
        DrawProgressBar(progress_rect);
        rect.top = progress_rect.bottom;
    }

    //绘制播放控制按钮
    const int btn_width = DPI(36);
    const int btn_height = min(rect.Height(), btn_width);

    CRect rc_btn{ CPoint(rect.left, rect.top + (rect.Height() - btn_height) / 2), CSize(btn_width, btn_height) };
    DrawControlButton(rc_btn, m_buttons[BTN_STOP], theApp.m_icon_set.stop_l);

    rc_btn.MoveToX(rc_btn.right);
    DrawControlButton(rc_btn, m_buttons[BTN_PREVIOUS], theApp.m_icon_set.previous_l);

    rc_btn.MoveToX(rc_btn.right);
    IconRes& paly_pause_icon = CPlayer::GetInstance().IsPlaying() ? theApp.m_icon_set.pause_l : theApp.m_icon_set.play_l;
    DrawControlButton(rc_btn, m_buttons[BTN_PLAY_PAUSE], paly_pause_icon);

    rc_btn.MoveToX(rc_btn.right);
    DrawControlButton(rc_btn, m_buttons[BTN_NEXT], theApp.m_icon_set.next_l);

    int progressbar_left = rc_btn.right + Margin();

    //绘制右侧按钮
    const int btn_side = DPI(24);
    rc_btn.right = rect.right;
    rc_btn.left = rc_btn.right - btn_side;
    rc_btn.top = rect.top + (rect.Height() - btn_side) / 2;
    rc_btn.bottom = rc_btn.top + btn_side;
    DrawControlBarBtn(rc_btn, m_buttons[BTN_SHOW_PLAYLIST], theApp.m_icon_set.show_playlist);

    rc_btn.MoveToX(rc_btn.left - btn_side);
    m_buttons[BTN_SELECT_FOLDER].enable = !CPlayer::GetInstance().m_loading;
    DrawControlBarBtn(rc_btn, m_buttons[BTN_SELECT_FOLDER], theApp.m_icon_set.media_lib);

    rc_btn.MoveToX(rc_btn.left - btn_side);
    if (CPlayer::GetInstance().IsFavourite())
        DrawControlBarBtn(rc_btn, m_buttons[BTN_FAVOURITE], theApp.m_icon_set.heart);
    else
        DrawControlBarBtn(rc_btn, m_buttons[BTN_FAVOURITE], theApp.m_icon_set.favourite);

    if (!progress_on_top)
    {
        progress_rect = rect;
        progress_rect.left = progressbar_left;
        progress_rect.right = rc_btn.left - Margin();
        DrawProgressBar(progress_rect);
    }
}

void CPlayerUIBase::DrawProgressBar(CRect rect)
{
    //绘制播放时间
    bool draw_progress_time{ rect.Width() > DPI(110) };
    CRect rc_time = rect;
    if (draw_progress_time)
    {
        wstring strTime = CPlayer::GetInstance().GetTimeString();

        m_draw.SetFont(&theApp.m_font_set.font8.GetFont(m_ui_data.full_screen));
        CSize strSize = m_draw.GetTextExtent(strTime.c_str());
        rc_time.left = rc_time.right - strSize.cx;
        //rc_time.InflateRect(0, DPI(2));
        rc_time.top -= DPI(1);
        m_draw.DrawWindowText(rc_time, strTime.c_str(), m_colors.color_text);
    }

    //绘制进度条
    const int progress_height = DPI(4);
    CRect progress_rect = rect;
    if (draw_progress_time)
        progress_rect.right = rc_time.left - Margin();
    progress_rect.top = rect.top + (rect.Height() - progress_height) / 2;
    progress_rect.bottom = progress_rect.top + progress_height;
    DrawProgess(progress_rect);
}

void CPlayerUIBase::DrawProgess(CRect rect)
{
    if (IsDrawBackgroundAlpha())
        m_draw.FillAlphaRect(rect, m_colors.color_spectrum_back, ALPHA_CHG(theApp.m_app_setting_data.background_transparency) * 2 / 3);
    else
        m_draw.FillRect(rect, m_colors.color_spectrum_back);

    m_buttons[BTN_PROGRESS].rect = DrawAreaToClient(rect, m_draw_rect);
    m_buttons[BTN_PROGRESS].rect.InflateRect(0, DPI(3));

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
        m_draw.SetFont(&theApp.m_font_set.font8.GetFont(theApp.m_ui_data.full_screen));      //AB重复使用小一号字体，即播放时间的字体

        double a_point_progres = static_cast<double>(CPlayer::GetInstance().GetARepeatPosition().toInt()) / CPlayer::GetInstance().GetSongLength();
        double b_point_progres = static_cast<double>(CPlayer::GetInstance().GetBRepeatPosition().toInt()) / CPlayer::GetInstance().GetSongLength();
        CRect rect_draw = rect;
        rect_draw.bottom += DPI(12);
        m_draw.SetDrawArea(rect_draw);
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
}

void CPlayerUIBase::DrawTranslateButton(CRect rect)
{
    m_buttons[BTN_TRANSLATE].enable = CPlayer::GetInstance().m_Lyrics.IsTranslated();
    DrawTextButton(rect, m_buttons[BTN_TRANSLATE], CCommon::LoadText(IDS_TRAS), m_ui_data.show_translate);
}

int CPlayerUIBase::DrawTopRightIcons(bool always_show_full_screen)
{
    if (!m_ui_data.show_window_frame && !m_ui_data.full_screen)
        return 0;

    int total_width = 0;
    const int icon_size = DPI(28);
    //绘制“全屏”图标
    if (always_show_full_screen || (!m_ui_data.show_playlist || m_draw_rect.Width() > m_layout.width_threshold || m_ui_data.full_screen))
    {
        total_width = icon_size;

        CRect rc_tmp;
        rc_tmp.right = m_draw_rect.right - EdgeMargin(true);
        rc_tmp.top = EdgeMargin(false);
        rc_tmp.bottom = rc_tmp.top + icon_size;
        rc_tmp.left = rc_tmp.right - icon_size;
        IconRes& icon{ m_ui_data.full_screen ? theApp.m_icon_set.full_screen : theApp.m_icon_set.full_screen1 };
        DrawControlButton(rc_tmp, m_buttons[BTN_FULL_SCREEN], icon);
        total_width += Margin();
    }
    else
    {
        m_buttons[BTN_FULL_SCREEN].rect.SetRectEmpty();
    }

    //绘制“主菜单”图标
    if (!m_ui_data.show_menu_bar || m_ui_data.full_screen)
    {
        CRect rc_tmp;
        rc_tmp.right = m_draw_rect.right - total_width - EdgeMargin(true);
        rc_tmp.top = EdgeMargin(false);
        rc_tmp.bottom = rc_tmp.top + icon_size;
        rc_tmp.left = rc_tmp.right - icon_size;
        DrawControlButton(rc_tmp, m_buttons[BTN_MENU], theApp.m_icon_set.menu);
        total_width += icon_size;
        total_width += Margin();
    }
    else
    {
        m_buttons[BTN_MENU].rect.SetRectEmpty();
    }

    return total_width;
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
    m_draw.SetFont(&theApp.m_font_set.font8.GetFont(m_ui_data.full_screen));
    m_draw.DrawWindowText(rc_tmp, buff, m_colors.color_text);
    m_draw.SetFont(&theApp.m_font_set.font9.GetFont(theApp.m_ui_data.full_screen));
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
        rc_fps.left = rc_fps.right - DPI(36);
        CFont* pOldFont = m_draw.SetFont(&theApp.m_font_set.font8.GetFont(theApp.m_ui_data.full_screen));
        CString str_info;
        str_info.Format(_T("%dFPS"), theApp.m_fps);
        m_draw.DrawWindowText(rc_fps, str_info, m_colors.color_text);
        m_draw.SetFont(pOldFont);

        rect.right = rc_fps.left;
    }

    rect.DeflateRect(DPI(4), 0);

    //显示播放列表载入状态
    if (CPlayer::GetInstance().m_loading)
    {
        int progress_percent = CPlayer::GetInstance().m_thread_info.process_percent;
        //绘制进度右侧的进度百分比
        CRect rc_percent{ rect };
        rc_percent.left = rc_percent.right - theApp.DPI(24);
        CFont* pOldFont = m_draw.SetFont(&theApp.m_font_set.font8.GetFont(theApp.m_ui_data.full_screen));
        CString str_info;
        str_info.Format(_T("%d%%"), progress_percent);
        m_draw.DrawWindowText(rc_percent, str_info, m_colors.color_text);
        m_draw.SetFont(pOldFont);

        rect.right = rc_percent.left - theApp.DPI(4);
        CRect rc_tmp{ rect };
        //绘制进度条（进度条里面包含10格）
        int bar_width = DPI(4);     //每一格的宽度
        int progress_width = (bar_width + DPI(2)) * 10 + DPI(2) * 2;
        rc_tmp.left = rect.right - progress_width;
        CRect rc_progress{ rc_tmp };
        rc_progress.DeflateRect(0, DPI(4));
        m_draw.SetDrawArea(rc_progress);
        m_draw.DrawRectOutLine(rc_progress, m_colors.color_text, DPI(1), false);
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
        static CDrawCommon::ScrollInfo scroll_info0;
        CString info;
        info = CCommon::LoadTextFormat(IDS_PLAYLIST_INIT_INFO, { CPlayer::GetInstance().GetSongNum() });
        m_draw.DrawScrollText(rc_tmp, info, m_colors.color_text, GetScrollTextPixel(), false, scroll_info0, reset);
    }
    //显示AB重复状态
    else if (CPlayer::GetInstance().GetABRepeatMode() != CPlayer::AM_NONE)
    {
        CString info;
        if (CPlayer::GetInstance().GetABRepeatMode() == CPlayer::AM_A_SELECTED)
            info = CCommon::LoadTextFormat(IDS_AB_REPEAT_A_SELECTED, { CPlayer::GetInstance().GetARepeatPosition().toString(false) });
        else if (CPlayer::GetInstance().GetABRepeatMode() == CPlayer::AM_AB_REPEAT)
            info = CCommon::LoadTextFormat(IDS_AB_REPEAT_ON_INFO, { CPlayer::GetInstance().GetARepeatPosition().toString(false), CPlayer::GetInstance().GetBRepeatPosition().toString(false) });
        m_draw.DrawWindowText(rect, info, m_colors.color_text);
    }
    //显示媒体库更新状态
    else if (theApp.IsMeidaLibUpdating() && theApp.m_media_num_added > 0)
    {
        CString info = CCommon::LoadTextFormat(IDS_UPDATING_MEDIA_LIB_INFO, { theApp.m_media_num_added });
        static CDrawCommon::ScrollInfo scroll_info2;
        m_draw.DrawScrollText(rect, info, m_colors.color_text, GetScrollTextPixel(), false, scroll_info2, reset);
    }
    else
    {
        //显示播放信息
        wstring str_info;
        if (CPlayer::GetInstance().IsError())
        {
            str_info = CCommon::LoadText(IDS_PLAY_ERROR).GetString();
            str_info += L": ";
            str_info += CPlayer::GetInstance().GetErrorInfo();
        }
        else
        {
            str_info = CPlayer::GetInstance().GetPlayingState();
        }
        int text_width = m_draw.GetTextExtent(str_info.c_str()).cx;
        CRect rect_play{ rect };
        rect_play.right = rect_play.left + text_width + theApp.DPI(8);
        m_draw.DrawWindowText(rect_play, str_info.c_str(), m_colors.color_text);

        //显示下一个播放曲目
        if (theApp.m_app_setting_data.show_next_track)
        {
            static CString str_next_track_label = CCommon::LoadText(IDS_NEXT_TRACK, _T(": "));
            CRect rect_next_track{ rect };
            rect_next_track.left = rect_play.right;
            rect_next_track.right = rect_next_track.left + m_draw.GetTextExtent(str_next_track_label).cx;
            m_draw.DrawWindowText(rect_next_track, str_next_track_label.GetString(), m_colors.color_text);

            rect_next_track.left = rect_next_track.right;
            rect_next_track.right = rect.right;
            wstring str_next_song;
            SongInfo next_song = CPlayer::GetInstance().GetNextTrack();
            if (next_song.IsEmpty())
            {
                if (CPlayer::GetInstance().GetRepeatMode() == RM_PLAY_RANDOM || CPlayer::GetInstance().GetRepeatMode() == RM_PLAY_SHUFFLE)
                    str_next_song += CCommon::LoadText(IDS_RANDOM_TRACK).GetString();
                else
                    str_next_song += CCommon::LoadText(IDS_NONE).GetString();
            }
            else
            {
                str_next_song += CPlayListCtrl::GetDisplayStr(next_song, theApp.m_media_lib_setting_data.display_format);
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

    //绘制应用图标
    auto app_icon = theApp.m_icon_set.app;
    CRect rect_temp = rect;
    rect_temp.right = rect_temp.left + m_layout.titlabar_height;
    //使图标在矩形中居中
    CSize icon_size = app_icon.GetSize();
    CRect rc_icon{ rect_temp };
    rc_icon.left = rect_temp.left + (rect_temp.Width() - icon_size.cx) / 2;
    rc_icon.top = rect_temp.top + (rect_temp.Height() - icon_size.cy) / 2;
    rc_icon.right = rc_icon.left + icon_size.cx;
    rc_icon.bottom = rc_icon.top + icon_size.cy;
    m_draw.DrawIcon(app_icon.GetIcon(), rc_icon.TopLeft(), icon_size);

    //绘制右侧图标
    rect_temp = rect;
    rect_temp.left = rect_temp.right - theApp.DPI(30);
    //关闭图标
    DrawUIButton(rect_temp, m_buttons[BTN_APP_CLOSE], theApp.m_icon_set.app_close);
    //最大化/还原图标
    rect_temp.MoveToX(rect_temp.left - rect_temp.Width());
    auto max_icon = (theApp.m_pMainWnd->IsZoomed() ? theApp.m_icon_set.restore : theApp.m_icon_set.maximize);
    DrawUIButton(rect_temp, m_buttons[BTN_MAXIMIZE], max_icon);
    //最小化图标
    rect_temp.MoveToX(rect_temp.left - rect_temp.Width());
    DrawUIButton(rect_temp, m_buttons[BTN_MINIMIZE], theApp.m_icon_set.minimize);
    //全屏模式图标
    rect_temp.MoveToX(rect_temp.left - rect_temp.Width());
    DrawUIButton(rect_temp, m_buttons[BTN_FULL_SCREEN], theApp.m_icon_set.full_screen1);
    //迷你模式图标
    rect_temp.MoveToX(rect_temp.left - rect_temp.Width());
    DrawUIButton(rect_temp, m_buttons[BTN_MINI1], theApp.m_icon_set.mini);
    //主菜单图标
    rect_temp.MoveToX(rect_temp.left - rect_temp.Width());
    DrawUIButton(rect_temp, m_buttons[BTN_MENU], theApp.m_icon_set.menu);

    //绘制标题栏文本
    rect_temp.right = rect_temp.left;
    rect_temp.left = m_layout.titlabar_height;
    CMusicPlayerDlg* pMainWnd = dynamic_cast<CMusicPlayerDlg*>(theApp.m_pMainWnd);
    CString title;
    pMainWnd->GetWindowText(title);
    m_draw.DrawWindowText(rect_temp, title.GetString(), m_colors.color_text);
}

void CPlayerUIBase::DrawAlbumCover(CRect rect)
{
    if (theApp.m_app_setting_data.show_album_cover && CPlayer::GetInstance().AlbumCoverExist())
    {
        if (theApp.m_app_setting_data.draw_album_high_quality)
            m_draw.DrawImage(CPlayer::GetInstance().GetAlbumCover(), rect.TopLeft(), rect.Size(), theApp.m_app_setting_data.album_cover_fit);
        else
            m_draw.DrawBitmap(CPlayer::GetInstance().GetAlbumCover(), rect.TopLeft(), rect.Size(), theApp.m_app_setting_data.album_cover_fit);
    }
    else
    {
        CRect rc_temp = rect;
        int cover_margin = static_cast<int>(rect.Width() * 0.08);
        rc_temp.DeflateRect(cover_margin, cover_margin);
        if (theApp.m_app_setting_data.draw_album_high_quality)
        {
            Gdiplus::Image* image{ CPlayer::GetInstance().IsPlaying() ? theApp.m_image_set.default_cover : theApp.m_image_set.default_cover_not_played };
            m_draw.DrawImage(image, rc_temp.TopLeft(), rc_temp.Size(), CDrawCommon::StretchMode::FIT);
        }
        else
        {
            int cover_side = min(rc_temp.Width(), rc_temp.Height());
            int x = rc_temp.left + (rc_temp.Width() - cover_side) / 2;
            int y = rc_temp.top + (rc_temp.Height() - cover_side) / 2;
            HICON& icon{ CPlayer::GetInstance().IsPlaying() ? theApp.m_icon_set.default_cover : theApp.m_icon_set.default_cover_not_played };
            m_draw.DrawIcon(icon, CPoint(x, y), CSize(cover_side, cover_side));
        }
    }
}

void CPlayerUIBase::DrawVolumeButton(CRect rect, LPCTSTR str, bool adj_btn_top)
{
    if (str == nullptr)
    {
        wchar_t buff[64];
        swprintf_s(buff, CCommon::LoadText(IDS_VOLUME, _T(": %d%%")), CPlayer::GetInstance().GetVolume());
        str = buff;
    }
    if (m_buttons[BTN_VOLUME].pressed)
        rect.MoveToXY(rect.left + theApp.DPI(1), rect.top + theApp.DPI(1));
    if (m_buttons[BTN_VOLUME].hover)        //鼠标指向音量区域时，以另外一种颜色显示
        m_draw.DrawWindowText(rect, str, m_colors.color_text_heighlight);
    else
        m_draw.DrawWindowText(rect, str, m_colors.color_text);
    //设置音量调整按钮的位置
    m_buttons[BTN_VOLUME].rect = DrawAreaToClient(rect, m_draw_rect);
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

void CPlayerUIBase::DrawABRepeatButton(CRect rect)
{
    CString info;
    CPlayer::ABRepeatMode ab_repeat_mode = CPlayer::GetInstance().GetABRepeatMode();
    if (ab_repeat_mode == CPlayer::AM_A_SELECTED)
        info = _T("A-");
    else
        info = _T("A-B");
    CFont* pOldFont = m_draw.GetFont();
    m_draw.SetFont(&theApp.m_font_set.font8.GetFont(theApp.m_ui_data.full_screen));      //AB重复使用小一号字体，即播放时间的字体
    DrawTextButton(rect, m_buttons[BTN_AB_REPEAT], info, ab_repeat_mode != CPlayer::AM_NONE);
    m_draw.SetFont(pOldFont);
}

void CPlayerUIBase::DrawLyrics(CRect rect, int margin)
{
    if (margin < 0)
        margin = Margin();

    //填充歌词区域背景
    if (theApp.m_app_setting_data.lyric_background)
    {
        BYTE alpha = 255;
        if (IsDrawBackgroundAlpha())
            alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency) * 3 / 5;
        if (theApp.m_app_setting_data.button_round_corners)
        {
            m_draw.SetDrawArea(rect);
            m_draw.DrawRoundRect(rect, m_colors.color_lyric_back, DPI(4), alpha);
        }
        else
        {
            m_draw.FillAlphaRect(rect, m_colors.color_lyric_back, alpha);
        }
    }
    //设置歌词文字区域
    if (margin > 0)
        rect.DeflateRect(margin, margin);
    //CDrawCommon::SetDrawArea(pDC, lyric_area);

    //绘制歌词文本
    m_draw.DrawLryicCommon(rect, theApp.m_lyric_setting_data.lyric_align);
}

IconRes* CPlayerUIBase::GetRepeatModeIcon()
{
    IconRes* pIcon = nullptr;
    switch (CPlayer::GetInstance().GetRepeatMode())
    {
    case RepeatMode::RM_PLAY_ORDER:
        pIcon = &theApp.m_icon_set.play_oder;
        break;
    case RepeatMode::RM_LOOP_PLAYLIST:
        pIcon = &theApp.m_icon_set.loop_playlist;
        break;
    case RepeatMode::RM_LOOP_TRACK:
        pIcon = &theApp.m_icon_set.loop_track;
        break;
    case RepeatMode::RM_PLAY_SHUFFLE:
        pIcon = &theApp.m_icon_set.play_shuffle;
        break;
    case RepeatMode::RM_PLAY_RANDOM:
        pIcon = &theApp.m_icon_set.play_random;
        break;
    case RepeatMode::RM_PLAY_TRACK:
        pIcon = &theApp.m_icon_set.play_track;
        break;
    }
    return pIcon;
}

//void CPlayerUIBase::AddMouseToolTip(BtnKey btn, LPCTSTR str)
//{
//  m_tool_tip->AddTool(theApp.m_pMainWnd, str, m_buttons[btn].rect, btn + 1);
//}
//
//void CPlayerUIBase::UpdateMouseToolTip(BtnKey btn, LPCTSTR str)
//{
//  m_tool_tip->UpdateTipText(str, theApp.m_pMainWnd, btn + 1);
//}

void CPlayerUIBase::AddToolTips()
{
    AddMouseToolTip(BTN_REPETEMODE, m_repeat_mode_tip);
    AddMouseToolTip(BTN_TRANSLATE, CCommon::LoadText(IDS_SHOW_LYRIC_TRANSLATION));
    AddMouseToolTip(BTN_VOLUME, CCommon::LoadText(IDS_MOUSE_WHEEL_ADJUST_VOLUME));
    AddMouseToolTip(BTN_SKIN, CCommon::LoadText(IDS_SWITCH_UI, _T(" (Ctrl+U)")));
    AddMouseToolTip(BTN_EQ, CCommon::LoadText(IDS_SOUND_EFFECT_SETTING, _T(" (Ctrl+E)")));
    AddMouseToolTip(BTN_SETTING, CCommon::LoadText(IDS_SETTINGS, _T(" (Ctrl+I)")));
    AddMouseToolTip(BTN_MINI, CCommon::LoadText(IDS_MINI_MODE, _T(" (Ctrl+M)")));
    AddMouseToolTip(BTN_MINI1, CCommon::LoadText(IDS_MINI_MODE, _T(" (Ctrl+M)")));
    AddMouseToolTip(BTN_INFO, m_info_tip);
    AddMouseToolTip(BTN_STOP, CCommon::LoadText(IDS_STOP));
    AddMouseToolTip(BTN_PREVIOUS, CCommon::LoadText(IDS_PREVIOUS));
    AddMouseToolTip(BTN_PLAY_PAUSE, CPlayer::GetInstance().IsPlaying() ? CCommon::LoadText(IDS_PAUSE) : CCommon::LoadText(IDS_PLAY));
    AddMouseToolTip(BTN_NEXT, CCommon::LoadText(IDS_NEXT));
    AddMouseToolTip(BTN_PROGRESS, CCommon::LoadText(IDS_SEEK_TO));
    AddMouseToolTip(BTN_SHOW_PLAYLIST, CCommon::LoadText(IDS_SHOW_HIDE_PLAYLIST, _T(" (Ctrl+L)")));
    AddMouseToolTip(BTN_SELECT_FOLDER, CCommon::LoadText(IDS_MEDIA_LIB, _T(" (Ctrl+T)")));
    AddMouseToolTip(BTN_FIND, CCommon::LoadText(IDS_FIND_SONGS, _T(" (Ctrl+F)")));
    AddMouseToolTip(BTN_COVER, m_cover_tip);
    AddMouseToolTip(BTN_FULL_SCREEN, CCommon::LoadText(IDS_FULL_SCREEN, _T(" (F11)")));
    AddMouseToolTip(BTN_MENU, CCommon::LoadText(IDS_MAIN_MENU));
    AddMouseToolTip(BTN_FAVOURITE, CCommon::LoadText(IDS_ADD_TO_MA_FAVOURITE));
    AddMouseToolTip(BTN_LRYIC, CCommon::LoadText(IDS_SHOW_DESKTOP_LYRIC));
    AddMouseToolTip(BTN_AB_REPEAT, CCommon::LoadText(IDS_AB_REPEAT, _T(" (Ctrl+R)")));
    AddMouseToolTip(BTN_APP_CLOSE, CCommon::LoadText(IDS_CLOSE));
    AddMouseToolTip(BTN_MINIMIZE, CCommon::LoadText(IDS_MINIMIZE));
    AddMouseToolTip(BTN_MAXIMIZE, CCommon::LoadText(IDS_MAXIMIZE));

    UpdateRepeatModeToolTip();
}
