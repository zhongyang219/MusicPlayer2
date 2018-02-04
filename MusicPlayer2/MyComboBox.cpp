#include "stdafx.h"
#include "MyComboBox.h"
#include "MusicPlayer2.h"

IMPLEMENT_DYNAMIC(CMyComboBox, CComboBox)

CMyComboBox::CMyComboBox()
{
}


CMyComboBox::~CMyComboBox()
{
}

void CMyComboBox::SetReadOnly(bool read_only)
{
	//((CEdit*)GetWindow(GW_CHILD))->SetReadOnly(read_only);		//将Endit控件设为只读
	m_read_only = read_only;
}

void CMyComboBox::SetEditReadOnly(bool read_only)
{
	((CEdit*)GetWindow(GW_CHILD))->SetReadOnly(read_only);		//将Endit控件设为只读
}

BEGIN_MESSAGE_MAP(CMyComboBox, CComboBox)
END_MESSAGE_MAP()



BOOL CMyComboBox::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (m_read_only)
	{
		if (pMsg->message == WM_MOUSEWHEEL)		//如果只读，则不响应鼠标滚轮消息
			return TRUE;
		if (pMsg->message == WM_LBUTTONDOWN || pMsg->message == WM_LBUTTONDBLCLK || pMsg->message == WM_MOUSEMOVE || pMsg->message == WM_LBUTTONUP)
		{
			CPoint point1 = pMsg->pt;
			CPoint point;
			GetCursorPos(&point);
			//获取箭头区域的位置
			GetWindowRect(m_arrow_rect);
			m_arrow_rect.left = m_arrow_rect.right - DPI(18);
			if(m_arrow_rect.PtInRect(point))		//如果鼠标指针的位置位于箭头区域，则不响应以上的鼠标消息
				return TRUE;
		}
	}

	return CComboBox::PreTranslateMessage(pMsg);
}
