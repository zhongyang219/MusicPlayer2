#include "stdafx.h"
#include "CMiniModeUI.h"
#include "PlayListCtrl.h"


CMiniModeUI::CMiniModeUI(SMiniModeUIData& ui_data, CWnd* pMaineWnd)
    : m_ui_data(ui_data), CPlayerUIBase(theApp.m_ui_data, pMaineWnd)
{
    m_font_time.CreatePointFont(80, CCommon::LoadText(IDS_DEFAULT_FONT));
}


CMiniModeUI::~CMiniModeUI()
{
}

void CMiniModeUI::Init(CDC* pDC)
{
    CPlayerUIBase::Init(pDC);
    m_first_draw = true;
}

void CMiniModeUI::_DrawInfo(CRect draw_rect, bool reset)
{
    //绘制专辑封面
    int cover_side = m_ui_data.window_height - 2 * m_ui_data.margin;
    CRect cover_rect{ CPoint(m_ui_data.margin, m_ui_data.margin), CSize(cover_side, cover_side) };
    if (theApp.m_app_setting_data.show_album_cover && CPlayer::GetInstance().AlbumCoverExist())
    {
        if (theApp.m_app_setting_data.draw_album_high_quality)
            m_draw.DrawImage(CPlayer::GetInstance().GetAlbumCover(), cover_rect.TopLeft(), cover_rect.Size(), theApp.m_app_setting_data.album_cover_fit);
        else
            m_draw.DrawBitmap(CPlayer::GetInstance().GetAlbumCover(), cover_rect.TopLeft(), cover_rect.Size(), theApp.m_app_setting_data.album_cover_fit);
    }
    else        //专辑封面不存在时显示默认专辑封面图标
    {
        if (IsDrawBackgroundAlpha())
            m_draw.FillAlphaRect(cover_rect, m_colors.color_spectrum_back, ALPHA_CHG(theApp.m_app_setting_data.background_transparency) * 2 / 3);
        else
            m_draw.FillRect(cover_rect, m_colors.color_spectrum_back);

        if (theApp.m_app_setting_data.draw_album_high_quality)
        {
            Gdiplus::Image* image{ CPlayer::GetInstance().IsPlaying() ? theApp.m_image_set.default_cover : theApp.m_image_set.default_cover_not_played };
            m_draw.DrawImage(image, cover_rect.TopLeft(), cover_rect.Size(), CDrawCommon::StretchMode::FIT);
        }
        else
        {
            //使图标在矩形框中居中
            CSize icon_size{ theApp.DPI(32), theApp.DPI(32) };
            CRect icon_rect;
            icon_rect.left = cover_rect.left + (cover_rect.Width() - icon_size.cx) / 2;
            icon_rect.right = icon_rect.left + icon_size.cx;
            icon_rect.top = cover_rect.top + (cover_rect.Height() - icon_size.cy) / 2;
            icon_rect.bottom = icon_rect.top + icon_size.cy;

            HICON& icon{ CPlayer::GetInstance().IsPlaying() ? theApp.m_icon_set.default_cover_small : theApp.m_icon_set.default_cover_small_not_played };
            m_draw.DrawIcon(icon, icon_rect.TopLeft(), icon_rect.Size());
        }
    }
    m_buttons[BTN_COVER].rect = cover_rect;

    //绘制播放控制按钮
    CRect rc_tmp;
    rc_tmp.MoveToXY(m_ui_data.window_height, m_ui_data.margin);
    rc_tmp.right = rc_tmp.left + theApp.DPI(27);
    rc_tmp.bottom = rc_tmp.top + theApp.DPI(22);
    DrawUIButton(rc_tmp, m_buttons[BTN_PREVIOUS], theApp.m_icon_set.previous_new);

    rc_tmp.MoveToX(rc_tmp.right + m_ui_data.margin);
    if (CPlayer::GetInstance().IsPlaying())
        DrawUIButton(rc_tmp, m_buttons[BTN_PLAY_PAUSE], theApp.m_icon_set.pause_new);
    else
        DrawUIButton(rc_tmp, m_buttons[BTN_PLAY_PAUSE], theApp.m_icon_set.play_new);

    rc_tmp.MoveToX(rc_tmp.right + m_ui_data.margin);
    DrawUIButton(rc_tmp, m_buttons[BTN_NEXT], theApp.m_icon_set.next_new);


    //绘制频谱分析
    rc_tmp.MoveToX(rc_tmp.right + m_ui_data.margin);
    rc_tmp.right = rc_tmp.left + theApp.DPI(30);

    //if (IsDrawBackgroundAlpha())
    //  m_draw.FillAlphaRect(rc_tmp, m_colors.color_spectrum_back, ALPHA_CHG(theApp.m_app_setting_data.background_transparency) * 2 / 3);
    //else
    //  m_draw.FillRect(rc_tmp, m_colors.color_spectrum_back);
    m_draw.SetDrawArea(rc_tmp);

    int width{ rc_tmp.Width() / 9 };    //每个柱形的宽度
    int gap{ rc_tmp.Width() / 22 }; //柱形的间隔
    m_draw.DrawSpectrum(rc_tmp, width - gap, gap, 16, m_colors.color_spectrum, false);

    //绘制播放时间
    rc_tmp.MoveToX(rc_tmp.right + m_ui_data.margin);
    rc_tmp.right = m_ui_data.widnow_width - 3 * theApp.DPI(20) - 4 * m_ui_data.margin;
    rc_tmp.bottom = rc_tmp.top + theApp.DPI(16);
    CString str;
    if (m_ui_data.m_show_volume)
        str.Format(CCommon::LoadText(IDS_VOLUME, _T(": %d%%")), CPlayer::GetInstance().GetVolume());
    else if (CPlayer::GetInstance().IsError())
        str = CCommon::LoadText(IDS_PLAY_ERROR);
    else
        str = CPlayer::GetInstance().GetTimeString().c_str();
    m_draw.SetFont(&m_font_time);
    m_draw.DrawWindowText(rc_tmp, str, m_colors.color_text, Alignment::CENTER);
    m_draw.SetFont(theApp.m_pMainWnd->GetFont());

    //绘制进度条
    rc_tmp.MoveToY(rc_tmp.bottom);
    rc_tmp.bottom = rc_tmp.top + theApp.DPI(6);
    CRect progress_rect = rc_tmp;
    int progress_height = theApp.DPI(2);
    progress_rect.top = progress_rect.top + (rc_tmp.Height() - progress_height) / 2;
    progress_rect.bottom = progress_rect.top + progress_height;

    if (IsDrawBackgroundAlpha())
        m_draw.FillAlphaRect(progress_rect, m_colors.color_spectrum_back, ALPHA_CHG(theApp.m_app_setting_data.background_transparency) * 2 / 3);
    else
        m_draw.FillRect(progress_rect, m_colors.color_spectrum_back);

    m_buttons[BTN_PROGRESS].rect = progress_rect;
    m_buttons[BTN_PROGRESS].rect.InflateRect(0, theApp.DPI(2));

    double progress = static_cast<double>(CPlayer::GetInstance().GetCurrentPosition()) / CPlayer::GetInstance().GetSongLength();
    if (progress > 1)
        progress = 1;
    double progress_width_double{ progress * progress_rect.Width() };
    int progress_width{ static_cast<int>(progress_width_double) };
    progress_rect.right = progress_rect.left + progress_width;
    if (progress_rect.right > progress_rect.left)
        m_draw.FillRect(progress_rect, m_colors.color_spectrum);
    //绘制进度条最右侧一像素
    //进度条最右侧一像素根据当前进度计算出透明度，以使得进度条的变化更加平滑
    BYTE alpha{ static_cast<BYTE>((progress_width_double - progress_width) * 256) };
    progress_rect.left = progress_rect.right;
    progress_rect.right = progress_rect.left + 1;
    m_draw.FillAlphaRect(progress_rect, m_colors.color_spectrum, alpha);

    //绘制右上角按钮
    rc_tmp.right = m_ui_data.widnow_width - m_ui_data.margin;
    rc_tmp.left = rc_tmp.right - theApp.DPI(20);
    rc_tmp.top = m_ui_data.margin;
    rc_tmp.bottom = rc_tmp.top + theApp.DPI(20);
    DrawTextButton(rc_tmp, m_buttons[BTN_CLOSE], _T("×"));

    rc_tmp.MoveToX(rc_tmp.left - rc_tmp.Width() - m_ui_data.margin);
    DrawTextButton(rc_tmp, m_buttons[BTN_RETURN], _T("□"));

    rc_tmp.MoveToX(rc_tmp.left - rc_tmp.Width() - m_ui_data.margin);
    DrawTextButton(rc_tmp, m_buttons[BTN_SHOW_PLAYLIST], _T("≡"));

    //绘制右下角按钮
    rc_tmp.MoveToXY(m_ui_data.widnow_width - theApp.DPI(20) - m_ui_data.margin, m_ui_data.margin + theApp.DPI(20));
    DrawUIButton(rc_tmp, m_buttons[BTN_SELECT_FOLDER], theApp.m_icon_set.media_lib);
    rc_tmp.MoveToX(rc_tmp.left - rc_tmp.Width() - m_ui_data.margin);
    if (CPlayer::GetInstance().IsFavourite())
        DrawUIButton(rc_tmp, m_buttons[BTN_FAVOURITE], theApp.m_icon_set.heart);
    else
        DrawUIButton(rc_tmp, m_buttons[BTN_FAVOURITE], theApp.m_icon_set.favourite);

    //绘制显示文本信息
    //rc_tmp.MoveToXY(m_ui_data.window_height, m_ui_data.margin + theApp.DPI(22));
    rc_tmp.top = m_ui_data.margin + theApp.DPI(22);
    rc_tmp.right = rc_tmp.left - m_ui_data.margin;
    rc_tmp.left = m_ui_data.window_height;
    rc_tmp.bottom = m_ui_data.window_height;
    if (CPlayer::GetInstance().IsMidi() && theApp.m_general_setting_data.midi_use_inner_lyric && !CPlayer::GetInstance().MidiNoLyric())
    {
        wstring current_lyric{ CPlayer::GetInstance().GetMidiLyric() };
        m_draw.DrawWindowText(rc_tmp, current_lyric.c_str(), m_colors.color_text, Alignment::CENTER);
    }
    else
    {
        Time time{ CPlayer::GetInstance().GetCurrentPosition() };
        CLyrics::Lyric current_lyric{ CPlayer::GetInstance().m_Lyrics.GetLyric(time, 0) };  //获取当歌词
        int progress{ CPlayer::GetInstance().m_Lyrics.GetLyricProgress(time) };     //获取当前歌词进度（范围为0~1000）
        bool no_lyric{ false };
        //如果当前一句歌词为空，且持续了超过了20秒，则不显示歌词
        no_lyric = (current_lyric.text.empty() && CPlayer::GetInstance().GetCurrentPosition() - current_lyric.time.toInt() > 20000) || progress >= 1000;

        if (CPlayer::GetInstance().m_Lyrics.IsEmpty() || no_lyric)  //没有歌词时显示播放的文件名
        {
            //正在播放的文件名以滚动的样式显示。如果参数要求强制刷新，则重置滚动位置
            static CDrawCommon::ScrollInfo scroll_info;
            m_draw.DrawScrollText(rc_tmp, CPlayListCtrl::GetDisplayStr(CPlayer::GetInstance().GetCurrentSongInfo(), theApp.m_media_lib_setting_data.display_format).c_str(),
                m_colors.color_text, GetScrollTextPixel(true), true, scroll_info, reset);
        }
        else        //显示歌词
        {
            COLORREF color2 = (theApp.m_lyric_setting_data.lyric_karaoke_disp ? m_colors.color_text_2 : m_colors.color_text);
            if (current_lyric.text.empty())     //如果当前歌词为空白，就显示为省略号
                current_lyric.text = CCommon::LoadText(IDS_DEFAULT_LYRIC_TEXT);
            m_draw.DrawWindowText(rc_tmp, current_lyric.text.c_str(), m_colors.color_text, color2, progress, Alignment::CENTER);
        }
    }
}

