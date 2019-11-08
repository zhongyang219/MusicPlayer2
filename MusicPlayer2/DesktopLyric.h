#pragma once
#include "LyricsWindow.h"
#include "CommonData.h"

struct LyricStyleDefaultData        //桌面歌词预设数据
{
    COLORREF color1 = 0;
    COLORREF color2 = 0;
    int gradient_mode = 0;
};

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
    void SetLyricOpacity(int opacity);
    HWND GetLyricWnd() const;

	static int ToGDIPluseFontStyle(const FontStyle& style);
	static Gdiplus::Color ToGDIPluseColor(COLORREF color);

private:
	CLyricsWindow m_lyric_window;


};

