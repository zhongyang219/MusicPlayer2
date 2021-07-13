#include "stdafx.h"
#include "CPlayerUI.h"


CPlayerUI::CPlayerUI(UIData& ui_data, CWnd* pMainWnd)
    : CPlayerUIBase(ui_data, pMainWnd)
{
}


CPlayerUI::~CPlayerUI()
{
}

void CPlayerUI::_DrawInfo(CRect draw_rect, bool reset)
{
    //draw_rect.MoveToXY(0, 0);

    CPoint text_start{ draw_rect.left + SpectralSize().cx + Margin() + EdgeMargin(true), draw_rect.top + EdgeMargin(false) };       //文本的起始坐标
    int text_height{ DPI(18) };     //文本的高度

    //全屏模式时在右上角绘制时间
    if (m_ui_data.full_screen)
    {
        DrawCurrentTime();
    }


    //绘制右上角图标
    int top_right_icon_size = DrawTopRightIcons();

    //显示歌曲信息
    m_draw.SetFont(&theApp.m_font_set.font9.GetFont(m_ui_data.full_screen));
    //m_draw.SetBackColor(color_back);
    CRect tmp{ text_start, CSize{1, text_height} };
    tmp.right = draw_rect.right - EdgeMargin(true) - top_right_icon_size;
    DrawSongInfo(tmp, reset);

    wchar_t buff[64];
    wstring lable1_str, lable1_content;
    wstring lable2_str, lable2_content;
    wstring lable3_str, lable3_content;
    wstring lable4_str, lable4_content;
    lable1_str = CCommon::LoadText(IDS_TITLE, _T(": "));
    lable1_content = CPlayer::GetInstance().GetCurrentSongInfo().GetTitle();
    if (CPlayer::GetInstance().IsMidi())
    {
        const MidiInfo& midi_info{ CPlayer::GetInstance().GetMidiInfo() };
        lable2_str = CCommon::LoadText(IDS_RHYTHM, _T(": "));
        swprintf_s(buff, L"%d/%d (%dbpm)", midi_info.midi_position, midi_info.midi_length, midi_info.speed);
        lable2_content = buff;

        //lable3_str = _T("速度：");
        //swprintf_s(buff, L"%d bpm", midi_info.speed);
        //lable3_content = buff;

        lable3_str = CCommon::LoadText(IDS_SOUND_FONT, _T(": "));
        lable3_content = CPlayer::GetInstance().GetSoundFontName();
    }
    else
    {
        lable2_str = CCommon::LoadText(IDS_ARTIST, _T(": "));
        lable2_content = CPlayer::GetInstance().GetCurrentSongInfo().GetArtist();
        lable3_str = CCommon::LoadText(IDS_ALBUM, _T(": "));
        lable3_content = CPlayer::GetInstance().GetCurrentSongInfo().GetAlbum();
    }
    lable4_str = CCommon::LoadText(IDS_FORMAT, _T(": "));
    //const BASS_CHANNELINFO channel_info{ CPlayer::GetInstance().GetChannelInfo() };
    lable4_content = GetDisplayFormatString();
    //显示标题
    tmp.MoveToXY(text_start.x, text_start.y + text_height);
    tmp.right = tmp.left + DPI(52);
    m_draw.DrawWindowText(tmp, lable1_str.c_str(), m_colors.color_text_lable);
    tmp.MoveToX(tmp.left + DPI(52));
    tmp.right = draw_rect.right - EdgeMargin(true) - top_right_icon_size;
    static CDrawCommon::ScrollInfo scroll_info2;
    m_draw.DrawScrollText2(tmp, lable1_content.c_str(), m_colors.color_text, GetScrollTextPixel(true), false, scroll_info2, reset);
    //显示艺术家
    tmp.MoveToXY(text_start.x, text_start.y + 2 * text_height);
    tmp.right = tmp.left + DPI(52);
    m_draw.DrawWindowText(tmp, lable2_str.c_str(), m_colors.color_text_lable);
    tmp.MoveToX(tmp.left + DPI(52));
    tmp.right = draw_rect.right - EdgeMargin(true);
    static CDrawCommon::ScrollInfo scroll_info3;
    if (CPlayer::GetInstance().IsMidi())
        m_draw.DrawWindowText(tmp, lable2_content.c_str(), m_colors.color_text);
    else
        m_draw.DrawScrollText2(tmp, lable2_content.c_str(), m_colors.color_text, GetScrollTextPixel(true), false, scroll_info3, reset);
    //显示唱片集
    tmp.MoveToXY(text_start.x, text_start.y + 3 * text_height);
    tmp.right = tmp.left + DPI(52);
    m_draw.DrawWindowText(tmp, lable3_str.c_str(), m_colors.color_text_lable);
    tmp.MoveToX(tmp.left + DPI(52));
    tmp.right = draw_rect.right - EdgeMargin(true);
    static CDrawCommon::ScrollInfo scroll_info4;
    m_draw.DrawScrollText2(tmp, lable3_content.c_str(), m_colors.color_text, GetScrollTextPixel(true), false, scroll_info4, reset);
    //显示文件格式和比特率
    tmp.MoveToXY(text_start.x, text_start.y + 4 * text_height);
    tmp.right = tmp.left + DPI(52);
    m_draw.DrawWindowText(tmp, lable4_str.c_str(), m_colors.color_text_lable);
    tmp.MoveToX(tmp.left + DPI(52));
    tmp.right = draw_rect.right - EdgeMargin(true);
    static CDrawCommon::ScrollInfo scroll_info5;
    m_draw.DrawScrollText2(tmp, lable4_content.c_str(), m_colors.color_text, GetScrollTextPixel(true), false, scroll_info5, reset);

    //显示频谱分析
    CRect spectral_rect{ CPoint{draw_rect.left + EdgeMargin(true), draw_rect.top + EdgeMargin(false)}, SpectralSize() };
    //绘制背景
    if (IsDrawBackgroundAlpha())
        m_draw.FillAlphaRect(spectral_rect, m_colors.color_spectrum_back, ALPHA_CHG(theApp.m_app_setting_data.background_transparency) * 2 / 3);
    else
        m_draw.FillRect(spectral_rect, m_colors.color_spectrum_back);

    CRect cover_rect = spectral_rect;
    cover_rect.DeflateRect(Margin() / 2, Margin() / 2);
    m_buttons[BTN_COVER].rect = DrawAreaToClient(cover_rect, m_draw_rect);
    //绘制专辑封面
    DrawAlbumCover(cover_rect);

    if (theApp.m_app_setting_data.show_spectrum)
    {
        m_draw.SetDrawArea(cover_rect);
        const int ROWS = 32;        //要显示的频谱柱形的数量
        int gap_width{ theApp.DPIRound(1, 0.4) };       //频谱柱形间隙宽度
        int width = (spectral_rect.Width() - (ROWS - 2) * gap_width) / (ROWS - 2);
        COLORREF color;
        if (theApp.m_app_setting_data.show_album_cover && CPlayer::GetInstance().AlbumCoverExist())
            color = m_colors.color_spectrum_cover;
        else
            color = m_colors.color_spectrum;
        m_draw.DrawSpectrum(cover_rect, width, gap_width, ROWS, color, false, theApp.m_app_setting_data.spectrum_low_freq_in_center);
    }

    //绘制工具条
    CPoint point{ spectral_rect.left, spectral_rect.bottom };
    point.y += 2 * Margin();
    CRect other_info_rect{ point, CSize(draw_rect.Width() - 2 * EdgeMargin(true), DPI(24)) };
    DrawToolBar(other_info_rect, IsDrawNarrowMode());

    //显示歌词
    CRect lyric_rect;
    if (IsDrawNarrowMode())
    {
        lyric_rect = other_info_rect;
        lyric_rect.MoveToY(other_info_rect.bottom + Margin());
        if (lyric_rect.Width() >= m_progress_on_top_threshold)      //如果界面宽度足够大导致进度条不显示在按钮上方而是右侧，则歌词区域可以有更大的高度
            lyric_rect.bottom += DPI(16);

        if (draw_rect.bottom - lyric_rect.bottom < DPI(40))
            lyric_rect.bottom = draw_rect.bottom - DPI(40);

        //绘制歌词
        DrawLyrics(lyric_rect, 0);
    }
    else
    {
        //if (CPlayer::GetInstance().IsPlaying() || reset)
        //{

        int control_bar_height;
        if (draw_rect.Width() - 2 * EdgeMargin(true) < m_progress_on_top_threshold)     //如果控制条的宽度小于一定值，则增加其高度，以便将进度条显示在按钮上方
            control_bar_height = DPI(50);
        else
            control_bar_height = DPI(36);

        lyric_rect = draw_rect;
        lyric_rect.MoveToY(other_info_rect.bottom + Margin() + control_bar_height);
        lyric_rect.bottom = draw_rect.bottom;
        DrawLyricsArea(lyric_rect);
        //}
    }
    m_draw_data.lyric_rect = lyric_rect;
    m_draw_data.thumbnail_rect = draw_rect;
    if (!IsDrawNarrowMode())
    {
        m_draw_data.thumbnail_rect.bottom = lyric_rect.top;
    }

    //绘制播放控制条
    CRect rc_control_bar;
    if (IsDrawNarrowMode())
    {
        rc_control_bar.top = lyric_rect.bottom + Margin();
        rc_control_bar.left = Margin();
        rc_control_bar.right = draw_rect.right - Margin();
        rc_control_bar.bottom = draw_rect.bottom - Margin();
    }
    else
    {
        rc_control_bar.top = other_info_rect.bottom + Margin();
        rc_control_bar.left = EdgeMargin(true);
        rc_control_bar.right = draw_rect.right - EdgeMargin(true);
        rc_control_bar.bottom = lyric_rect.top;
    }
    DrawControlBar(rc_control_bar);

    //绘制音量调按钮，因为必须在上层，所以必须在最后绘制
    DrawVolumnAdjBtn();
}

