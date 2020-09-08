// EditEx.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "EditEx.h"


// CEditEx

IMPLEMENT_DYNAMIC(CEditEx, CEdit)

CEditEx::CEditEx()
{

}

CEditEx::~CEditEx()
{
}


void CEditEx::ResetModified()
{
    SetModify(FALSE);
    Invalidate(FALSE);
}

CString CEditEx::GetText() const
{
    CString str;
    GetWindowText(str);
    return str;
}

void CEditEx::SetColorWhenModified(bool val)
{
    m_show_color_when_modified = val;
}

BEGIN_MESSAGE_MAP(CEditEx, CEdit)
    ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()



// CEditEx 消息处理程序




BOOL CEditEx::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	//按Ctrl+A全选
	if ((GetKeyState(VK_CONTROL) & 0x80) && (pMsg->wParam == 'A'))
	{
		SetSel(0, -1);
		return TRUE;
	}
	return CEdit::PreTranslateMessage(pMsg);
}


HBRUSH CEditEx::CtlColor(CDC* pDC, UINT nCtlColor)
{
    // TODO:  在此更改 DC 的任何特性

    DWORD style = GetStyle();
    bool is_read_only = ((style & ES_READONLY) != 0);
    if (m_show_color_when_modified && GetModify() && !is_read_only)
        pDC->SetTextColor(theApp.m_app_setting_data.theme_color.dark1);        //如果文本已修改，则显示为主题色

    // TODO:  如果不应调用父级的处理程序，则返回非 null 画笔
    static HBRUSH hBackBrush{ CreateSolidBrush(GetSysColor(COLOR_WINDOW)) };
    return (is_read_only ? NULL : hBackBrush);
}
