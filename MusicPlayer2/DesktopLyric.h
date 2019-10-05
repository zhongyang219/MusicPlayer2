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
	void ClearLyric();
	void ApplySettings(const DesktopLyricSettingData& data);
	void SetLyricWindowVisible(bool visible);
	void SetLyricWindowLock(bool locked);

	static int ToGDIPluseFontStyle(const FontStyle& style);
	static Gdiplus::Color ToGDIPluseColor(COLORREF color);

private:
	CLyricsWindow m_lyric_window;
};