void CPlayerUI::DrawLyricsArea(CRect lyric_rect)
{
    bool draw_background{ IsDrawBackgroundAlpha() };
    bool midi_lyric = CPlayerUIHelper::IsMidiLyric();
    //显示“歌词秀”
    CRect tmp;
    tmp = lyric_rect;
    tmp.left += (Margin() + EdgeMargin(true));
    tmp.bottom = tmp.top + DPI(28);
    m_draw.SetFont(&theApp.m_font_set.font9.GetFont(m_ui_data.full_screen));
    m_draw.DrawWindowText(tmp, CCommon::LoadText(IDS_LYRIC_SHOW, _T(": ")), m_colors.color_text);
    //显示翻译按钮
    CRect translate_rect{ tmp };
    translate_rect.DeflateRect(DPI(4), DPI(4));
    translate_rect.right = lyric_rect.right - (Margin() + EdgeMargin(true));
    translate_rect.left = translate_rect.right - translate_rect.Height();
    DrawTranslateButton(translate_rect);

    //绘制歌词
    CRect lyric_area = lyric_rect;
    lyric_area.DeflateRect(Margin() + EdgeMargin(true), 0);
    lyric_area.top += 2 * Margin();
    lyric_area.bottom -= (Margin() + EdgeMargin(false));
    lyric_area.top += DPI(20);
    DrawLyrics(lyric_area, 2 * Margin());
}

