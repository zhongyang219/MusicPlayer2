#include "stdafx.h"
#include "CUIDrawer.h"
#include "MusicPlayer2.h"
#include "Player.h"

CUIDrawer::CUIDrawer(UIColors& colors)
    : m_colors(colors)
{
}


CUIDrawer::~CUIDrawer()
{
}

void CUIDrawer::SetLyricFont(CFont* lyric_font, CFont* lyric_tr_font)
{
    m_lyric_font = lyric_font;
    m_lyric_tr_font = lyric_tr_font;
}

void CUIDrawer::DrawLryicCommon(CRect rect, Alignment align, bool show_song_info)
{
    DrawAreaGuard guard(this, rect);
    static int flag{};
    if (!IsDrawMultiLine(rect.Height()))
        DrawLyricTextSingleLine(rect, flag, true, align, show_song_info);
    else
        DrawLyricTextMultiLine(rect, align, show_song_info);
}

int CUIDrawer::GetLyricTextHeight() const
{
    //计算文本高度
    CFont* pOldFont = m_pDC->SelectObject(m_lyric_font);
    int height = m_pDC->GetTextExtent(L"文").cy;
    m_pDC->SelectObject(pOldFont);
    return height;  //根据当前的字体设置计算文本的高度
}

void CUIDrawer::Create(CDC* pDC, CFont* pFont)
{
    CDrawCommon::Create(pDC, pFont);
}

bool CUIDrawer::IsDrawMultiLine(int height) const
{
    return height >= static_cast<int>(GetLyricTextHeight() * 3.5);
}

void CUIDrawer::SetForCortanaLyric(bool for_cortana_lyric)
{
    m_for_cortana_lyric = for_cortana_lyric;
}

