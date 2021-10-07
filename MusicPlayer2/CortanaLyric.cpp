#include "stdafx.h"
#include "CortanaLyric.h"
#include "PlayListCtrl.h"
#include "CPlayerUIBase.h"

CCriticalSection CCortanaLyric::m_critical;

CCortanaLyric::CCortanaLyric()
{
	m_draw.SetForCortanaLyric();
}


CCortanaLyric::~CCortanaLyric()
{
    if (m_pDC != nullptr)
        m_cortana_wnd->ReleaseDC(m_pDC);

	//退出时恢复Cortana窗口透明
#ifndef COMPILE_IN_WIN_XP
	SetCortanaBarOpaque(false);
#endif // !COMPILE_IN_WIN_XP

}

void CCortanaLyric::Init()
{
    if (m_enable)
    {
        CSingleLock sync(&m_critical, TRUE);
        HWND hTaskBar = ::FindWindow(_T("Shell_TrayWnd"), NULL);	//任务栏的句柄
        m_hCortanaBar = ::FindWindowEx(hTaskBar, NULL, _T("TrayDummySearchControl"), NULL);	//Cortana栏的句柄（其中包含3个子窗口）
        m_cortana_hwnd = ::FindWindowEx(m_hCortanaBar, NULL, _T("Button"), NULL);	//Cortana搜索框中类名为“Button”的窗口的句柄
        m_hCortanaStatic = ::FindWindowEx(m_hCortanaBar, NULL, _T("Static"), NULL);		//Cortana搜索框中类名为“Static”的窗口的句柄
        if (m_cortana_hwnd == NULL) return;
        wchar_t buff[32];
        ::GetWindowText(m_cortana_hwnd, buff, 31);		//获取Cortana搜索框中原来的字符串，用于在程序退出时恢复
        m_cortana_default_text = buff;
        m_cortana_wnd = CWnd::FromHandle(m_cortana_hwnd);		//获取Cortana搜索框的CWnd类的指针
        if (m_cortana_wnd == nullptr) return;

        ::GetClientRect(m_hCortanaBar, m_cortana_rect);	//获取Cortana搜索框的矩形区域

        CRect cortana_rect;
        CRect cortana_static_rect;		//Cortana搜索框中static控件的矩形区域
        ::GetWindowRect(m_hCortanaBar, cortana_rect);
        ::GetWindowRect(m_hCortanaStatic, cortana_static_rect);	//获取Cortana搜索框中static控件的矩形区域

        const int min_conver_width = theApp.DPI(40);
        m_cover_width = cortana_static_rect.left - cortana_rect.left;
        if (m_cover_width < min_conver_width)
            m_cover_width = min_conver_width;

        m_pDC = m_cortana_wnd->GetDC();
        m_draw.Create(m_pDC, m_cortana_wnd);


        //获取用来检查小娜是否为深色模式的采样点的坐标
        //m_check_dark_point.x = cortana_rect.right - 2;
        //m_check_dark_point.y = cortana_rect.top + 2;

        CWinVersionHelper::CheckWindows10LightTheme();
        SetDarkMode(!CWinVersionHelper::IsWindows10LightTheme());

        //设置字体
        LOGFONT lf;
        SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(LOGFONT), &lf, 0);		//获取系统默认字体
        if (m_default_font.m_hObject)
            m_default_font.DeleteObject();
        m_default_font.CreatePointFont(110, lf.lfFaceName);

        InitFont();

        //为Cortana搜索框设置一个透明色，使Cortana搜索框不透明
#ifndef COMPILE_IN_WIN_XP
        //if(theApp.m_lyric_setting_data.cortana_opaque)
        //{
        m_cortana_opaque = false;
		SetCortanaBarOpaque(true);
        //}
#endif // !COMPILE_IN_WIN_XP

    }
}

void CCortanaLyric::InitFont()
{
    theApp.m_font_set.cortana.SetFont(theApp.m_lyric_setting_data.cortana_font);
    FontInfo translate_font = theApp.m_lyric_setting_data.cortana_font;
    translate_font.size--;
    theApp.m_font_set.cortana_translate.SetFont(translate_font);
}

void CCortanaLyric::SetEnable(bool enable)
{
    m_enable = enable;
}

