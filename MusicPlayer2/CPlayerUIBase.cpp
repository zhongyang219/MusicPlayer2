#include "stdafx.h"
#include "CPlayerUIBase.h"


CPlayerUIBase::CPlayerUIBase(UIData& ui_data, CWnd* pMainWnd)
    : m_ui_data(ui_data), m_pMainWnd(pMainWnd)
{

    //m_font_time.CreatePointFont(80, CCommon::LoadText(IDS_DEFAULT_FONT));
}


CPlayerUIBase::~CPlayerUIBase()
{
}

void CPlayerUIBase::SetToolTip(CToolTipCtrl * pToolTip)
{
    m_tool_tip = pToolTip;
}

void CPlayerUIBase::Init(CDC * pDC)
{
    m_pDC = pDC;
    m_draw.Create(m_pDC, m_pMainWnd);
}

void CPlayerUIBase::DrawInfo(bool reset)
{
    PreDrawInfo();

    //设置缓冲的DC
    CDC MemDC;
    CBitmap MemBitmap;
    MemDC.CreateCompatibleDC(NULL);

    MemBitmap.CreateCompatibleBitmap(m_pDC, m_draw_rect.Width(), m_draw_rect.Height());
    CBitmap *pOldBit = MemDC.SelectObject(&MemBitmap);
    m_draw.SetDC(&MemDC);	//将m_draw中的绘图DC设置为缓冲的DC
    m_draw.SetFont(&theApp.m_font_set.normal.GetFont(theApp.m_ui_data.full_screen));

    //绘制背景
    DrawBackground();

    //绘制界面中其他信息
    _DrawInfo(reset);

    //将缓冲区DC中的图像拷贝到屏幕中显示
    m_pDC->BitBlt(m_draw_rect.left, m_draw_rect.top, m_draw_rect.Width(), m_draw_rect.Height(), &MemDC, 0, 0, SRCCOPY);
    MemDC.SelectObject(pOldBit);
    MemBitmap.DeleteObject();
    MemDC.DeleteDC();


    if (m_first_draw)
    {
        AddToolTips();
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
        if(btn.second.rect.PtInRect(point) != FALSE)
            btn.second.pressed = true;
    }
}

