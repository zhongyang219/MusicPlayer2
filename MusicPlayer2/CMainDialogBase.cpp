#include "stdafx.h"
#include "CMainDialogBase.h"
#include "Common.h"
#include "MusicPlayer2.h"

IMPLEMENT_DYNAMIC(CMainDialogBase, CDialog)

CMainDialogBase::CMainDialogBase(UINT nIDTemplate, CWnd * pParent)
	: CDialog(nIDTemplate, pParent)
{
}


CMainDialogBase::~CMainDialogBase()
{
}

void CMainDialogBase::SetFullScreen(bool full_screen)
{
	m_bFullScreen = full_screen;
	if (full_screen)
	{
		// 获取主窗口所在监视器句柄，如果窗口不在任何监视器则返回主监视器句柄
		HMONITOR hMonitor = MonitorFromWindow(theApp.m_pMainWnd->GetSafeHwnd(), MONITOR_DEFAULTTOPRIMARY);

		// 获取监视器信息
		MONITORINFO lpmi;
		lpmi.cbSize = sizeof(lpmi);
		GetMonitorInfo(hMonitor, &lpmi);

		// 用m_struOldWndpl得到当前窗口的显示状态和窗体位置，以供退出全屏后使用
		GetWindowPlacement(&m_struOldWndpl);

		CRect monitor{ lpmi.rcMonitor };
		POINT offset{ lpmi.rcWork.left - lpmi.rcMonitor.left, lpmi.rcWork.top - lpmi.rcMonitor.top };
		// 设置窗口对象参数，为全屏做好准备并进入全屏状态
		WINDOWPLACEMENT struWndpl;
		struWndpl.length = sizeof(WINDOWPLACEMENT);
		struWndpl.flags = 0;
		struWndpl.showCmd = SW_SHOWNORMAL;
		struWndpl.rcNormalPosition = monitor - offset;
		SetWindowPlacement(&struWndpl);//该函数设置指定窗口的显示状态和显示大小位置等，是我们该程序最为重要的函数
	}
	else
	{
		SetWindowPlacement(&m_struOldWndpl);
	}
}

void CMainDialogBase::ShowTitlebar(bool show)
{
	if (show)
	{
        ModifyStyle(0, WS_CAPTION, SWP_FRAMECHANGED);		//还原标题栏
	}
	else
	{
		ModifyStyle(WS_CAPTION, 0, SWP_FRAMECHANGED);		//去掉标题栏
	}
}

void CMainDialogBase::ShowSizebox(bool show)
{
    if (show)
    {
        ModifyStyle(0, WS_SIZEBOX, SWP_FRAMECHANGED);        // 还原大小边框
    }
    else
    {
        ModifyStyle(WS_SIZEBOX, 0, SWP_FRAMECHANGED);        // 去掉大小边框
    }
}

BEGIN_MESSAGE_MAP(CMainDialogBase, CDialog)
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()


void CMainDialogBase::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialog::OnGetMinMaxInfo(lpMMI);
}
