#include "stdafx.h"
#include "StaticEx.h"
#include "DrawCommon.h"
#include "MusicPlayer2.h"


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

CString CStaticEx::GetWindowText() const
{
    CString str;
    CStatic::GetWindowText(str);
    return str;
}

void CStaticEx::SetIcon(HICON hIcon, CSize size)
{
	m_hIcon = hIcon;
	m_icon_size = size;
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
	CStatic::GetWindowText(str);
	//绘制背景
	if(!m_transparent)
		draw.FillRect(rect, m_back_color);
	else
		DrawThemeParentBackground(m_hWnd, dc.GetSafeHdc(), &rect);	//重绘控件区域以解决文字重叠的问题

	CRect rc_text = rect;

	//绘制图标
	bool draw_icon = (m_hIcon != NULL && m_icon_size.cx > 0 && m_icon_size.cy > 0);
	if (draw_icon)
	{
		CRect rc_tmp = rect;
		rc_tmp.right = rc_tmp.left + m_icon_size.cx + theApp.DPI(4);
		rc_text.left = rc_tmp.right;
		CPoint icon_left_top;
		icon_left_top.x = rc_tmp.left + (rc_tmp.Width() - m_icon_size.cx) / 2;
		icon_left_top.y = rc_tmp.top + (rc_tmp.Height() - m_icon_size.cy) / 2;
		draw.SetDrawArea(rc_tmp);
		draw.DrawIcon(m_hIcon, icon_left_top, m_icon_size);
	}

	//绘制文本
	draw.DrawWindowText(rc_text, str, m_text_color);
}