CSize CPlayerUI::SpectralSize()
{
    CSize size = m_layout.spectral_size;
    if (m_ui_data.full_screen)
    {
        size.cx = static_cast<int>(size.cx * CONSTVAL::FULL_SCREEN_ZOOM_FACTOR);
        size.cy = static_cast<int>(size.cy * CONSTVAL::FULL_SCREEN_ZOOM_FACTOR);
    }
    return size;
}

//void CPlayerUI::RButtonUp(CPoint point)
//{
//  CPlayerUIBase::RButtonUp(point);
//
//  if (m_buttons[BTN_REPETEMODE].rect.PtInRect(point))
//      return;
//
//  CPoint point1;      //定义一个用于确定光标位置的位置
//  GetCursorPos(&point1);  //获取当前光标的位置，以便使得菜单可以跟随光标，该位置以屏幕左上角点为原点，point则以客户区左上角为原点
//
//  if (!m_draw_data.lyric_rect.PtInRect(point))
//  {
//      m_main_popup_menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, m_pMainWnd);
//  }
//  else
//  {
//      m_popup_menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, m_pMainWnd);
//  }
//
//}

void CPlayerUI::MouseMove(CPoint point)
{
    CPlayerUIBase::MouseMove(point);

    ////显示专辑封面的提示
    //if (theApp.m_nc_setting_data.show_cover_tip && theApp.m_app_setting_data.show_album_cover)
    //{
    //  CRect cover_rect{ m_draw_data.cover_rect };
    //  cover_rect.MoveToXY(m_draw_rect.left + m_draw_data.cover_rect.left, m_draw_rect.top + m_draw_data.cover_rect.top);
    //  bool show_cover_tip{ cover_rect.PtInRect(point) != FALSE };
    //  static bool last_show_cover_tip{ false };
    //  if (!last_show_cover_tip && show_cover_tip)
    //  {
    //      CString info;
    //      if (CPlayer::GetInstance().AlbumCoverExist())
    //      {
    //          info = CCommon::LoadText(IDS_ALBUM_COVER, _T(": "));
    //          //CFilePathHelper cover_path(CPlayer::GetInstance().GetAlbumCoverPath());
    //          //if (cover_path.GetFileNameWithoutExtension() == ALBUM_COVER_NAME)
    //          if (CPlayer::GetInstance().IsInnerCover())
    //          {
    //              info += CCommon::LoadText(IDS_INNER_ALBUM_COVER_TIP_INFO);
    //              switch (CPlayer::GetInstance().GetAlbumCoverType())
    //              {
    //              case 0: info += _T("jpg"); break;
    //              case 1: info += _T("png"); break;
    //              case 2: info += _T("gif"); break;
    //              }
    //          }
    //          else
    //          {
    //              info += CCommon::LoadText(IDS_OUT_IMAGE, _T("\r\n"));
    //              info += CPlayer::GetInstance().GetAlbumCoverPath().c_str();
    //          }
    //      }
    //      m_tool_tip->AddTool(m_pMainWnd, info);
    //      m_tool_tip->SetMaxTipWidth(DPI(400));
    //      m_tool_tip->Pop();
    //  }
    //  if (last_show_cover_tip && !show_cover_tip)
    //  {
    //      m_tool_tip->AddTool(m_pMainWnd, _T(""));
    //      m_tool_tip->Pop();
    //  }
    //  last_show_cover_tip = show_cover_tip;
    //}

}

//void CPlayerUI::OnSizeRedraw(int cx, int cy)
//{
//    m_show_volume_adj = false;
//    CPlayerUIBase::OnSizeRedraw(cx, cy);
//}

//CRect CPlayerUI::GetThumbnailClipArea()
//{
//  //CRect info_rect;
//  //if (!IsDrawNarrowMode())
//  //  info_rect = CRect{ CPoint{ Margin(), Margin() + DPI(20) }, CSize{ m_ui_data.client_width / 2 - 2 * Margin(), m_layout.info_height2 - 3 * Margin() } };
//  //else
//  //  info_rect = CRect{ CPoint{ Margin(), DPI(20) }, CSize{ m_ui_data.client_width - 2 * Margin(), m_layout.info_height - 2 * Margin() } };
//
//  return info_rect;
//}

int CPlayerUI::GetClassId()
{
    return 1000;
}