void CPlayerUIBase::RButtonUp(CPoint point)
{
    if (!m_draw_rect.PtInRect(point))
        return;

    if (m_buttons[BTN_VOLUME].rect.PtInRect(point) == FALSE)
        m_show_volume_adj = false;

    CPoint point1;		//定义一个用于确定光标位置的位置
    GetCursorPos(&point1);	//获取当前光标的位置，以便使得菜单可以跟随光标，该位置以屏幕左上角点为原点，point则以客户区左上角为原点
    if (m_buttons[BTN_REPETEMODE].rect.PtInRect(point))		//如果在“循环模式”的矩形区域内点击鼠标右键，则弹出“循环模式”的子菜单
    {
        CMenu* pMenu = theApp.m_menu_set.m_main_popup_menu.GetSubMenu(0)->GetSubMenu(1);
        if (pMenu != NULL)
            pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, theApp.m_pMainWnd);
        return;
    }

    if (m_buttons[BTN_SHOW_PLAYLIST].rect.PtInRect(point))
    {
        CMenu* pMenu = theApp.m_menu_set.m_playlist_btn_menu.GetSubMenu(0);
        if (pMenu != NULL)
            pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, theApp.m_pMainWnd);
        return;
    }

    for (auto& btn : m_buttons)
    {
        //按钮上点击右键不弹出菜单
        if (btn.first != BTN_COVER && btn.second.rect.PtInRect(point) != FALSE)
            return;
    }

    if (!m_draw_data.lyric_rect.PtInRect(point))	//如果在歌词区域点击了鼠标右键
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
        btn.second.hover = (btn.second.rect.PtInRect(point) != FALSE);
    }

    m_buttons[BTN_PROGRESS].hover = m_buttons[BTN_PROGRESS].hover && !(m_show_volume_adj && (m_buttons[BTN_VOLUME_UP].rect.PtInRect(point) || m_buttons[BTN_VOLUME_DOWN].rect.PtInRect(point)));

    //鼠标指向进度条时显示定位到几分几秒
    if (m_buttons[BTN_PROGRESS].hover)
    {
        __int64 song_pos;
        song_pos = static_cast<__int64>(point.x - m_buttons[BTN_PROGRESS].rect.left) * CPlayer::GetInstance().GetSongLength() / m_buttons[BTN_PROGRESS].rect.Width();
        Time song_pos_time;
        song_pos_time.int2time(static_cast<int>(song_pos));
        CString str;
        static int last_sec{};
        if (last_sec != song_pos_time.sec)		//只有鼠标指向位置对应的秒数变化了才更新鼠标提示
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
    if (!m_show_volume_adj)		//如果设有显示音量调整按钮，则点击音量区域就显示音量调整按钮
        m_show_volume_adj = (m_buttons[BTN_VOLUME].rect.PtInRect(point) != FALSE);
    else		//如果已经显示了音量调整按钮，则点击音量调整时保持音量调整按钮的显示
        m_show_volume_adj = (m_buttons[BTN_VOLUME_UP].rect.PtInRect(point) || m_buttons[BTN_VOLUME_DOWN].rect.PtInRect(point));

    for (auto& btn : m_buttons)
    {
        //if (btn.second.rect.PtInRect(point) != FALSE)
        //{
        //btn.second.hover = false;
        btn.second.pressed = false;
        //}

        if (btn.second.rect.PtInRect(point) && btn.second.enable)
        {
            switch (btn.first)
            {
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

            case BTN_INFO:
                m_buttons[BTN_INFO].hover = false;
                theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_SONG_INFO);
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

void CPlayerUIBase::OnSizeRedraw(int cx, int cy)
{
    CRect redraw_rect;
    if (!m_ui_data.narrow_mode)	//在普通界面模式下
    {
        if (cx < m_ui_data.client_width)	//如果界面宽度变窄了
        {
            //重新将绘图区域右侧区域的矩形区域填充为对话框背景色
            redraw_rect = m_draw_rect;
            if (m_ui_data.show_playlist)
            {
                redraw_rect.left = cx / 2/* - Margin()*/;
                redraw_rect.right = m_ui_data.client_width / 2 + Margin();
                m_pDC->FillSolidRect(redraw_rect, CONSTVAL::BACKGROUND_COLOR);
            }
            //else
            //{
            //	redraw_rect.left = cx - Margin();
            //	redraw_rect.right = cx;
            //}
        }
        //if (cy < m_ui_data.client_height)	//如果界面高度变小了
        //{
        //	//重新将绘图区域下方区域的矩形区域填充为对话框背景色
        //	redraw_rect = m_draw_rect;
        //	redraw_rect.top = cy - Margin();
        //	redraw_rect.bottom = cy;
        //	m_pDC->FillSolidRect(redraw_rect, CONSTVAL::BACKGROUND_COLOR);
        //}
    }
    else if (m_ui_data.narrow_mode)	//在窄界面模式下
    {
        //if (cx < m_ui_data.client_width)		//如果宽度变窄了
        //{
        //	//重新将绘图区域右侧区域的矩形区域填充为对话框背景色
        //	redraw_rect = m_draw_rect;
        //	redraw_rect.left = cx - Margin();
        //	redraw_rect.right = cx;
        //	m_pDC->FillSolidRect(redraw_rect, CONSTVAL::BACKGROUND_COLOR);
        //}
        //if (cy < m_ui_data.client_height)	//如果界面高度变小了
        //{
        //	if (!m_ui_data.show_playlist)
        //	{
        //		//重新将绘图区域下方区域的矩形区域填充为对话框背景色
        //		redraw_rect = m_draw_rect;
        //		redraw_rect.top = cy - Margin();
        //		redraw_rect.bottom = cy;
        //		m_pDC->FillSolidRect(redraw_rect, CONSTVAL::BACKGROUND_COLOR);
        //	}
        //}
    }
}

CRect CPlayerUIBase::GetThumbnailClipArea()
{
    //获取菜单栏的高度
    int menu_bar_height = 0;
    if (m_ui_data.show_menu_bar)
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
    m_colors = CPlayerUIHelper::GetUIColors(theApp.m_app_setting_data.theme_color, theApp.m_app_setting_data.dark_mode);

    //if (m_repeat_mode_tip.IsEmpty())
    SetRepeatModeToolTipText();
    SetSongInfoToolTipText();

    //设置绘制的矩形区域
    SetDrawRect();
}

void CPlayerUIBase::SetDrawRect()
{
    if (!m_ui_data.show_playlist)
    {
        m_draw_rect = CRect(0, 0, m_ui_data.client_width, m_ui_data.client_height);
        //m_draw_rect.DeflateRect(Margin(), Margin());
    }
    else
    {
        if (!m_ui_data.narrow_mode)
        {
            m_draw_rect = CRect{ /*CPoint{Margin(), Margin()}*/ CPoint(),
                                                                CPoint{m_ui_data.client_width / 2 /* - Margin()*/, m_ui_data.client_height} };
        }
        else
        {
            m_draw_rect = CRect{ /*CPoint{ Margin(), Margin() }*/ CPoint(),
                                                                  CSize{ m_ui_data.client_width /*- 2 * Margin()*/, DrawAreaHeight() - Margin() } };
        }

    }
}

void CPlayerUIBase::DrawBackground()
{
    CRect draw_rect = m_draw_rect;
    draw_rect.MoveToXY(0, 0);

    //绘制背景
    if (theApp.m_app_setting_data.album_cover_as_background)
    {
        if (CPlayer::GetInstance().AlbumCoverExist())
        {
            CImage& back_image{ theApp.m_app_setting_data.background_gauss_blur ? CPlayer::GetInstance().GetAlbumCoverBlur() : CPlayer::GetInstance().GetAlbumCover() };
            m_draw.DrawBitmap(back_image, CPoint(0, 0), m_draw_rect.Size(), CDrawCommon::StretchMode::FILL);
        }
        else
        {
            //MemDC.FillSolidRect(0, 0, m_draw_rect.Width(), m_draw_rect.Height(), GetSysColor(COLOR_BTNFACE));	//给缓冲DC的绘图区域填充对话框的背景颜色
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
    wchar_t buff[64];
    if (CPlayer::GetInstance().m_loading)
    {
        static CDrawCommon::ScrollInfo scroll_info0;
        CString info;
        info = CCommon::LoadTextFormat(IDS_PLAYLIST_INIT_INFO, { CPlayer::GetInstance().GetSongNum(), CPlayer::GetInstance().m_thread_info.process_percent });
        m_draw.DrawScrollText(rect, info, m_colors.color_text, DPI(1.5), false, scroll_info0, reset);
    }
    else
    {
        //绘制播放状态
        CString play_state_str = CPlayer::GetInstance().GetPlayingState().c_str();
        CRect rc_tmp{ rect };
        //m_draw.GetDC()->SelectObject(theApp.m_pMainWnd->GetFont());
        rc_tmp.right = rc_tmp.left + m_draw.GetTextExtent(play_state_str).cx + DPI(4);
        m_draw.DrawWindowText(rc_tmp, play_state_str, m_colors.color_text_lable);

        //绘制歌曲序号
        rc_tmp.MoveToX(rc_tmp.right);
        rc_tmp.right = rc_tmp.left + DPI(30);
        swprintf_s(buff, sizeof(buff) / 2, L"%.3d", CPlayer::GetInstance().GetIndex() + 1);
        m_draw.DrawWindowText(rc_tmp, buff, m_colors.color_text_2);

        //绘制文件名
        rc_tmp.MoveToX(rc_tmp.right);
        rc_tmp.right = rect.right;
        static CDrawCommon::ScrollInfo scroll_info1;
        m_draw.DrawScrollText(rc_tmp, CPlayer::GetInstance().GetFileName().c_str(), m_colors.color_text, DPI(1.5), false, scroll_info1, reset);
    }
}

void CPlayerUIBase::DrawControlBarBtn(CRect rect, UIButton & btn, const IconRes & icon)
{
    rect.DeflateRect(DPI(2), DPI(2));
    DrawUIButton(rect, btn, icon);
}

void CPlayerUIBase::DrawToolBar(CRect rect, bool draw_translate_button)
{
    bool draw_background{ IsDrawBackgroundAlpha() };
    //绘制背景
    BYTE alpha;
    if (theApp.m_app_setting_data.dark_mode)
        alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency) * 2 / 3;
    else
        alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency);

    if (draw_background)
        m_draw.FillAlphaRect(rect, m_colors.color_control_bar_back, alpha);
    else
        m_draw.FillRect(rect, m_colors.color_control_bar_back);

    CRect rc_tmp = rect;

    //绘制循环模式
    rc_tmp.right = rect.left + rect.Height();
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
    }
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


    //绘制翻译按钮
    if (draw_translate_button && rect.Width() >= DPI(262))
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

    rc_tmp.left = rc_tmp.right = rect.right;

    //显示<<<<
    if (rect.Width() >= DPI(313))
    {
        int progress;
        Time time{ CPlayer::GetInstance().GetCurrentPosition() };
        if (CPlayer::GetInstance().IsMidi())
        {
            ////progress = (CPlayer::GetInstance().GetMidiInfo().midi_position % 16 + 1) *1000 / 16;
            //if (CPlayer::GetInstance().GetMidiInfo().tempo == 0)
            //	progress = 0;
            //else
            //	progress = (time.time2int() * 1000 / CPlayer::GetInstance().GetMidiInfo().tempo % 4 + 1) * 250;
            progress = (CPlayer::GetInstance().GetMidiInfo().midi_position % 4 + 1) * 250;
        }
        else
        {
            progress = (time.sec % 4 * 1000 + time.msec) / 4;
        }
        rc_tmp.right = rc_tmp.left;
        rc_tmp.left = rc_tmp.right - DPI(44);
        m_draw.DrawWindowText(rc_tmp, _T("<<<<"), m_colors.color_text, m_colors.color_text_2, progress, false);
    }


    //显示音量
    wchar_t buff[64];
    rc_tmp.right = rc_tmp.left;
    rc_tmp.left = rc_tmp.right - DPI(72);
    swprintf_s(buff, CCommon::LoadText(IDS_VOLUME, _T(": %d%%")), CPlayer::GetInstance().GetVolume());
    CRect rc_vol{ rc_tmp };
    if (m_buttons[BTN_VOLUME].pressed)
        rc_vol.MoveToXY(rc_vol.left + theApp.DPI(1), rc_vol.top + theApp.DPI(1));
    if (m_buttons[BTN_VOLUME].hover)		//鼠标指向音量区域时，以另外一种颜色显示
        m_draw.DrawWindowText(rc_vol, buff, m_colors.color_text_heighlight);
    else
        m_draw.DrawWindowText(rc_vol, buff, m_colors.color_text);
    //设置音量调整按钮的位置
    m_buttons[BTN_VOLUME].rect = DrawAreaToClient(rc_tmp, m_draw_rect);
    m_buttons[BTN_VOLUME].rect.DeflateRect(0, DPI(4));
    m_buttons[BTN_VOLUME].rect.right -= DPI(12);
    m_buttons[BTN_VOLUME_DOWN].rect = m_buttons[BTN_VOLUME].rect;
    m_buttons[BTN_VOLUME_DOWN].rect.bottom += DPI(4);
    m_buttons[BTN_VOLUME_DOWN].rect.MoveToY(m_buttons[BTN_VOLUME].rect.bottom);
    m_buttons[BTN_VOLUME_DOWN].rect.right = m_buttons[BTN_VOLUME].rect.left + m_buttons[BTN_VOLUME].rect.Width() / 2;
    m_buttons[BTN_VOLUME_UP].rect = m_buttons[BTN_VOLUME_DOWN].rect;
    m_buttons[BTN_VOLUME_UP].rect.MoveToX(m_buttons[BTN_VOLUME_DOWN].rect.right);
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

void CPlayerUIBase::DrawUIButton(CRect rect, UIButton & btn, const IconRes & icon)
{
    if(btn.pressed)
        rect.MoveToXY(rect.left + theApp.DPI(1), rect.top + theApp.DPI(1));

    CRect rc_tmp = rect;
    //rc_tmp.DeflateRect(DPI(2), DPI(2));
    m_draw.SetDrawArea(rc_tmp);

    BYTE alpha;
    if (IsDrawBackgroundAlpha())
        alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency) * 2 / 3;
    else
        alpha = 255;
    if(btn.pressed && btn.hover)
        m_draw.FillAlphaRect(rc_tmp, m_colors.color_button_pressed, alpha);
    else if (btn.hover)
        m_draw.FillAlphaRect(rc_tmp, m_colors.color_text_2, alpha);

    //else if (!theApp.m_app_setting_data.dark_mode)
    //	m_draw.FillAlphaRect(rc_tmp, m_colors.color_button_back, alpha);

    btn.rect = DrawAreaToClient(rc_tmp, m_draw_rect);

    rc_tmp = rect;
    //使图标在矩形中居中
    CSize icon_size = icon.GetSize(theApp.m_ui_data.full_screen);
    rc_tmp.left = rect.left + (rect.Width() - icon_size.cx) / 2;
    rc_tmp.top = rect.top + (rect.Height() - icon_size.cy) / 2;
    rc_tmp.right = rc_tmp.left + icon_size.cx;
    rc_tmp.bottom = rc_tmp.top + icon_size.cy;

    const HICON& hIcon = icon.GetIcon(!theApp.m_app_setting_data.dark_mode, theApp.m_ui_data.full_screen);
    m_draw.DrawIcon(hIcon, rc_tmp.TopLeft(), rc_tmp.Size());

}

void CPlayerUIBase::DrawControlButton(CRect rect, UIButton & btn, const IconRes & icon)
{
    if (btn.pressed)
        rect.MoveToXY(rect.left + theApp.DPI(1), rect.top + theApp.DPI(1));

    CRect rc_tmp = rect;
    m_draw.SetDrawArea(rc_tmp);

    BYTE alpha;
    if (IsDrawBackgroundAlpha())
        alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency) * 2 / 3;
    else
        alpha = 255;
    if (btn.pressed && btn.hover)
        m_draw.FillAlphaRect(rc_tmp, m_colors.color_button_pressed, alpha);
    else if (btn.hover)
        m_draw.FillAlphaRect(rc_tmp, m_colors.color_text_2, alpha);

    //else if (!theApp.m_app_setting_data.dark_mode)
    //	m_draw.FillAlphaRect(rc_tmp, m_colors.color_button_back, alpha);

    btn.rect = DrawAreaToClient(rc_tmp, m_draw_rect);

    rc_tmp = rect;
    //使图标在矩形中居中
    CSize icon_size = icon.GetSize(theApp.m_ui_data.full_screen);
    rc_tmp.left = rect.left + (rect.Width() - icon_size.cx) / 2;
    rc_tmp.top = rect.top + (rect.Height() - icon_size.cy) / 2;
    rc_tmp.right = rc_tmp.left + icon_size.cx;
    rc_tmp.bottom = rc_tmp.top + icon_size.cy;

    const HICON& hIcon = icon.GetIcon(!theApp.m_app_setting_data.dark_mode, theApp.m_ui_data.full_screen);
    m_draw.DrawIcon(hIcon, rc_tmp.TopLeft(), rc_tmp.Size());
}

void CPlayerUIBase::DrawTextButton(CRect rect, UIButton & btn, LPCTSTR text, bool back_color)
{
    if (btn.enable)
    {
        BYTE alpha;
        if (IsDrawBackgroundAlpha())
            alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency) * 2 / 3;
        else
            alpha = 255;
        if (btn.pressed)
        {
            rect.MoveToXY(rect.left + theApp.DPI(1), rect.top + theApp.DPI(1));
            m_draw.FillAlphaRect(rect, m_colors.color_button_pressed, alpha);
        }
        else if (btn.hover)
        {
            m_draw.FillAlphaRect(rect, m_colors.color_text_2, alpha);
        }
        else if (back_color)
        {
            m_draw.FillAlphaRect(rect, m_colors.color_button_back, alpha);
        }
        m_draw.DrawWindowText(rect, text, m_colors.color_text, Alignment::CENTER);
    }
    else
    {
        m_draw.DrawWindowText(rect, text, GRAY(200), Alignment::CENTER);
    }
    btn.rect = DrawAreaToClient(rect, m_draw_rect);
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
    if(m_ui_data.full_screen)
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

    return info_height;
}

