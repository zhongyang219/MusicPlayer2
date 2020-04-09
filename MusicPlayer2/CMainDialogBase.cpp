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
		//获取系统屏幕宽高  
		int g_iCurScreenWidth = GetSystemMetrics(SM_CXSCREEN);
		int g_iCurScreenHeight = GetSystemMetrics(SM_CYSCREEN);

		//当前菜单栏的高度减去一行菜单栏的高度
		int height_comp = 0;
		if(theApp.m_ui_data.show_menu_bar)
			height_comp = CCommon::GetMenuBarHeight(m_hWnd) - theApp.DPIRound(19);


		//用m_struOldWndpl得到当前窗口的显示状态和窗体位置，以供退出全屏后使用  
		GetWindowPlacement(&m_struOldWndpl);

		//计算出窗口全屏显示客户端所应该设置的窗口大小，主要为了将不需要显示的窗体边框等部分排除在屏幕外  
		CRect rectWholeDlg;
		CRect rectClient;
		GetWindowRect(&rectWholeDlg);//得到当前窗体的总的相对于屏幕的坐标  
		RepositionBars(0, 0xffff, AFX_IDW_PANE_FIRST, reposQuery, &rectClient);//得到窗口客户区坐标  
		ClientToScreen(&rectClient);//将客户区相对窗体的坐标转为相对屏幕坐标  
		m_rectFullScreen.left = rectWholeDlg.left - rectClient.left;
		m_rectFullScreen.top = rectWholeDlg.top - rectClient.top + height_comp;
		m_rectFullScreen.right = rectWholeDlg.right + g_iCurScreenWidth - rectClient.right;
		m_rectFullScreen.bottom = rectWholeDlg.bottom + g_iCurScreenHeight - rectClient.bottom;

		//设置窗口对象参数，为全屏做好准备并进入全屏状态  
		WINDOWPLACEMENT struWndpl;
		struWndpl.length = sizeof(WINDOWPLACEMENT);
		struWndpl.flags = 0;
		struWndpl.showCmd = SW_SHOWNORMAL;
		struWndpl.rcNormalPosition = m_rectFullScreen;
		SetWindowPlacement(&struWndpl);//该函数设置指定窗口的显示状态和显示大小位置等，是我们该程序最为重要的函数  
	}
	else
	{
		SetWindowPlacement(&m_struOldWndpl);
	}
}
BEGIN_MESSAGE_MAP(CMainDialogBase, CDialog)
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()


void CMainDialogBase::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_bFullScreen)
	{
		lpMMI->ptMaxSize.x = m_rectFullScreen.Width();
		lpMMI->ptMaxSize.y = m_rectFullScreen.Height();
		lpMMI->ptMaxPosition.x = m_rectFullScreen.left;
		lpMMI->ptMaxPosition.y = m_rectFullScreen.top;
		lpMMI->ptMaxTrackSize.x = m_rectFullScreen.Width();
		lpMMI->ptMaxTrackSize.y = m_rectFullScreen.Height();
	}

	CDialog::OnGetMinMaxInfo(lpMMI);
}