void CUIDrawer::DrawLyricTextMultiLine(CRect lyric_area, Alignment align, bool show_song_info)
{
    // AUTO时多行歌词居中显示
    if (align == Alignment::AUTO) align = Alignment::CENTER;

    int line_space{};
    if (m_for_cortana_lyric)
    {
        line_space = theApp.DPI(4);
    }
    else
    {
        line_space = theApp.m_lyric_setting_data.lyric_line_space;
        if (theApp.m_ui_data.full_screen)
            line_space = static_cast<int>(line_space * CONSTVAL::FULL_SCREEN_ZOOM_FACTOR);
    }

    int lyric_height = GetLyricTextHeight() + line_space;			//文本高度加上行间距
    int lyric_height2 = lyric_height * 2 + line_space;		//包含翻译的歌词高度

    CFont* pOldFont = SetFont(m_lyric_font);
    if (CPlayer::GetInstance().IsPlaylistEmpty())   //当前播放为空时在歌词区域显示播放提示
    {
        CFont* font = SetFont(&theApp.m_font_set.GetFontBySize(10).GetFont());
        wstring no_track_tip_str = theApp.m_str_table.LoadTextFormat(L"UI_LYRIC_NO_TRACKS_TIP", {
            theApp.m_accelerator_res.GetShortcutDescriptionById(ID_SHOW_PLAYLIST),
            theApp.m_accelerator_res.GetShortcutDescriptionById(ID_FILE_OPEN),
            theApp.m_accelerator_res.GetShortcutDescriptionById(ID_FILE_OPEN_FOLDER),
            theApp.m_accelerator_res.GetShortcutDescriptionById(ID_MEDIA_LIB)});
        DrawWindowText(lyric_area, no_track_tip_str.c_str(), m_colors.color_text_2, Alignment::LEFT, false, true);
        SetFont(font);
    }
    else if (CPlayerUIHelper::IsMidiLyric())
    {
        wstring current_lyric{ CPlayer::GetInstance().GetMidiLyric() };
        DrawWindowText(lyric_area, current_lyric.c_str(), m_colors.color_text, Alignment::CENTER, false, true);
    }
    else if (CPlayer::GetInstance().m_Lyrics.IsEmpty())
    {
        //没有歌词时显示歌曲信息
        if (theApp.m_lyric_setting_data.show_song_info_if_lyric_not_exist || show_song_info)
        {
            CString song_info_str;
            const SongInfo& cur_song{ CPlayer::GetInstance().GetCurrentSongInfo() };
            std::wstring artist{ cur_song.album_artist };
            if (artist.empty())
                artist = cur_song.GetArtist();
            song_info_str.Format(_T("%s - %s\r\n%s"), artist.c_str(), cur_song.GetAlbum().c_str(), cur_song.GetTitle().c_str());
            DrawWindowText(lyric_area, song_info_str, m_colors.color_text, align, false, true);
        }
        //显示“当前歌曲没有歌词”
        else
        {
            static const wstring& no_lyric_info = theApp.m_str_table.LoadText(L"UI_LYRIC_NONE");
            DrawWindowText(lyric_area, no_lyric_info.c_str(), m_colors.color_text_2, Alignment::CENTER);
        }
    }
    else
    {
        //CRect arect{ lyric_area };		//一行歌词的矩形区域
        //arect.bottom = arect.top + lyric_height;
        //vector<CRect> rects(CPlayer::GetInstance().m_Lyrics.GetLyricCount() + 1, arect);
        //为每一句歌词创建一个矩形，保存在容器里
        vector<CRect> rects;
        int lyric_count = CPlayer::GetInstance().m_Lyrics.GetLyricCount() + 1;		//获取歌词数量（由于第一行歌词需要显示标题，所以这里要+1）
        for (int i{}; i < lyric_count; i++)
        {
            CRect arect{ lyric_area };
            if (!CPlayer::GetInstance().m_Lyrics.GetLyric(i - 1).translate.empty() && theApp.m_lyric_setting_data.show_translate)
                arect.bottom = arect.top + lyric_height2;
            else
                arect.bottom = arect.top + lyric_height;
            rects.push_back(arect);
        }
        int center_pos = (lyric_area.top + lyric_area.bottom) / 2;		//歌词区域的中心y坐标
        Time time{ CPlayer::GetInstance().GetCurrentPosition() };		//当前播放时间
        int lyric_index = CPlayer::GetInstance().m_Lyrics.GetLyricIndex(time);		            // 当前歌词的序号
        int progress{ CPlayer::GetInstance().m_Lyrics.GetLyricProgress(time, false, false, [this](const wstring& str) { return GetTextExtent(str.c_str()).cx; }) };		// 当前歌词进度（范围为0~1000），多行歌词使用的进度不含进度符号
        int y_progress;			//当前歌词在y轴上的进度
        if (!CPlayer::GetInstance().m_Lyrics.GetLyric(lyric_index).translate.empty() && theApp.m_lyric_setting_data.show_translate)
            y_progress = progress * lyric_height2 / 1000;
        else
            y_progress = progress * lyric_height / 1000;
        //int start_pos = center_pos - y_progress - (lyric_index + 1)*lyric_height;		//第1句歌词的起始y坐标
        //计算第1句歌词的起始y坐标
        //由于当前歌词需要显示在歌词区域的中心位置，因此从中心位置开始，减去当前歌词在Y轴上的进度
        //再依次减去之前每一句歌词的高度，即得到了第一句歌词的起始位置
        int start_pos;
        start_pos = center_pos - y_progress;
        for (int i{ lyric_index - 1 }; i >= -1; i--)
        {
            if (theApp.m_lyric_setting_data.show_translate && !CPlayer::GetInstance().m_Lyrics.GetLyric(i).translate.empty())
                start_pos -= lyric_height2;
            else
                start_pos -= lyric_height;
        }

        //依次绘制每一句歌词
        for (int i{ -1 }; i < static_cast<int>(rects.size()) - 1; i++)
        {
            //计算每一句歌词的位置
            if (i == -1)
                rects[i + 1].MoveToY(start_pos);
            else
                rects[i + 1].MoveToY(rects[i].bottom);
            //绘制歌词文本
            if (!(rects[i + 1] & lyric_area).IsRectEmpty())		//只有当一句歌词的矩形区域和歌词区域的矩形有交集时，才绘制歌词
            {
                //设置歌词文本和翻译文本的矩形区域
                CRect rect_text{ rects[i + 1] };
                CRect rect_translate;
                const CLyrics::Lyric& lyric_i = CPlayer::GetInstance().m_Lyrics.GetLyric(i);
                if (!lyric_i.translate.empty() && theApp.m_lyric_setting_data.show_translate)
                {
                    rect_text.MoveToY(rect_text.top + line_space);
                    rect_text.bottom = rect_text.top + GetLyricTextHeight();
                    rect_translate = rect_text;
                    rect_translate.MoveToY(rect_text.bottom + line_space);
                }

                if (i == lyric_index && progress < 1000)		//绘制正在播放的歌词（处于逐渐过度到高亮过程中的歌词）
                {
                    //这里实现文本从非高亮缓慢变化到高亮效果
                    int last_time_span = time - lyric_i.time_start;     //当前播放的歌词已持续的时间
                    int fade_percent = last_time_span / 8;         //计算颜色高亮变化的百分比，除数越大则持续时间越长，10则为1秒
                    COLORREF text_color = CColorConvert::GetGradientColor(m_colors.color_text_2, m_colors.color_text, fade_percent);
                    //绘制歌词文本
                    SetFont(m_lyric_font);
                    if (theApp.m_lyric_setting_data.lyric_karaoke_disp)
                        DrawWindowText(rect_text, lyric_i.text.c_str(), m_colors.color_text, m_colors.color_text_2, progress, align, true);
                    else
                        DrawWindowText(rect_text, lyric_i.text.c_str(), text_color, text_color, progress, align, true);
                    //绘制翻译文本
                    if (!lyric_i.translate.empty() && theApp.m_lyric_setting_data.show_translate)
                    {
                        SetFont(m_lyric_tr_font);
                        DrawWindowText(rect_translate, lyric_i.translate.c_str(), text_color, text_color, progress, align, true);
                    }
                }
                else		//绘制非正在播放的歌词
                {
                    SetFont(m_lyric_font);
                    COLORREF text_color;
                    if (i == lyric_index - 1 || (i == lyric_index && progress == 1000))         // 绘制正在取消高亮的歌词（这里实现一句歌词颜色从高亮缓慢变化到非高亮效果），逐字歌词最后一句在此处取消高亮
                    {
                        int last_time_span = time - (lyric_i.time_start + lyric_i.time_span);   // 引入逐字歌词后上句歌词结束后时长不等于当前播放的歌词已持续的时间，此处应当使用上句歌词结束时间
                        int fade_percent = last_time_span / 20;         //计算颜色高亮变化的百分比，当持续时间为2000毫秒时为100%，即颜色缓慢变化的时间为2秒
                        text_color = CColorConvert::GetGradientColor(m_colors.color_text, m_colors.color_text_2, fade_percent);
                    }
                    else
                    {
                        text_color = m_colors.color_text_2;
                    }
                    //绘制歌词文本
                    DrawWindowText(rect_text, lyric_i.text.c_str(), text_color, align, true);
                    //绘制翻译文本
                    if (!lyric_i.translate.empty() && theApp.m_lyric_setting_data.show_translate)
                    {
                        SetFont(m_lyric_tr_font);
                        DrawWindowText(rect_translate, lyric_i.translate.c_str(), text_color, align, true);
                    }
                }
            }
        }
    }
    SetFont(pOldFont);
}

