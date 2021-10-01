#include "stdafx.h"
#include "CPlayerUI3.h"
#include "PlayListCtrl.h"


CPlayerUI3::CPlayerUI3(UIData& ui_data, CWnd* pMainWnd)
    : CPlayerUIBase(ui_data, pMainWnd)
{
}

CPlayerUI3::~CPlayerUI3()
{
}

void CPlayerUI3::_DrawInfo(CRect draw_rect, bool reset /*= false*/)
{
    m_draw_data.thumbnail_rect = draw_rect;

    //绘制工具栏
    CRect rc_tool_bar = draw_rect;
    rc_tool_bar.top = rc_tool_bar.bottom - DPI(24) - 2 * Margin();
    rc_tool_bar.DeflateRect(Margin(), Margin());

    //绘制专辑封面
    CRect rc_album = rc_tool_bar;
    rc_album.right = rc_album.left + rc_tool_bar.Height();
    m_draw.SetDrawArea(rc_album);
    bool draw_album_cover{ theApp.m_app_setting_data.show_album_cover && CPlayer::GetInstance().AlbumCoverExist() };
    if (draw_album_cover)
    {
        m_draw.DrawBitmap(CPlayer::GetInstance().GetAlbumCover(), rc_album.TopLeft(), rc_album.Size(), theApp.m_app_setting_data.album_cover_fit);
    }
    else
    {
        int ICON_SIDE = DPI(16);
        int x = rc_album.left + (rc_album.Width() - ICON_SIDE) / 2;
        int y = rc_album.top + (rc_album.Height() - ICON_SIDE) / 2;
        IconRes& icon{ CPlayer::GetInstance().IsPlaying() ? theApp.m_icon_set.default_cover_toolbar : theApp.m_icon_set.default_cover_toolbar_not_played };
        m_draw.DrawIcon(icon.GetIcon(false, IsDrawLargeIcon()), CPoint(x, y), CSize(ICON_SIDE, ICON_SIDE));
    }

    //绘制歌曲信息
    int info_width = rc_tool_bar.Width() / 5;
    if (info_width > DPI(200))
        info_width = DPI(200);
    wstring str_song_info = CPlayListCtrl::GetDisplayStr(CPlayer::GetInstance().GetCurrentSongInfo(), DisplayFormat::DF_ARTIST_TITLE);
    int text_width = m_draw.GetTextExtent(str_song_info.c_str()).cx + DPI(4);
    if (info_width > text_width)
        info_width = text_width;
    CRect rc_tmp = rc_tool_bar;
    int song_info_margin{ draw_album_cover ? DPI(4) : 0 };
    rc_tmp.left = rc_album.right + song_info_margin;
    rc_tmp.right = rc_tmp.left + info_width;
    static CDrawCommon::ScrollInfo scroll_info;
    m_draw.DrawScrollText(rc_tmp, str_song_info.c_str(),
        m_colors.color_text, GetScrollTextPixel(), false, scroll_info, reset);

    //绘制循环模式按钮
    if (m_draw_rect.Width() > DPI(330))
    {
        rc_tmp.left = rc_tmp.right;
        rc_tmp.right = rc_tmp.left + rc_tmp.Height();
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
        if (pIcon != nullptr)
            DrawControlBarBtn(rc_tmp, m_buttons[BTN_REPETEMODE], *pIcon);
    }
    else
    {
        m_buttons[BTN_REPETEMODE].rect = CRect();
        rc_tmp.left = rc_tmp.right - rc_tmp.Height();
    }

    //绘制设置按钮
    if (m_draw_rect.Width() > DPI(430))
    {
        rc_tmp.MoveToX(rc_tmp.right);
        DrawControlBarBtn(rc_tmp, m_buttons[BTN_SETTING], theApp.m_icon_set.setting);
    }
    else
    {
        m_buttons[BTN_SETTING].rect = CRect();
    }

    //绘制更换界面按钮
    //if (m_draw_rect.Width() > DPI(360))
    //{
    rc_tmp.MoveToX(rc_tmp.right);
    DrawControlBarBtn(rc_tmp, m_buttons[BTN_SKIN], theApp.m_icon_set.skin);
    //}
    //else
    //{
    //    m_buttons[BTN_SKIN].rect = CRect();
    //}

    //绘制AB重复按钮
    if (m_draw_rect.Width() > DPI(384))
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

    //绘制上一曲按钮
    rc_tmp.MoveToX(rc_tmp.right);
    DrawControlBarBtn(rc_tmp, m_buttons[BTN_PREVIOUS], theApp.m_icon_set.previous_new);

    //绘制播放/暂停按钮
    rc_tmp.MoveToX(rc_tmp.right);
    const IconRes& icon_res{ CPlayer::GetInstance().IsPlaying() ? theApp.m_icon_set.pause_new : theApp.m_icon_set.play_new };
    DrawControlBarBtn(rc_tmp, m_buttons[BTN_PLAY_PAUSE], icon_res);

    //绘制下一曲按钮
    rc_tmp.MoveToX(rc_tmp.right);
    DrawControlBarBtn(rc_tmp, m_buttons[BTN_NEXT], theApp.m_icon_set.next_new);

    int progress_bar_left = rc_tmp.right;

    //绘制显示播放列表按钮
    rc_tmp = rc_tool_bar;
    rc_tmp.left = rc_tmp.right - rc_tool_bar.Height();
    DrawControlBarBtn(rc_tmp, m_buttons[BTN_SHOW_PLAYLIST], theApp.m_icon_set.show_playlist);

    //绘制音量
    if (m_draw_rect.Width() > DPI(500))
    {
        rc_tmp.right = rc_tmp.left;
        rc_tmp.left = rc_tmp.right - DPI(56);

        DrawVolumeButton(rc_tmp, true);
    }

    //绘制进度条
    CRect rc_progress_bar = rc_tool_bar;
    rc_progress_bar.left = progress_bar_left;
    rc_progress_bar.right = rc_tmp.left - Margin();
    DrawProgressBar(rc_progress_bar);

    //绘制音量调整按钮
    if (m_draw_rect.Width() > DPI(500))
        DrawVolumnAdjBtn();

    //绘制歌词
    CRect rc_lyric = draw_rect;
    rc_lyric.bottom = rc_tool_bar.top;
    rc_lyric.DeflateRect(Margin(), Margin());
    m_draw_data.lyric_rect = rc_lyric;
    DrawLyrics(rc_lyric);

    //全屏模式时在右上角绘制时间
    if (m_ui_data.full_screen)
    {
        DrawCurrentTime();
    }

    //绘制右上角图标
    DrawTopRightIcons(true);

}

int CPlayerUI3::GetClassId()
{
    return 3000;
}
