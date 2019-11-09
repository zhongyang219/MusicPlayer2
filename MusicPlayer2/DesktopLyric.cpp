#include "stdafx.h"
#include "DesktopLyric.h"
#include "MusicPlayer2.h"
#include "PlayListCtrl.h"

CDesktopLyric::CDesktopLyric()
{
}


CDesktopLyric::~CDesktopLyric()
{
}

void CDesktopLyric::Create()
{
	m_lyric_window.Create(theApp.DPI(150));
}

void CDesktopLyric::ShowLyric()
{
	if (!m_lyric_window.IsWindowVisible())
		return;

	if(!CPlayer::GetInstance().m_Lyrics.IsEmpty())
	{
		Time time{ CPlayer::GetInstance().GetCurrentPosition() };
		int progress = CPlayer::GetInstance().m_Lyrics.GetLyricProgress(time);
		CLyrics::Lyric lyric = CPlayer::GetInstance().m_Lyrics.GetLyric(time, 0);
		if (lyric.text.empty())
			lyric.text = CCommon::LoadText(IDS_DEFAULT_LYRIC_TEXT_CORTANA);

        m_lyric_window.SetLyricDoubleLine(theApp.m_lyric_setting_data.desktop_lyric_data.lyric_double_line);
        m_lyric_window.SetShowTranslate(theApp.m_ui_data.show_translate);
        if(theApp.m_lyric_setting_data.desktop_lyric_data.lyric_double_line)
        {
            CLyrics::Lyric next_lyric = CPlayer::GetInstance().m_Lyrics.GetLyric(time, 1);
            if (next_lyric.text.empty())
                next_lyric.text = CCommon::LoadText(IDS_DEFAULT_LYRIC_TEXT_CORTANA);
            m_lyric_window.SetNextLyric(next_lyric.text.c_str());
        }

        int lyric_index = CPlayer::GetInstance().m_Lyrics.GetLyricIndex(time);
        static int last_lyric_index = -1;

		if (lyric_index != last_lyric_index)
		{
            m_lyric_window.SetLyricChangeFlag(true);
			m_lyric_window.UpdateLyricTranslate(lyric.translate.c_str());
			m_lyric_window.UpdateLyrics(lyric.text.c_str(), progress);
            last_lyric_index = lyric_index;
		}
		else
		{
            m_lyric_window.SetLyricChangeFlag(false);
            m_lyric_window.UpdateLyrics(progress);
		}
	}
	else
	{
		const SongInfo& cur_song_info = CPlayer::GetInstance().GetCurrentSongInfo();
		std::wstring display_text = CPlayListCtrl::GetDisplayStr(cur_song_info, DF_ARTIST_TITLE);
		if (display_text != m_lyric_window.GetLyricStr().GetString())
		{
			m_lyric_window.UpdateLyrics(display_text.c_str(), 0);
		}
		else
		{
			m_lyric_window.UpdateLyrics(0);
		}
        m_lyric_window.UpdateLyricTranslate(_T(""));
        m_lyric_window.SetNextLyric(_T(""));
	}

}

void CDesktopLyric::ClearLyric()
{
	m_lyric_window.UpdateLyricTranslate(_T(""));
	m_lyric_window.UpdateLyrics(_T(""), 0);
    m_lyric_window.SetNextLyric(_T(""));
}

void CDesktopLyric::ApplySettings(const DesktopLyricSettingData& data)
{
	m_lyric_window.SetLyricsFont(data.lyric_font.name.c_str(), theApp.DPI(data.lyric_font.size), ToGDIPluseFontStyle(data.lyric_font.style));
	m_lyric_window.SetLyricsColor(ToGDIPluseColor(data.text_color1), ToGDIPluseColor(data.text_color2), static_cast<LyricsGradientMode>(data.text_gradient));
	m_lyric_window.SetHighlightColor(ToGDIPluseColor(data.highlight_color1), ToGDIPluseColor(data.highlight_color2), static_cast<LyricsGradientMode>(data.highlight_gradient));
	SetLyricWindowLock(data.lock_desktop_lyric);
}

void CDesktopLyric::SetLyricWindowVisible(bool visible)
{
	m_lyric_window.ShowWindow(visible);
}

void CDesktopLyric::SetLyricWindowLock(bool locked)
{
	if (locked)
	{
		SetWindowLong(m_lyric_window.GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong(m_lyric_window.GetSafeHwnd(), GWL_EXSTYLE) | WS_EX_TRANSPARENT);		//设置鼠标穿透
	}
	else
	{
		SetWindowLong(m_lyric_window.GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong(m_lyric_window.GetSafeHwnd(), GWL_EXSTYLE) & (~WS_EX_TRANSPARENT));		//取消鼠标穿透
	}
	m_lyric_window.SetDrawBackground(!locked);
}

void CDesktopLyric::SetLyricOpacity(int opacity)
{
    m_lyric_window.SetAlpha(opacity * 255 / 100);
}

HWND CDesktopLyric::GetLyricWnd() const
{
    return m_lyric_window.GetSafeHwnd();
}

int CDesktopLyric::ToGDIPluseFontStyle(const FontStyle& style)
{
	int value = 0;
	if (style.bold)
		value |= Gdiplus::FontStyle::FontStyleBold;
	if (style.italic)
		value |= Gdiplus::FontStyle::FontStyleItalic;
	if (style.underline)
		value |= Gdiplus::FontStyle::FontStyleUnderline;
	if (style.strike_out)
		value |= Gdiplus::FontStyle::FontStyleStrikeout;
	return value;

}

Gdiplus::Color CDesktopLyric::ToGDIPluseColor(COLORREF color)
{
	Gdiplus::Color gdiplus_color;
	gdiplus_color.SetFromCOLORREF(color);
	return gdiplus_color;
}