void CUIDrawer::DrawLyricTextSingleLine(CRect rect, int& flag, bool double_line, Alignment align, bool show_song_info)
{
    CFont* pOldFont = SetFont(m_lyric_font);

    if (CPlayerUIHelper::IsMidiLyric())
    {
        wstring current_lyric{ CPlayer::GetInstance().GetMidiLyric() };
        DrawWindowText(rect, current_lyric.c_str(), m_colors.color_text, Alignment::CENTER, false, true);
    }
    else if (CPlayer::GetInstance().m_Lyrics.IsEmpty())
    {
        //没有歌词时显示歌曲信息
        if (theApp.m_lyric_setting_data.show_song_info_if_lyric_not_exist || show_song_info)
        {
            CString song_info_str;
            const SongInfo& cur_song{ CPlayer::GetInstance().GetCurrentSongInfo() };
            song_info_str.Format(_T("%s - %s"), cur_song.GetArtist().c_str(), cur_song.GetTitle().c_str());
            static CDrawCommon::ScrollInfo lyric_scroll_info;
            DrawScrollText(rect, song_info_str, m_colors.color_text, CPlayerUIHelper::GetScrollTextPixel(), theApp.m_lyric_setting_data.lyric_align != Alignment::LEFT, lyric_scroll_info);
        }
        //显示“当前歌曲没有歌词”
        else
        {
            static const wstring& no_lyric_info = theApp.m_str_table.LoadText(L"UI_LYRIC_NONE");
            DrawWindowText(rect, no_lyric_info.c_str(), m_colors.color_text_2, Alignment::CENTER);
        }
    }
    else
    {
        DrawAreaGuard guard(this, rect);
        CRect lyric_rect = rect;

        static const wstring& empty_lyric = theApp.m_str_table.LoadText(L"UI_LYRIC_EMPTY_LINE");
        const bool karaoke{ theApp.m_lyric_setting_data.lyric_karaoke_disp };
        const bool ignore_blank{ theApp.m_lyric_setting_data.donot_show_blank_lines};
        auto& now_lyrics{ CPlayer::GetInstance().m_Lyrics };
        Time time{ CPlayer::GetInstance().GetCurrentPosition() };
        CLyrics::Lyric current_lyric{ now_lyrics.GetLyric(time, false, ignore_blank, karaoke) };
        int progress{ now_lyrics.GetLyricProgress(time, ignore_blank, karaoke, [this](const wstring& str) { return GetTextExtent(str.c_str()).cx; }) };
        
        //当前歌词为空，且持续了超过了20秒
        bool no_lyric = (current_lyric.text.empty() && CPlayer::GetInstance().GetCurrentPosition() - current_lyric.time_start > 20000) || progress >= 1000;
        //当指定了show_song_info时，如果当前歌词为空，且持续了超过了20秒，则显示歌曲信息
        if (no_lyric && show_song_info)
        {
            //显示歌曲信息
            CString song_info_str;
            const SongInfo& cur_song{ CPlayer::GetInstance().GetCurrentSongInfo() };
            song_info_str.Format(_T("%s - %s"), cur_song.GetArtist().c_str(), cur_song.GetTitle().c_str());
            static CDrawCommon::ScrollInfo lyric_scroll_info;
            DrawScrollText(rect, song_info_str, m_colors.color_text, CPlayerUIHelper::GetScrollTextPixel(), theApp.m_lyric_setting_data.lyric_align != Alignment::LEFT, lyric_scroll_info);
        }
        else
        {
            bool switch_flag{ flag > 5000 };
            switch_flag ^= (flag % 5000) > progress;
            flag = switch_flag ? 10000 + progress : progress;

            if (current_lyric.text.empty())
                current_lyric.text = empty_lyric;
            //双行显示歌词
            if (double_line && (current_lyric.translate.empty() || !theApp.m_lyric_setting_data.show_translate) && rect.Height() > static_cast<int>(GetLyricTextHeight() * 1.73))
            {
                wstring next_lyric_text;
                next_lyric_text = now_lyrics.GetLyric(time, true, ignore_blank, karaoke).text;
                if (next_lyric_text.empty())
                    next_lyric_text = empty_lyric;
                //这里实现文本从非高亮缓慢变化到高亮效果
                int last_time_span = time - current_lyric.time_start;     //当前播放的歌词已持续的时间
                int fade_percent = last_time_span / 8;         //计算颜色高亮变化的百分比，除数越大则持续时间越长，10则为1秒
                if (progress == 1000) fade_percent = 0;         // 进度为1000时当前歌词“已完成”不再高亮
                // 这里的fade_percent当合并空行开启时可能为负，在颜色渐变处规范取值，此处不再处理
                DrawLyricDoubleLine(lyric_rect, current_lyric.text.c_str(), next_lyric_text.c_str(), align, progress, switch_flag, fade_percent);
            }
            else
            {
                // AUTO时单行歌词居中显示
                if (align == Alignment::AUTO) align = Alignment::CENTER;
                // 单行歌词在这里显示翻译，同时更新歌词区域为单行有翻译时的位置
                if (theApp.m_lyric_setting_data.show_translate && !current_lyric.translate.empty() && rect.Height() > static_cast<int>(GetLyricTextHeight() * 1.73))
                {
                    lyric_rect.bottom = lyric_rect.top + rect.Height() / 2;
                    CRect translate_rect = lyric_rect;
                    translate_rect.MoveToY(lyric_rect.bottom);

                    SetFont(m_lyric_tr_font);
                    DrawWindowText(translate_rect, current_lyric.translate.c_str(), m_colors.color_text, m_colors.color_text, progress, align, true);
                }
                // 绘制单行歌词
                SetFont(m_lyric_font);
                if (theApp.m_lyric_setting_data.lyric_karaoke_disp)
                    DrawWindowText(lyric_rect, current_lyric.text.c_str(), m_colors.color_text, m_colors.color_text_2, progress, align, true);
                else if (0 < progress && progress < 1000)   // 仅高亮“正在进行”的歌词
                    DrawWindowText(lyric_rect, current_lyric.text.c_str(), m_colors.color_text, m_colors.color_text, progress, align, true);
                else
                    DrawWindowText(lyric_rect, current_lyric.text.c_str(), m_colors.color_text_2, m_colors.color_text_2, progress, align, true);
            }
        }
    }

    SetFont(pOldFont);
}