void CMiniModeUI::RButtonUp(CPoint point)
{
}

void CMiniModeUI::LButtonUp(CPoint point)
{
    for (auto& btn : m_buttons)
    {
        btn.second.hover = false;
        btn.second.pressed = false;

        if (btn.second.rect.PtInRect(point))
        {
            switch (btn.first)
            {
            case BTN_PREVIOUS:
                theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_PREVIOUS);
                break;
            case BTN_PLAY_PAUSE:
                theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_PLAY_PAUSE);
                break;
            case BTN_NEXT:
                theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_NEXT);
                break;
            case BTN_SHOW_PLAYLIST:
                m_pMainWnd->SendMessage(WM_COMMAND, ID_SHOW_PLAY_LIST);
                break;
            case BTN_RETURN:
                //m_buttons[BTN_RETURN].hover = false;
                m_pMainWnd->SendMessage(WM_COMMAND, IDOK);
                break;
            case BTN_CLOSE:
                if (theApp.m_general_setting_data.minimize_to_notify_icon)
                    m_pMainWnd->ShowWindow(HIDE_WINDOW);
                else
                    m_pMainWnd->SendMessage(WM_COMMAND, ID_MINI_MODE_EXIT);
                break;
            case BTN_SELECT_FOLDER:
                m_pMainWnd->SendMessage(WM_COMMAND, ID_SET_PATH);
                break;
            case BTN_FAVOURITE:
                m_pMainWnd->SendMessage(WM_COMMAND, ID_ADD_REMOVE_FROM_FAVOURITE);
                break;
            default:
                break;
            case BTN_COVER:
                break;
            case BTN_PROGRESS:
            {
                int ckick_pos = point.x - btn.second.rect.left;
                double progress = static_cast<double>(ckick_pos) / btn.second.rect.Width();
                CPlayer::GetInstance().SeekTo(progress);
            }
            break;
            }

        }
    }
}

