#pragma once
#include <afxwin.h>
class CStaticEx :
	public CStatic
{
public:
	CStaticEx();
	~CStaticEx();

	void SetTextColor(COLORREF text_color);
	void SetBackgroundColor(COLORREF back_color);
    void SetWindowText(LPCTSTR lpszString);
    CString GetWindowText() const;

private:
	COLORREF m_text_color{ GRAY(0) };
	COLORREF m_back_color{ GRAY(255) };
	bool m_transparent{ true };
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
};