void CCortanaLyric::DrawInfo()
{
    if (!m_enable)
        return;

    CSingleLock sync(&m_critical, TRUE);
    bool is_midi_lyric = CPlayerUIHelper::IsMidiLyric();

    //不使用兼容模式显示歌词，直接在小娜搜索框内绘图
    if(!theApp.m_lyric_setting_data.cortana_lyric_compatible_mode)
    {
        if (m_pDC != nullptr)
        {
            m_draw.SetFont(&theApp.m_font_set.cortana.GetFont());
            //双缓冲绘图
            CDrawDoubleBuffer drawDoubleBuffer(m_pDC, m_cortana_rect);
            //使用m_draw绘图
            m_draw.SetDC(drawDoubleBuffer.GetMemDC());
            m_draw.FillRect(m_cortana_rect, m_colors.back_color);

            if(theApp.m_lyric_setting_data.cortana_show_spectrum)
                DrawSpectrum();

            if (is_midi_lyric)
            {
                wstring current_lyric{ CPlayer::GetInstance().GetMidiLyric() };
                DrawCortanaTextSimple(current_lyric.c_str(), Alignment::LEFT);
            }
            else
            {
			    Time time{ CPlayer::GetInstance().GetCurrentPosition() };
			    int progress = CPlayer::GetInstance().m_Lyrics.GetLyricProgress(time);
			    CLyrics::Lyric lyric = CPlayer::GetInstance().m_Lyrics.GetLyric(time, 0);
                bool no_lyric{ false };
                //如果当前一句歌词为空，且持续了超过了20秒，则不显示歌词
                no_lyric = (lyric.text.empty() && CPlayer::GetInstance().GetCurrentPosition() - lyric.time.toInt() > 20000) || progress >= 1000;

                if (!CPlayer::GetInstance().m_Lyrics.IsEmpty() && !no_lyric && theApp.m_lyric_setting_data.cortana_show_lyric)		//有歌词时显示歌词
                {
				    if (m_draw.IsDrawMultiLine(m_cortana_rect.Height()))
				    {
					    m_draw.DrawLyricTextMultiLine(TextRect(), theApp.m_lyric_setting_data.cortana_lyric_align);
				    }
				    else
				    {
					    m_draw.DrawLyricTextSingleLine(TextRect(), theApp.m_lyric_setting_data.cortana_lyric_double_line, theApp.m_lyric_setting_data.cortana_lyric_align);
				    }
                }
                else			//没有歌词时在Cortana搜索框上以滚动的方式显示当前播放歌曲的文件名
                {
                    static int index{};
                    static wstring song_name{};
                    //如果当前播放的歌曲发生变化，DrawCortanaText函数的第2参数为true，即重置滚动位置
                    static CString str_now_playing{ CCommon::LoadText(IDS_NOW_PLAYING, _T(": ")) };
                    if (index != CPlayer::GetInstance().GetIndex() || song_name != CPlayer::GetInstance().GetFileName())
                    {
                        DrawCortanaText((str_now_playing + CPlayListCtrl::GetDisplayStr(CPlayer::GetInstance().GetCurrentSongInfo(), theApp.m_media_lib_setting_data.display_format).c_str()), true, GetScrollTextPixel());
                        index = CPlayer::GetInstance().GetIndex();
                        song_name = CPlayer::GetInstance().GetFileName();
                    }
                    else
                    {
                        DrawCortanaText((str_now_playing + CPlayListCtrl::GetDisplayStr(CPlayer::GetInstance().GetCurrentSongInfo(), theApp.m_media_lib_setting_data.display_format).c_str()), false, GetScrollTextPixel());
                    }
                }
            }

            //计算频谱，根据频谱幅值使Cortana图标显示动态效果
            float spectrum_avr{};		//取前面N个频段频谱值的平均值
            const int IMIN = FFT_SAMPLE / 20;
            const int IMAX = FFT_SAMPLE / 3;
            for (int i{ IMIN }; i < IMAX; i++)
                spectrum_avr += CPlayer::GetInstance().GetFFTData()[i];
            spectrum_avr /= (IMAX - IMIN);
            int spetraum = static_cast<int>(spectrum_avr * 22000);		//调整乘号后面的数值可以调整Cortana图标跳动时缩放的大小
            SetBeatAmp(spetraum);
            //显示专辑封面，如果没有专辑封面，则显示Cortana图标
            AlbumCoverEnable(theApp.m_lyric_setting_data.cortana_show_album_cover/* && CPlayer::GetInstance().AlbumCoverExist()*/);
            DrawAlbumCover(CPlayer::GetInstance().GetAlbumCover());

            //if (!m_colors.dark /*&& !theApp.m_lyric_setting_data.cortana_opaque*/)		//非深色模式下，在搜索顶部绘制边框
            //{
            //    CRect rect{ m_cortana_rect };
            //    rect.left += m_cover_width;
            //    m_draw.DrawRectTopFrame(rect, m_border_color);
            //}
            CDrawCommon::SetDrawArea(m_pDC, m_cortana_rect);
        }
    }

    //使用兼容模式显示歌词，给小娜搜索框设置文本
    else
    {
        CWnd* pWnd = CWnd::FromHandle(m_hCortanaStatic);
        if (pWnd != nullptr)
        {
            static wstring str_disp_last;
            wstring str_disp;
            if (is_midi_lyric)
            {
                str_disp = CPlayer::GetInstance().GetMidiLyric();
            }
            else if (!CPlayer::GetInstance().m_Lyrics.IsEmpty())		//有歌词时显示歌词
            {
                Time time{ CPlayer::GetInstance().GetCurrentPosition() };
                str_disp = CPlayer::GetInstance().m_Lyrics.GetLyric(time, 0).text;
                if (str_disp.empty())
                    str_disp = CCommon::LoadText(IDS_DEFAULT_LYRIC_TEXT);
            }
            else
            {
                //没有歌词时显示当前播放歌曲的名称
                str_disp = CCommon::LoadText(IDS_NOW_PLAYING, _T(": ")).GetString() + CPlayListCtrl::GetDisplayStr(CPlayer::GetInstance().GetCurrentSongInfo(), theApp.m_media_lib_setting_data.display_format);
            }

            if(str_disp != str_disp_last)
            {
                pWnd->SetWindowText(str_disp.c_str());
                pWnd->Invalidate();
                str_disp_last = str_disp;
            }
        }
    }
}