CRect CMiniModeUI::GetThumbnailClipArea()
{
    return CRect();
}

void CMiniModeUI::UpdateSongInfoTip(LPCTSTR str_tip)
{
    UpdateMouseToolTip(BTN_COVER, str_tip);
}

void CMiniModeUI::UpdatePlayPauseButtonTip()
{
    if (CPlayer::GetInstance().IsPlaying() && !CPlayer::GetInstance().IsError())
        UpdateMouseToolTip(BTN_PLAY_PAUSE, CCommon::LoadText(IDS_PAUSE));
    else
        UpdateMouseToolTip(BTN_PLAY_PAUSE, CCommon::LoadText(IDS_PLAY));
}

void CMiniModeUI::PreDrawInfo()
{
    //设置颜色
    m_colors = CPlayerUIHelper::GetUIColors(theApp.m_app_setting_data.theme_color, theApp.m_app_setting_data.dark_mode);
    //设置绘图区域
    m_draw_rect = CRect(CPoint(0, 0), CSize(m_ui_data.widnow_width, m_ui_data.window_height));
}

void CMiniModeUI::AddMouseToolTip(BtnKey btn, LPCTSTR str)
{
    m_tool_tip.AddTool(m_pMainWnd, str, m_buttons[btn].rect, btn + 1);
}

