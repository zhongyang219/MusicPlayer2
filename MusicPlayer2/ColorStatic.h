#pragma once


// CColorStatic

class CColorStatic : public CStatic
{
	DECLARE_DYNAMIC(CColorStatic)

public:
	CColorStatic();
	virtual ~CColorStatic();

	void SetFillColor(COLORREF fill_color);		//设置要填充的背景色

protected:
	COLORREF m_fill_color{ RGB(255, 255,255) };

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};