void CCortanaLyric::SetCortanaBarOpaque(bool opaque)
{
	if (opaque)
	{
		if(!m_cortana_opaque)
		{
			//设置不透明
			SetWindowLong(m_hCortanaBar, GWL_EXSTYLE, GetWindowLong(m_hCortanaBar, GWL_EXSTYLE) | WS_EX_LAYERED);
			::SetLayeredWindowAttributes(m_hCortanaBar, theApp.m_lyric_setting_data.cortana_transparent_color, 0, LWA_COLORKEY);
		}
	}
	else
	{
		if(m_cortana_opaque)
			//恢复不透明
			SetWindowLong(m_hCortanaBar, GWL_EXSTYLE, GetWindowLong(m_hCortanaBar, GWL_EXSTYLE) & ~WS_EX_LAYERED);
	}
	m_cortana_opaque = opaque;
}

void CCortanaLyric::DrawCortanaTextSimple(LPCTSTR str, Alignment align)
{
    if (m_enable && m_cortana_hwnd != NULL && m_cortana_wnd != nullptr)
    {
        CRect text_rect{ TextRect() };
        m_draw.DrawWindowText(text_rect, str, m_colors.info_text_color, align, false, false, true);
    }
}

void CCortanaLyric::DrawCortanaText(LPCTSTR str, bool reset, double scroll_pixel)
{
    if (m_enable && m_cortana_hwnd != NULL && m_cortana_wnd != nullptr)
    {
        static CDrawCommon::ScrollInfo cortana_scroll_info;
        CRect text_rect{ TextRect() };
        m_draw.DrawScrollText(text_rect, str, m_colors.info_text_color, scroll_pixel, false, cortana_scroll_info, reset);
    }
}

void CCortanaLyric::DrawCortanaText(LPCTSTR str, int progress)
{
    if (m_enable && m_cortana_hwnd != NULL && m_cortana_wnd != nullptr)
    {
        CRect text_rect{ TextRect() };
        if(theApp.m_lyric_setting_data.lyric_karaoke_disp)
            m_draw.DrawWindowText(text_rect, str, m_colors.text_color, m_colors.text_color2, progress);
        else
            m_draw.DrawWindowText(text_rect, str, m_colors.text_color, m_colors.text_color, progress);
    }
}