void CMiniModeUI::UpdateMouseToolTip(BtnKey btn, LPCTSTR str)
{
    //if (m_buttons[btn].hover)
    //{
    m_tool_tip.UpdateTipText(str, m_pMainWnd, btn + 1);
    //}
}

void CMiniModeUI::UpdateToolTipPosition()
{
}

void CMiniModeUI::AddToolTips()
{
    AddMouseToolTip(BTN_PREVIOUS, CCommon::LoadText(IDS_PREVIOUS));
    AddMouseToolTip(BTN_PLAY_PAUSE, CPlayer::GetInstance().IsPlaying() ? CCommon::LoadText(IDS_PAUSE) : CCommon::LoadText(IDS_PLAY));
    AddMouseToolTip(BTN_NEXT, CCommon::LoadText(IDS_NEXT));
    AddMouseToolTip(BTN_SHOW_PLAYLIST, CCommon::LoadText(IDS_SHOW_HIDE_PLAYLIST));
    AddMouseToolTip(BTN_RETURN, CCommon::LoadText(IDS_BACK_TO_NARMAL));
    AddMouseToolTip(BTN_CLOSE, CCommon::LoadText(IDS_CLOSE));
    AddMouseToolTip(BTN_COVER, _T(""));
    AddMouseToolTip(BTN_PROGRESS, CCommon::LoadText(IDS_SEEK_TO));
    AddMouseToolTip(BTN_SELECT_FOLDER, CCommon::LoadText(IDS_MEDIA_LIB, _T(" (Ctrl+T)")));
    AddMouseToolTip(BTN_FAVOURITE, CCommon::LoadText(IDS_ADD_TO_MA_FAVOURITE));
}
