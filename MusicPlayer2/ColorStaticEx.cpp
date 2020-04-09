#include "stdafx.h"
#include "ColorStaticEx.h"


CColorStaticEx::CColorStaticEx()
{
}


CColorStaticEx::~CColorStaticEx()
{
}


COLORREF CColorStaticEx::GetFillColor() const
{
	return m_fill_color;
}

void CColorStaticEx::PreSubclassWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	DWORD dwStyle = GetStyle();
	::SetWindowLong(GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);

	CColorStatic::PreSubclassWindow();
}
BEGIN_MESSAGE_MAP(CColorStaticEx, CColorStatic)
//	ON_CONTROL_REFLECT(STN_CLICKED, &CColorStaticEx::OnStnClicked)
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


//void CColorStaticEx::OnStnClicked()
//{
//	// TODO: 在此添加控件通知处理程序代码
//}


void CColorStaticEx::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CColorDialog color_dlg(m_fill_color);
	if (color_dlg.DoModal() == IDOK)
	{
		SetFillColor(color_dlg.GetColor());

        //向父窗口发送WM_COLOR_SELECTED消息
        CWnd* pParent = GetParent();
        if(pParent!=nullptr)
            ::SendMessage(pParent->GetSafeHwnd(), WM_COLOR_SELECTED, (WPARAM)this, 0);
	}

	CColorStatic::OnLButtonUp(nFlags, point);
}