void CCortanaLyric::DrawAlbumCover(const CImage & album_cover)
{
    if (m_enable)
    {
        CRect cover_rect = CoverRect();
        m_draw.SetDrawArea(cover_rect);
        if (album_cover.IsNull() || !m_show_album_cover)
        {
            if (theApp.m_lyric_setting_data.show_default_album_icon_in_search_box)
            {
                CRect rc_icon{};
                const int icon_side = theApp.DPI(32);
                rc_icon.right = rc_icon.bottom = icon_side;
                rc_icon.MoveToX(cover_rect.left + (cover_rect.Width() - icon_side) / 2);
                rc_icon.MoveToY(cover_rect.top + (cover_rect.Height() - icon_side) / 2);
                HICON icon{ CPlayer::GetInstance().IsPlaying() ? theApp.m_icon_set.default_cover_small : theApp.m_icon_set.default_cover_small_not_played };
                m_draw.DrawIcon(icon, rc_icon.TopLeft(), rc_icon.Size());
            }
            else
            {
                int cortana_img_id{ m_colors.dark ? IDB_CORTANA_BLACK : IDB_CORTANA_WHITE };
                if (theApp.m_lyric_setting_data.cortana_icon_beat)
                {
                    m_draw.FillRect(cover_rect, m_colors.back_color);
                    CRect rect{ cover_rect };
                    rect.DeflateRect(theApp.DPI(4), theApp.DPI(4));
                    int inflate;
                    inflate = m_beat_amp * theApp.DPI(14) / 1000;
                    rect.InflateRect(inflate, inflate);
                    m_draw.DrawBitmap(cortana_img_id, rect.TopLeft(), rect.Size(), CDrawCommon::StretchMode::FIT);
                }
                else
                {
                    m_draw.DrawBitmap(cortana_img_id, cover_rect.TopLeft(), cover_rect.Size(), CDrawCommon::StretchMode::FIT);
                }
            }

            //if(!m_colors.dark)
            //    m_draw.DrawRectTopFrame(cover_rect, m_border_color);
        }
        else
        {
            m_draw.DrawBitmap(album_cover, cover_rect.TopLeft(), cover_rect.Size(), CDrawCommon::StretchMode::FILL);
        }
    }
}

void CCortanaLyric::DrawSpectrum()
{
    CRect rc_spectrum{ TextRect() };
	const int max_spectrum_height{ theApp.DPI(80) };		//频谱区域的最大高度
	if (rc_spectrum.Height() > max_spectrum_height)
		rc_spectrum.top = rc_spectrum.bottom - max_spectrum_height;

    m_draw.SetDrawArea(rc_spectrum);
    rc_spectrum.right += theApp.DPI(8);
    m_draw.DrawSpectrum(rc_spectrum, CUIDrawer::SC_64, false, theApp.m_app_setting_data.spectrum_low_freq_in_center);
    m_draw.SetDrawArea(m_cortana_rect);
}

CRect CCortanaLyric::TextRect() const
{
    CRect text_rect{ m_cortana_rect };
    text_rect.left += m_cover_width;
    text_rect.DeflateRect(theApp.DPI(4), theApp.DPI(2));
    text_rect.top -= theApp.DPI(1);
    return text_rect;
}

CRect CCortanaLyric::CoverRect() const
{
    CRect cover_rect = m_cortana_rect;
    cover_rect.right = cover_rect.left + m_cover_width;
    return cover_rect;
}

double CCortanaLyric::GetScrollTextPixel() const
{
    //界面刷新频率越高，即界面刷新时间间隔越小，则每次滚动的像素值就要越小
    double pixel = static_cast<double>(theApp.m_app_setting_data.ui_refresh_interval) *0.025 + 0.2;
    pixel = static_cast<double>(theApp.GetDPI()) * pixel / 96;
    if (pixel < 0.1)
        pixel = 0.1;
    if (pixel > 0.5 && pixel < 1)
        pixel = 1;
    return pixel;
}

