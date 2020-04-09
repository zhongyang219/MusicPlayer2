#pragma once
#include "ColorStatic.h"
class CColorStaticEx :
	public CColorStatic
{
public:
	CColorStaticEx();
	~CColorStaticEx();
	COLORREF GetFillColor() const;

	virtual void PreSubclassWindow();
	DECLARE_MESSAGE_MAP()
//	afx_msg void OnStnClicked();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};

