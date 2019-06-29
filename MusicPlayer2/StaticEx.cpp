#include "stdafx.h"
#include "StaticEx.h"
#include "DrawCommon.h"


CStaticEx::CStaticEx()
{
}


CStaticEx::~CStaticEx()
{
}

void CStaticEx::SetTextColor(COLORREF text_color)
{
	m_text_color = text_color;
}

void CStaticEx::SetBackgroundColor(COLORREF back_color)
{
	m_back_color = back_color;
	m_transparent = false;
}

void CStaticEx::SetWindowText(LPCTSTR lpszString)
{
    CWnd::SetWindowText(lpszString);
    Invalidate();
}

BEGIN_MESSAGE_MAP(CStaticEx, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()


void CStaticEx::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CStatic::OnPaint()

	CDrawCommon draw;
	draw.Create(&dc, this);

	CRect rect;
	GetClientRect(rect);
	CString str;
	GetWindowText(str);
	if(!m_transparent)
		draw.FillRect(rect, m_back_color);
	else
		DrawThemeParentBackground(m_hWnd, dc.GetSafeHdc(), &rect);	//重绘控件区域以解决文字重叠的问题
	draw.DrawWindowText(rect, str, m_text_color);
}
