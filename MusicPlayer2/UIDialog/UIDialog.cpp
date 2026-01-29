// UIDialog.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "UIDialog.h"
#include <dwmapi.h>
#include "WinVersionHelper.h"

// CUIDialog 对话框

IMPLEMENT_DYNAMIC(CUIDialog, CDialog)

CUIDialog::CUIDialog(UINT ui_res_id, CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_UI_DIALOG, pParent),
	m_ui(this, ui_res_id, m_ui_data)
{
	m_ui_data.enable_background = false;
	m_ui_data.show_playlist = false;
	m_ui_data.show_menu_bar = false;
	m_ui_data.enable_titlebar = false;
	m_ui_data.enable_statusbar = false;
}

CUIDialog::~CUIDialog()
{
}

void CUIDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CUIDialog, CDialog)
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEHWHEEL()
	ON_WM_MOUSELEAVE()
	ON_WM_SIZE()
	ON_WM_RBUTTONDOWN()
	ON_WM_DESTROY()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()


// CUIDialog 消息处理程序


BOOL CUIDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	//深色模式下，为对话框启用深色标题栏
	if (theApp.m_app_setting_data.dark_mode && CWinVersionHelper::IsWindows10Version1809OrLater())
	{
		BOOL darkMode = TRUE;
		DwmSetWindowAttribute(m_hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &darkMode, sizeof(darkMode));
	}

	m_pDC = GetDC();
	m_ui.Init(m_pDC);

	SetWindowText(m_ui.GetUIName().c_str());
	SetIcon(theApp.m_icon_mgr.GetHICON(IconMgr::IT_App, IconMgr::IS_Auto, IconMgr::IS_DPI_16), FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


BOOL CUIDialog::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_MOUSEMOVE)
	{
		m_ui.GetToolTipCtrl().RelayEvent(pMsg);
	}

	return CDialog::PreTranslateMessage(pMsg);
}


void CUIDialog::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	m_ui.DrawInfo();
}


void CUIDialog::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_ui.LButtonUp(point);
	Invalidate(FALSE);
	CDialog::OnLButtonUp(nFlags, point);
}


void CUIDialog::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_ui.LButtonDown(point);
	Invalidate(FALSE);
	CDialog::OnLButtonDown(nFlags, point);
}


void CUIDialog::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	m_ui.DoubleClick(point);
	CDialog::OnLButtonDblClk(nFlags, point);
}


void CUIDialog::OnRButtonUp(UINT nFlags, CPoint point)
{
	m_ui.RButtonUp(point);
	CDialog::OnRButtonUp(nFlags, point);
}


void CUIDialog::OnRButtonDown(UINT nFlags, CPoint point)
{
	m_ui.RButtonDown(point);
	CDialog::OnRButtonDown(nFlags, point);
}


void CUIDialog::OnMouseMove(UINT nFlags, CPoint point)
{
	m_ui.MouseMove(point);
	Invalidate(FALSE);
	CDialog::OnMouseMove(nFlags, point);
}


BOOL CUIDialog::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	CPoint point = pt;
	ScreenToClient(&point);
	m_ui.MouseWheel(zDelta, point);
	Invalidate(FALSE);
	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}


void CUIDialog::OnMouseLeave()
{
	m_ui.MouseLeave();
	Invalidate(FALSE);
	CDialog::OnMouseLeave();
}


void CUIDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	m_ui_data.draw_area_width = cx;
	m_ui_data.draw_area_height = cy;
	Invalidate();
}


void CUIDialog::OnDestroy()
{
	CDialog::OnDestroy();
	ReleaseDC(m_pDC);
}