void CUIDrawer::DrawSpectrum(CRect rect, SpectrumCol col, bool draw_reflex /*= false*/, bool low_freq_in_center, bool fixed_width, Alignment alignment)
{
    int cols;		//要显示的频谱柱形的数量
    switch (col)
    {
    case CUIDrawer::SC_64:
        cols = 64;
        break;
    case CUIDrawer::SC_32:
        cols = 32;
        break;
    case CUIDrawer::SC_16:
        cols = 16;
        break;
    case CUIDrawer::SC_8:
        cols = 8;
        break;
    default:
        cols = SPECTRUM_COL;
        break;
    }
    int max_width{ rect.Width() };
    if (fixed_width)
    {
        if (col == SC_64)
        {
            max_width = DPI(280);
        }
    }
    double gap_width_double{ max_width * (SPECTRUM_COL / cols) / 168.0 };   //频谱柱形间隙宽度
    if (theApp.m_ui_data.full_screen && !m_for_cortana_lyric)
        gap_width_double *= CONSTVAL::FULL_SCREEN_ZOOM_FACTOR;
    int gap_width{ static_cast<int>(gap_width_double + 0.5) };		//这里加0.5用作将小数四舍五入处理
    int width = (max_width - (cols - 1) * gap_width) / (cols - 1);
    if (gap_width < 1)
        gap_width = 1;
    if (width < 1)
        width = 1;

    DrawAreaGuard guard(this, rect, true, fixed_width);
    DrawSpectrum(rect, width, gap_width, cols, m_colors.color_spectrum, draw_reflex, low_freq_in_center, alignment);
}

