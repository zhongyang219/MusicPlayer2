#pragma once
#include "LyricsWindow.h"
#include "CommonData.h"
class CDesktopLyric
{
public:
	CDesktopLyric();
	~CDesktopLyric();

	void Create();
	void ShowLyric();
	void ApplySettings(const DesktopLyricSettingData& data);
	void SetLyricWindowVisible(bool visible);

	static int ToGDIPluseFontStyle(const FontStyle& style);
	static Gdiplus::Color ToGDIPluseColor(COLORREF color);

private:
	CLyricsWindow m_lyric_window;
};