bool CPlayerUIBase::IsDrawBackgroundAlpha() const
{
    return theApp.m_app_setting_data.album_cover_as_background && (CPlayer::GetInstance().AlbumCoverExist() || !m_ui_data.default_background.IsNull());
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
        //else if (m_buttons[BTN_VOLUME_UP].hover)
        //	btn_up_back_color = m_colors.color_control_bar_back;
        else
            btn_up_back_color = m_colors.color_text_2;

        if (m_buttons[BTN_VOLUME_DOWN].pressed && m_buttons[BTN_VOLUME_DOWN].hover)
            btn_down_back_color = m_colors.color_button_pressed;
        //else if (m_buttons[BTN_VOLUME_DOWN].hover)
        //	btn_down_back_color = m_colors.color_control_bar_back;
        else
            btn_down_back_color = m_colors.color_text_2;


        m_draw.FillAlphaRect(volume_up_rect, btn_up_back_color, alpha);
        m_draw.FillAlphaRect(volume_down_rect, btn_down_back_color, alpha);

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
    if(CPlayer::GetInstance().IsFavourite())
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
    CRect rc_time = rect;
    wstring strTime = CPlayer::GetInstance().GetTimeString();

    m_draw.SetFont(&theApp.m_font_set.time.GetFont(m_ui_data.full_screen));
    CSize strSize = m_draw.GetTextExtent(strTime.c_str());
    rc_time.left = rc_time.right - strSize.cx;
    //rc_time.InflateRect(0, DPI(2));
    rc_time.top -= DPI(1);
    m_draw.DrawWindowText(rc_time, strTime.c_str(), m_colors.color_text);

    //绘制进度条
    const int progress_height = DPI(4);
    CRect progress_rect = rect;
    progress_rect.right = rc_time.left - Margin();
    progress_rect.top = rect.top + (rect.Height() - progress_height) / 2;
    progress_rect.bottom = progress_rect.top + progress_height;

    if (IsDrawBackgroundAlpha())
        m_draw.FillAlphaRect(progress_rect, m_colors.color_spectrum_back, ALPHA_CHG(theApp.m_app_setting_data.background_transparency) * 2 / 3);
    else
        m_draw.FillRect(progress_rect, m_colors.color_spectrum_back);

    m_buttons[BTN_PROGRESS].rect = DrawAreaToClient(progress_rect, m_draw_rect);
    m_buttons[BTN_PROGRESS].rect.InflateRect(0, DPI(3));

    double progress = static_cast<double>(CPlayer::GetInstance().GetCurrentPosition()) / CPlayer::GetInstance().GetSongLength();
    progress_rect.right = progress_rect.left + static_cast<int>(progress * progress_rect.Width());
    if (progress_rect.right > progress_rect.left)
        m_draw.FillRect(progress_rect, m_colors.color_spectrum);
}

