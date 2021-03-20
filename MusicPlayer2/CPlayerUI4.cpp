#include "stdafx.h"
#include "CPlayerUI4.h"

CPlayerUI4::CPlayerUI4(UIData& ui_data, CWnd* pMainWnd)
    : CPlayerUIBase(ui_data, pMainWnd)
{
}

CPlayerUI4::~CPlayerUI4()
{
}

void CPlayerUI4::_DrawInfo(bool reset /*= false*/)
{
    CRect draw_rect = m_draw_rect;
    bool right_lyric = (!m_ui_data.narrow_mode && !m_ui_data.show_playlist) || draw_rect.Width() > DPI(600);        //是否在界面右侧显示歌词

    //绘制状态条
    if (CPlayerUIHelper::IsDrawStatusBar())
    {
        CRect rc_status_bar = m_draw_rect;
        draw_rect.bottom -= DPI(20);
        rc_status_bar.top = draw_rect.bottom;
        DrawStatusBar(rc_status_bar, reset);
    }

    if (m_ui_data.full_screen)
    {
        draw_rect.DeflateRect(EdgeMargin(true), EdgeMargin(false));
    }

    if (right_lyric)
    {
        draw_rect.right = draw_rect.left + draw_rect.Width() / 2;
    }


    //绘制专辑封面
    int cover_height = 0;
    CRect rect_cover{ draw_rect };
    if (IsDrawNarrowMode())
        rect_cover.bottom -= DPI(64);
    else
        rect_cover.bottom -= DPI(84);
    if (rect_cover.bottom > rect_cover.top)
    {
        m_draw_data.thumbnail_rect = rect_cover;
        m_draw.SetDrawArea(rect_cover);
        DrawAlbumCover(rect_cover);
        cover_height = rect_cover.Height();

        //绘制播放信息
        CRect rect_info{ rect_cover };
        rect_info.top = rect_info.bottom - DPI(60);
        if (rect_info.top < draw_rect.top)
            rect_info.top = draw_rect.top;

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
        m_draw.DrawScrollText(rect_artist, CPlayer::GetInstance().GetCurrentSongInfo().GetArtist().c_str(), text_color, GetScrollTextPixel(true), false, scroll_info_artist, reset);
        //绘制歌曲标题
        CRect rect_title{ rect_info };
        rect_title.top = rect_artist.bottom;
        CFont* pOldFont = m_draw.SetFont(&theApp.m_font_set.ui4_title.GetFont(theApp.m_ui_data.full_screen));
        static CDrawCommon::ScrollInfo scroll_info_title;
        m_draw.DrawScrollText(rect_title, CPlayer::GetInstance().GetCurrentSongInfo().GetTitle().c_str(), text_color, GetScrollTextPixel(true), false, scroll_info_title, reset);
        m_draw.SetFont(pOldFont);
    }
    m_draw.SetDrawArea(m_draw_rect);

    //绘制进度条
    CRect progress_rect{ rect_cover };
    progress_rect.top = rect_cover.bottom;
    progress_rect.bottom = progress_rect.top + DPI(4);
    DrawProgess(progress_rect);

    //绘制控制按钮
    CRect rect_control_bar = draw_rect;
    rect_control_bar.top = progress_rect.bottom;
    const int btn_size = DPI(36);
    const int btn_gap{ m_ui_data.full_screen ? DPI(6) : 0 };
    const int btn_num = 7;
    const int control_bar_width = btn_num * btn_size + btn_gap * (btn_num - 1);
    rect_control_bar.top += (rect_control_bar.Height() - btn_size) / 2;
    rect_control_bar.bottom = rect_control_bar.top + btn_size;
    rect_control_bar.left += (rect_control_bar.Width() - control_bar_width) / 2;
    rect_control_bar.right = rect_control_bar.left + control_bar_width;

    auto btnRectChg = [this](CRect rc_btn)
    {
        rc_btn.DeflateRect(DPI(4), DPI(4));
        return rc_btn;
    };

    //绘制循环模式按钮
    CRect rect_btn{ rect_control_bar };
    rect_btn.right = rect_btn.left + btn_size;
    IconRes* pIcon = GetRepeatModeIcon();
    if (pIcon != nullptr)
        DrawUIButton(btnRectChg(rect_btn), m_buttons[BTN_REPETEMODE], *pIcon);
    //绘制红心按钮
    rect_btn.MoveToX(rect_btn.right + btn_gap);
    DrawUIButton(btnRectChg(rect_btn), m_buttons[BTN_FAVOURITE], (CPlayer::GetInstance().IsFavourite() ? theApp.m_icon_set.heart : theApp.m_icon_set.favourite));
    //绘制上一曲按钮
    rect_btn.MoveToX(rect_btn.right + btn_gap);
    DrawUIButton(rect_btn, m_buttons[BTN_PREVIOUS], theApp.m_icon_set.previous_l);
    //绘制播放/暂停按钮
    rect_btn.MoveToX(rect_btn.right + btn_gap);
    DrawUIButton(rect_btn, m_buttons[BTN_PLAY_PAUSE], (CPlayer::GetInstance().IsPlaying() ? theApp.m_icon_set.pause_l : theApp.m_icon_set.play_l));
    //绘制下一曲按钮
    rect_btn.MoveToX(rect_btn.right + btn_gap);
    DrawUIButton(rect_btn, m_buttons[BTN_NEXT], theApp.m_icon_set.next_l);
    //绘制媒体库按钮
    rect_btn.MoveToX(rect_btn.right + btn_gap);
    DrawUIButton(btnRectChg(rect_btn), m_buttons[BTN_SELECT_FOLDER], theApp.m_icon_set.media_lib);
    //绘制播放列表按钮
    rect_btn.MoveToX(rect_btn.right + btn_gap);
    DrawUIButton(btnRectChg(rect_btn), m_buttons[BTN_SHOW_PLAYLIST], theApp.m_icon_set.show_playlist);

    //绘制频谱分析
    const int spectrum_max_width = DPI(280);
    CRect rect_spectrum{ draw_rect };
    rect_spectrum.top = rect_spectrum.bottom - DPI(32);
    if (rect_spectrum.Width() > spectrum_max_width)
    {
        rect_spectrum.left += (rect_spectrum.Width() - spectrum_max_width) / 2;
        rect_spectrum.right = rect_spectrum.left + spectrum_max_width;
    }
    m_draw.SetDrawArea(rect_spectrum);
    m_draw.DrawSpectrum(rect_spectrum, CUIDrawer::SC_64, false, theApp.m_app_setting_data.spectrum_low_freq_in_center);

    //绘制歌词
    if (right_lyric)
    {
        CRect rect_lyric{ draw_rect };
        //rect_lyric.left = draw_rect.right + Margin();
        //rect_lyric.right = m_draw_rect.right - EdgeMargin(true);
        rect_lyric.MoveToX(rect_lyric.right);
        rect_lyric.DeflateRect(DPI(4), DPI(4));
        m_draw.SetDrawArea(rect_lyric);
        if (theApp.m_app_setting_data.lyric_background)
        {
            if (IsDrawBackgroundAlpha())
                m_draw.FillAlphaRect(rect_lyric, m_colors.color_lyric_back, ALPHA_CHG(theApp.m_app_setting_data.background_transparency) * 3 / 5);
            else
                m_draw.FillRect(rect_lyric, m_colors.color_lyric_back);
        }
        rect_lyric.DeflateRect(DPI(4), DPI(4));
        m_draw.DrawLryicCommon(rect_lyric, theApp.m_app_setting_data.lyric_align);
        m_draw_data.lyric_rect = rect_lyric;        //保存歌词区域
    }

    //绘制右上角图标
    DrawTopRightIcons(true);

    //全屏模式时在右上角绘制时间
    if (m_ui_data.full_screen)
    {
        DrawCurrentTime();
    }
}

int CPlayerUI4::GetClassId()
{
    return 4000;
}
