#pragma once
#include "LyricsWindow.h"
#include "CommonData.h"

struct LyricStyle
{
    COLORREF color1 = 0;
    COLORREF color2 = 0;
    int gradient_mode = 0;
};

struct LyricStyleDefaultData        //桌面歌词预设数据
{
    LyricStyle normal_style;
    LyricStyle highlight_style;
};
const int LYRIC_DEFAULT_STYLE_NUM = 3;

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
    void SetLyricBackgroundPenetrate(bool penetrate);
    HWND GetLyricWnd() const;
    LyricStyleDefaultData GetDefaultStyle(int index);

	static int ToGDIPluseFontStyle(const FontStyle& style);
	static Gdiplus::Color ToGDIPluseColor(COLORREF color);

private:
	CLyricsWindow m_lyric_window;
    LyricStyleDefaultData m_default_style[LYRIC_DEFAULT_STYLE_NUM];

};