void CPlayerUIBase::DrawTranslateButton(CRect rect)
{
    m_buttons[BTN_TRANSLATE].enable = CPlayer::GetInstance().m_Lyrics.IsTranslated();
    DrawTextButton(rect, m_buttons[BTN_TRANSLATE], CCommon::LoadText(IDS_TRAS), m_ui_data.show_translate);
}

int CPlayerUIBase::DrawTopRightIcons()
{
    int total_width = 0;
    const int icon_size = DPI(28);
    //绘制“全屏”图标
    if (!m_ui_data.show_playlist || m_ui_data.full_screen)
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
    m_draw.SetFont(&theApp.m_font_set.time.GetFont(m_ui_data.full_screen));
    m_draw.DrawWindowText(rc_tmp, buff, m_colors.color_text);
    m_draw.SetFont(&theApp.m_font_set.normal.GetFont(theApp.m_ui_data.full_screen));
}

//void CPlayerUIBase::AddMouseToolTip(BtnKey btn, LPCTSTR str)
//{
//	m_tool_tip->AddTool(theApp.m_pMainWnd, str, m_buttons[btn].rect, btn + 1);
//}
//
//void CPlayerUIBase::UpdateMouseToolTip(BtnKey btn, LPCTSTR str)
//{
//	m_tool_tip->UpdateTipText(str, theApp.m_pMainWnd, btn + 1);
//}

void CPlayerUIBase::AddToolTips()
{
    AddMouseToolTip(BTN_REPETEMODE, m_repeat_mode_tip);
    AddMouseToolTip(BTN_TRANSLATE, CCommon::LoadText(IDS_SHOW_LYRIC_TRANSLATION));
    AddMouseToolTip(BTN_VOLUME, CCommon::LoadText(IDS_MOUSE_WHEEL_ADJUST_VOLUME));
    AddMouseToolTip(BTN_SKIN, CCommon::LoadText(IDS_SWITCH_UI));
    AddMouseToolTip(BTN_EQ, CCommon::LoadText(IDS_SOUND_EFFECT_SETTING, _T(" (Ctrl+E)")));
    AddMouseToolTip(BTN_SETTING, CCommon::LoadText(IDS_SETTINGS, _T(" (Ctrl+I)")));
    AddMouseToolTip(BTN_MINI, CCommon::LoadText(IDS_MINI_MODE, _T(" (Ctrl+M)")));
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
}