void CCortanaLyric::ResetCortanaText()
{
    if (m_enable && m_cortana_hwnd != NULL && m_cortana_wnd != nullptr)
    {
        CSingleLock sync(&m_critical, TRUE);
        CWnd* pWnd = CWnd::FromHandle(m_hCortanaStatic);
        if (pWnd != nullptr)
        {
            CString str;
            pWnd->GetWindowText(str);
            if (str != m_cortana_default_text.c_str())
            {
                pWnd->SetWindowText(m_cortana_default_text.c_str());
                pWnd->Invalidate();
            }
        }

        if (!theApp.m_lyric_setting_data.cortana_lyric_compatible_mode && m_pDC != nullptr)
        {
            m_draw.SetFont(&m_default_font);
            COLORREF color;		//Cortana默认文本的颜色
            color = (m_dark_mode ? GRAY(173) : GRAY(16));
            m_draw.SetDC(m_pDC);
            //先绘制Cortana图标
            CRect cover_rect = CoverRect();
            if (m_dark_mode)
                m_draw.DrawBitmap(IDB_CORTANA_BLACK, cover_rect.TopLeft(), cover_rect.Size(), CDrawCommon::StretchMode::FIT);
            else
                m_draw.DrawBitmap(IDB_CORTANA_WHITE, cover_rect.TopLeft(), cover_rect.Size(), CDrawCommon::StretchMode::FIT);
            //再绘制Cortana默认文本
            CRect rect{ m_cortana_rect };
            rect.left += m_cover_width;
            m_draw.FillRect(rect, m_colors.back_color);
            m_draw.DrawWindowText(rect, m_cortana_default_text.c_str(), color);
            //if (!m_dark_mode)
            //{
            //    rect.left -= m_cover_width;
            //    m_draw.DrawRectTopFrame(rect, m_border_color);
            //}
            //m_cortana_wnd->Invalidate();
        }
    }
}

void CCortanaLyric::SetDarkMode(bool dark_mode)
{
    if (m_enable)
    {
		m_dark_mode = dark_mode;
		SetCortanaBarOpaque(!m_dark_mode);
		SetUIColors();
    }
}

void CCortanaLyric::ApplySearchBoxTransparentChanged()
{
    if (CWinVersionHelper::IsWindows10LightTheme())
    {
        m_cortana_opaque = false;
        SetCortanaBarOpaque(true);
    }
}

void CCortanaLyric::AlbumCoverEnable(bool enable)
{
    CSingleLock sync(&m_critical, TRUE);
    bool last_enable;
    last_enable = m_show_album_cover;
    m_show_album_cover = enable;
    if (last_enable && !enable && m_pDC != nullptr)
    {
        CRect cover_rect = CoverRect();
        CDrawCommon::SetDrawArea(m_pDC, cover_rect);
        m_pDC->FillSolidRect(cover_rect, m_colors.back_color);
    }
}

void CCortanaLyric::SetBeatAmp(int beat_amp)
{
    m_beat_amp = beat_amp;
    if (m_beat_amp < 0) m_beat_amp = 0;
    if (m_beat_amp > 2000) m_beat_amp = 2000;
}

void CCortanaLyric::SetUIColors()
{
    if (theApp.m_lyric_setting_data.cortana_color == 1)
        m_colors.dark = true;
    else if (theApp.m_lyric_setting_data.cortana_color == 2)
        m_colors.dark = false;
    else
        m_colors.dark = m_dark_mode;

	m_lyric_colors = CPlayerUIHelper::GetUIColors(theApp.m_app_setting_data.theme_color, m_colors.dark);

	if (m_colors.dark)
    {
        m_colors.text_color = theApp.m_app_setting_data.theme_color.light3;
        m_colors.text_color2 = theApp.m_app_setting_data.theme_color.light1;
        m_colors.info_text_color = theApp.m_app_setting_data.theme_color.light3;
        m_lyric_colors.color_spectrum = theApp.m_app_setting_data.theme_color.dark0;

        DWORD dwStyle = GetWindowLong(m_hCortanaStatic, GWL_STYLE);
        if ((dwStyle & WS_VISIBLE) != 0)		//根据Cortana搜索框中static控件是否有WS_VISIBLE属性为绘图背景设置不同的背景色
            m_colors.back_color = GRAY(47);	//设置绘图的背景颜色
        else
            m_colors.back_color = GRAY(10);	//设置绘图的背景颜色
    }
    else
    {
        m_colors.text_color = theApp.m_app_setting_data.theme_color.dark3;
        m_colors.text_color2 = theApp.m_app_setting_data.theme_color.dark1;
        m_colors.info_text_color = theApp.m_app_setting_data.theme_color.dark2;
        m_colors.back_color = LIGHT_MODE_SEARCH_BOX_BACKGROUND_COLOR;
        m_lyric_colors.color_spectrum = theApp.m_app_setting_data.theme_color.dark0;

		m_lyric_colors.color_text = theApp.m_app_setting_data.theme_color.dark3;
		m_lyric_colors.color_text_2 = theApp.m_app_setting_data.theme_color.dark1;
    }
}
