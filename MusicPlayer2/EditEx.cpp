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


BEGIN_MESSAGE_MAP(CEditEx, CEdit)
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
