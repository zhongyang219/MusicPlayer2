#include "stdafx.h"
#include "DesktopLyric.h"
#include "MusicPlayer2.h"

CDesktopLyric::CDesktopLyric()
{
}


CDesktopLyric::~CDesktopLyric()
{
}

void CDesktopLyric::Create()
{
	m_lyric_window.Create();
}

void CDesktopLyric::ShowLyric()
{
	Time time{ CPlayer::GetInstance().GetCurrentPosition() };
	int progress = CPlayer::GetInstance().m_Lyrics.GetLyricProgress(time);
	CLyrics::Lyric lyric = CPlayer::GetInstance().m_Lyrics.GetLyric(time, 0);
	static std::wstring last_lyric_str;
	if (!lyric.text.empty())
	{
		if (last_lyric_str != lyric.text)
		{
			m_lyric_window.UpdateLyrics(lyric.text.c_str(), progress);
			last_lyric_str = lyric.text;
		}
		else
		{
			m_lyric_window.UpdateLyrics(progress);
		}
	}

}

void CDesktopLyric::ApplySettings(const DesktopLyricSettingData& data)
{
	m_lyric_window.SetLyricsFont(data.lyric_font.name.c_str(), theApp.DPI(data.lyric_font.size), ToGDIPluseFontStyle(data.lyric_font.style));
	m_lyric_window.SetLyricsColor(ToGDIPluseColor(data.text_color1), ToGDIPluseColor(data.text_color2), static_cast<LyricsGradientMode>(data.text_gradient));
	m_lyric_window.SetHighlightColor(ToGDIPluseColor(data.highlight_color1), ToGDIPluseColor(data.highlight_color2), static_cast<LyricsGradientMode>(data.highlight_gradient));
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