void CUIDrawer::DrawSpectrum(CRect rect, int col_width, int gap_width, int cols, COLORREF color, bool draw_reflex, bool low_freq_in_center, Alignment alignment, bool draw_peak, int sprctrum_height)
{
    CRect rc_spectrum_top = rect;
    if (draw_reflex)     //如果要绘制倒影，则倒影占总高度的1/3
        rc_spectrum_top.bottom = rect.top + (rect.Height() * 2 / 3);

    CRect rects[SPECTRUM_COL];
    rects[0] = rc_spectrum_top;
    rects[0].right = rects[0].left + col_width;

    //频谱的实际宽度
    int width_actrual{ col_width * cols + gap_width * (cols - 1) };
    //如果频谱的实际宽度小于矩形的宽度，则让根据alignment的值让频谱居中或右对齐显示
    if ((width_actrual < rect.Width() && alignment == Alignment::CENTER) || (width_actrual >= rect.Width() && theApp.m_app_setting_data.spectrum_low_freq_in_center))
        rects[0].MoveToX(rects[0].left + (rect.Width() - width_actrual) / 2);
    else if (width_actrual < rect.Width() && alignment == Alignment::RIGHT)
        rects[0].MoveToX(rects[0].left + (rect.Width() - width_actrual));

    for (int i{ 1 }; i < cols; i++)
    {
        rects[i] = rects[0];
        rects[i].left += (i * (col_width + gap_width));
        rects[i].right += (i * (col_width + gap_width));
    }
    for (int i{}; i < cols; i++)
    {
        int index;
        if (low_freq_in_center)
        {
            if (i < cols / 2)
                index = (-i + cols / 2) * 2 - 1;
            else
                index = (i - cols / 2) * 2;
        }
        else
        {
            index = i;
        }
        if (index >= cols)
            index = cols;
        if (index < 0)
            index = 0;
        float spetral_data = CPlayer::GetInstance().GetSpectralData()[index * (SPECTRUM_COL / cols)];
        float peak_data = CPlayer::GetInstance().GetSpectralPeakData()[index * (SPECTRUM_COL / cols)];

        CRect rect_tmp{ rects[i] };
        if (sprctrum_height <= 0)
            sprctrum_height = theApp.m_app_setting_data.sprctrum_height;
        int spetral_height = static_cast<int>(spetral_data * rects[0].Height() / 30 * sprctrum_height / 100);
        int peak_height = static_cast<int>(peak_data * rects[0].Height() / 30 * sprctrum_height / 100);
        if (spetral_height < 0 || CPlayer::GetInstance().IsError()) spetral_height = 0;		//如果播放出错，不显示频谱
        if (peak_height < 0 || CPlayer::GetInstance().IsError()) peak_height = 0;

        int peak_rect_height = max(theApp.DPIRound(1.1), gap_width / 2);        //顶端矩形的高度
        spetral_height += peak_rect_height;                                     //频谱至少和顶端矩形一样高
        peak_height += peak_rect_height;

        rect_tmp.top = rect_tmp.bottom - spetral_height;
        if (rect_tmp.top < rects[0].top) rect_tmp.top = rects[0].top;
        FillRect(rect_tmp, color, true);
        //绘制倒影
        if (draw_reflex)
        {
            CRect rc_invert = rect_tmp;
            rc_invert.bottom = rect_tmp.top + rect_tmp.Height() * 2 / 3;
            rc_invert.MoveToY(rect_tmp.bottom + gap_width);
            FillAlphaRect(rc_invert, color, 96, true);
        }

        if (draw_peak)
        {
            //绘制顶端
            CRect rect_peak{ rect_tmp };
            rect_peak.bottom = rect_tmp.bottom - peak_height - gap_width;
            rect_peak.top = rect_peak.bottom - peak_rect_height;
            FillRect(rect_peak, color, true);
            ////绘制顶端倒影
            //CRect rc_peak_invert = rect_peak;
            //rc_peak_invert.MoveToY(rc_invert.top + peak_height + theApp.DPIRound(1.1));
            //FillAlphaRect(rc_peak_invert, color, 96);
        }
    }

}

