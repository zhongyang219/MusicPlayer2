#include "stdafx.h"
#include "CPlayerUI5.h"

CPlayerUI5::CPlayerUI5(UIData& ui_data, CWnd* pMainWnd)
    : CPlayerUIBase(ui_data, pMainWnd)
{
}

CPlayerUI5::~CPlayerUI5()
{
}

void CPlayerUI5::_DrawInfo(CRect draw_rect, bool reset)
{
    if (m_ui_data.full_screen)
    {
        draw_rect.DeflateRect(EdgeMargin(true), EdgeMargin(false));
    }

    //绘制歌词
    CRect rect_lyric{ draw_rect };
    if (IsDrawNarrowMode())
        rect_lyric.bottom -= DPI(154);
    else
        rect_lyric.bottom -= DPI(182);
    int lyric_bottum = rect_lyric.bottom;
    if (rect_lyric.bottom > rect_lyric.top)
    {
        rect_lyric.DeflateRect(DPI(4), DPI(4));
        m_draw.SetDrawArea(rect_lyric);
        m_draw_data.lyric_rect = rect_lyric;        //保存歌词区域
        DrawLyrics(rect_lyric);
    }

    //绘制专辑封面和歌曲信息
    CRect rect_info{ draw_rect };
    rect_info.top = lyric_bottum - DPI(4);
    rect_info.bottom = rect_info.top + DPI(98);

    //设置任务预览窗口区域
    m_draw_data.thumbnail_rect = rect_info;
    m_draw_data.thumbnail_rect.bottom = draw_rect.bottom;

    //绘制专辑封面
    CRect rect_cover = rect_info;
    rect_cover.right = rect_cover.left + rect_info.Height();
    rect_cover.DeflateRect(DPI(8), DPI(8));
    m_draw.SetDrawArea(rect_cover);
    DrawAlbumCover(rect_cover);
    m_draw.SetDrawArea(draw_rect);

    //绘制歌曲信息
    rect_info.left = rect_cover.right;
    rect_info.DeflateRect(DPI(8), DPI(12));
    //绘制标题
    CRect rect_title{ rect_info };
    rect_title.DeflateRect(0, DPI(8));
    rect_title.bottom -= DPI(36);
    wstring str_title;
    if (CPlayer::GetInstance().GetCurrentSongInfo().IsTitleEmpty())             //如果标题为空，则显示文件名
        str_title = CPlayer::GetInstance().GetCurrentSongInfo().GetFileName();
    else
        str_title = CPlayer::GetInstance().GetCurrentSongInfo().GetTitle();
    CFont* pOldFont = m_draw.SetFont(&theApp.m_font_set.font12.GetFont(theApp.m_ui_data.full_screen));
    static CDrawCommon::ScrollInfo scroll_info_title;
    m_draw.DrawScrollText(rect_title, str_title.c_str(), m_colors.color_text, GetScrollTextPixel(true), false, scroll_info_title, reset);
    m_draw.SetFont(pOldFont);
    //绘制艺术家
    CRect rect_artist{ rect_info };
    rect_artist.top = rect_title.bottom;
    pOldFont = m_draw.SetFont(&theApp.m_font_set.font10.GetFont(theApp.m_ui_data.full_screen));
    static CDrawCommon::ScrollInfo scroll_info_artist;
    m_draw.DrawScrollText(rect_artist, CPlayer::GetInstance().GetCurrentSongInfo().GetArtist().c_str(), m_colors.color_text, GetScrollTextPixel(true), false, scroll_info_artist, reset);
    m_draw.SetFont(pOldFont);

    //绘制进度条
    CRect rect_progressbar{ draw_rect };
    rect_progressbar.left = rect_cover.left;
    rect_progressbar.right = rect_info.right;
    rect_progressbar.top = rect_cover.bottom + DPI(8);
    rect_progressbar.bottom = rect_progressbar.top + DPI(16);
    DrawProgressBar(rect_progressbar);

    //绘制控制按钮
    CRect rect_control_bar{ rect_progressbar };
    rect_control_bar.top = rect_progressbar.bottom - DPI(4);
    rect_control_bar.bottom = draw_rect.bottom - DPI(8);
    const int btn_size = DPI(36);
    const int btn_gap{ m_ui_data.full_screen ? DPI(6) : 0 };
    rect_control_bar.top += (rect_control_bar.Height() - btn_size) / 2;
    rect_control_bar.bottom = rect_control_bar.top + btn_size;

    auto btnRectChg = [this](CRect rc_btn)
    {
        rc_btn.DeflateRect(DPI(4), DPI(4));
        return rc_btn;
    };

    //绘制上一曲按钮
    CRect rect_btn{ rect_control_bar };
    rect_btn.right = rect_btn.left + btn_size;
    DrawUIButton(rect_btn, m_buttons[BTN_PREVIOUS], theApp.m_icon_set.previous_l);
    //绘制播放/暂停按钮
    rect_btn.MoveToX(rect_btn.right + btn_gap);
    DrawUIButton(rect_btn, m_buttons[BTN_PLAY_PAUSE], (CPlayer::GetInstance().IsPlaying() ? theApp.m_icon_set.pause_l : theApp.m_icon_set.play_l));
    //绘制下一曲按钮
    rect_btn.MoveToX(rect_btn.right + btn_gap);
    DrawUIButton(rect_btn, m_buttons[BTN_NEXT], theApp.m_icon_set.next_l);
    //绘制循环模式按钮
    rect_btn.MoveToX(rect_btn.right + btn_gap);
    IconRes* pIcon = GetRepeatModeIcon();
    if (pIcon != nullptr)
        DrawUIButton(btnRectChg(rect_btn), m_buttons[BTN_REPETEMODE], *pIcon);
    //绘制红心按钮
    rect_btn.MoveToX(rect_btn.right + btn_gap);
    DrawUIButton(btnRectChg(rect_btn), m_buttons[BTN_FAVOURITE], (CPlayer::GetInstance().IsFavourite() ? theApp.m_icon_set.heart : theApp.m_icon_set.favourite));

    //绘制播放列表按钮
    rect_btn.MoveToX(rect_control_bar.right - btn_size);
    DrawUIButton(btnRectChg(rect_btn), m_buttons[BTN_SHOW_PLAYLIST], theApp.m_icon_set.show_playlist);
    //绘制媒体库按钮
    rect_btn.MoveToX(rect_btn.left - btn_size);
    DrawUIButton(btnRectChg(rect_btn), m_buttons[BTN_SELECT_FOLDER], theApp.m_icon_set.media_lib);

    //绘制频谱分析
    if (theApp.m_app_setting_data.show_spectrum)
    {
        const int spectrum_max_width = DPI(280);
        CRect rect_spectrum{ draw_rect };
        rect_spectrum.top = rect_spectrum.bottom - DPI(32);
        rect_spectrum.left += DPI(24);
        rect_spectrum.right = rect_spectrum.left + spectrum_max_width;
        m_draw.SetDrawArea(rect_spectrum);
        m_draw.DrawSpectrum(rect_spectrum, CUIDrawer::SC_64, false, theApp.m_app_setting_data.spectrum_low_freq_in_center);
    }

    //绘制右上角图标
    DrawTopRightIcons(true);

    //全屏模式时在右上角绘制时间
    if (m_ui_data.full_screen)
    {
        DrawCurrentTime();
    }

}

int CPlayerUI5::GetClassId()
{
    return 5000;
}
