#include "stdafx.h"
#include "CNotifyIcon.h"
#include "MusicPlayer2.h"

CNotifyIcon::CNotifyIcon()
{
}


CNotifyIcon::~CNotifyIcon()
{
}

void CNotifyIcon::Init(HICON hIcon)
{
	m_ntIcon.cbSize = sizeof(NOTIFYICONDATA);	//该结构体变量的大小
	m_ntIcon.hIcon = hIcon;						//设置图标
	m_ntIcon.hWnd = theApp.m_pMainWnd->GetSafeHwnd();				//接收托盘图标通知消息的窗口句柄
	CCommon::WStringCopy(m_ntIcon.szTip, 128, APP_NAME);
	m_ntIcon.uCallbackMessage = MY_WM_NOTIFYICON;			//应用程序定义的消息ID号
	m_ntIcon.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;		//图标的属性：设置成员uCallbackMessage、hIcon、szTip有效
}

void CNotifyIcon::SetIcon(HICON hIcon)
{
    m_ntIcon.hIcon = hIcon;
}

void CNotifyIcon::AddNotifyIcon()
{
	::Shell_NotifyIcon(NIM_ADD, &m_ntIcon);
}

void CNotifyIcon::DeleteNotifyIcon()
{
	::Shell_NotifyIcon(NIM_DELETE, &m_ntIcon);
}

void CNotifyIcon::SetIconToolTip(LPCTSTR strTip)
{
	if (m_tool_tip_str != strTip)
	{
		CCommon::WStringCopy(m_ntIcon.szTip, 128, strTip);
		::Shell_NotifyIcon(NIM_MODIFY, &m_ntIcon);
	}
	m_tool_tip_str = strTip;
}

void CNotifyIcon::OnNotifyIcon(UINT msgId, HWND hMiniMode)
{
	if (msgId == WM_LBUTTONUP)
	{
		if (hMiniMode != NULL)
		{
			::ShowWindow(hMiniMode, SW_RESTORE);
			::SetForegroundWindow(hMiniMode);
		}
		else
		{
			theApp.m_pMainWnd->ShowWindow(SW_RESTORE);
			theApp.m_pMainWnd->SetForegroundWindow();
		}

	}
	if (msgId == WM_RBUTTONUP)
	{
		theApp.m_pMainWnd->SetForegroundWindow();

		//在通知区点击右键弹出右键菜单
		CPoint point;
		GetCursorPos(&point);	//获取当前光标的位置
		theApp.m_menu_set.m_notify_menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, theApp.m_pMainWnd); //在指定位置显示弹出菜单
	}
	if (msgId == WM_LBUTTONDBLCLK)
	{
	}
}
