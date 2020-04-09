#pragma once
#include "ColorConvert.h"

// CPlayerProgressBar

class CPlayerProgressBar : public CStatic
{
	DECLARE_DYNAMIC(CPlayerProgressBar)

public:
	CPlayerProgressBar();
	virtual ~CPlayerProgressBar();

    void SetProgress(int progress);
    void SetBackgroundColor(COLORREF back_color);
    void SetBarCount(int bar_cnt);

protected:
    int m_progress;       //当前进度（百分比）
    const ColorTable& m_theme_color;
    COLORREF m_back_color{ RGB(255, 255, 255) };
    int m_bar_count{ 10 };

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
};