int CUIDrawer::DPI(int pixel)
{
    if (theApp.m_ui_data.full_screen && !m_for_cortana_lyric)
        return theApp.DPI(pixel * CONSTVAL::FULL_SCREEN_ZOOM_FACTOR);
    else
        return theApp.DPI(pixel);
}

void CUIDrawer::DrawLyricDoubleLine(CRect rect, LPCTSTR lyric, LPCTSTR next_lyric, Alignment align, int progress, bool switch_flag, int fade_percent)
{
    CFont* pOldFont = SetFont(m_lyric_font);

    CRect up_rect{ rect }, down_rect{ rect };       //上半部分和下半部分歌词的矩形区域
    up_rect.bottom = up_rect.top + (up_rect.Height() / 2);
    down_rect.top = down_rect.bottom - (down_rect.Height() / 2);

    // 对齐方式为AUTO时使用上左下右的卡拉OK对齐方式
    Alignment up_align{ Alignment::LEFT }, down_align{ Alignment::RIGHT };
    if (align != Alignment::AUTO)
        up_align = down_align = align;

    COLORREF color1, color2;
    if (theApp.m_lyric_setting_data.lyric_karaoke_disp)
    {
        color1 = m_colors.color_text;
        color2 = m_colors.color_text_2;
    }
    else
    {
        color1 = color2 = CColorConvert::GetGradientColor(m_colors.color_text_2, m_colors.color_text, fade_percent);
    }

    // 绘制当前歌词
    if (!switch_flag)
    {
        DrawWindowText(up_rect, lyric, color1, color2, progress, up_align);
        DrawWindowText(down_rect, next_lyric, m_colors.color_text_2, down_align);
    }
    else
    {
        DrawWindowText(up_rect, next_lyric, m_colors.color_text_2, up_align);
        DrawWindowText(down_rect, lyric, color1, color2, progress, down_align);
    }
    SetFont(pOldFont);
}
