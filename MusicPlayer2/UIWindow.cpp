#include "stdafx.h"
#include "UIWindow.h"
#include "MusicPlayer2.h"
#include "MusicPlayerDlg.h"


void CUIWindow::PreSubclassWindow()
{
    // TODO: 在此添加专用代码和/或调用基类
    DWORD dwStyle = GetStyle();
    ::SetWindowLong(GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);

    CStatic::PreSubclassWindow();
}


BOOL CUIWindow::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if (pMsg->message == WM_MOUSEMOVE)
    {
        m_pUI->GetToolTipCtrl().RelayEvent(pMsg);
    }

    return CStatic::PreTranslateMessage(pMsg);
}
BEGIN_MESSAGE_MAP(CUIWindow, CStatic)
    ON_WM_LBUTTONUP()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONDBLCLK()
//    ON_WM_RBUTTONDBLCLK()
    ON_WM_MOUSEMOVE()
    ON_WM_RBUTTONUP()
    ON_WM_PAINT()
    ON_WM_SIZE()
END_MESSAGE_MAP()


void CUIWindow::OnLButtonUp(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    CMusicPlayerDlg* pMainWindow = dynamic_cast<CMusicPlayerDlg*>(theApp.m_pMainWnd);
    if (pMainWindow != nullptr && !pMainWindow->m_no_lbtnup)
        m_pUI->LButtonUp(point);

    CStatic::OnLButtonUp(nFlags, point);
}


void CUIWindow::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    m_pUI->LButtonDown(point);

    CStatic::OnLButtonDown(nFlags, point);
}


void CUIWindow::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    CStatic::OnLButtonDblClk(nFlags, point);
}


//void CUIWindow::OnRButtonDblClk(UINT nFlags, CPoint point)
//{
//    // TODO: 在此添加消息处理程序代码和/或调用默认值
//
//    CStatic::OnRButtonDblClk(nFlags, point);
//}


void CUIWindow::OnMouseMove(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    m_pUI->MouseMove(point);

    CStatic::OnMouseMove(nFlags, point);
}


void CUIWindow::OnRButtonUp(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    if (nFlags == MK_SHIFT)		//按住Shift键点击鼠标右键时，弹出系统菜单
    {
        CPoint point1;
        GetCursorPos(&point1);
        theApp.m_menu_set.m_main_menu_popup.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, theApp.m_pMainWnd);
    }
    else
    {
        m_pUI->RButtonUp(point);
    }

    CStatic::OnRButtonUp(nFlags, point);
}


void CUIWindow::OnPaint()
{
    CPaintDC dc(this); // device context for painting
                       // TODO: 在此处添加消息处理程序代码
                       // 不为绘图消息调用 CStatic::OnPaint()
    CMusicPlayerDlg* pMainWindow = dynamic_cast<CMusicPlayerDlg*>(theApp.m_pMainWnd);
    //需要重绘时通知线程强制重绘
    if (pMainWindow != nullptr)
        pMainWindow->m_ui_thread_para.ui_force_refresh = true;
}


void CUIWindow::OnSize(UINT nType, int cx, int cy)
{
    CStatic::OnSize(nType, cx, cy);

    // TODO: 在此处添加消息处理程序代码
    theApp.m_ui_data.draw_area_width = cx;
    theApp.m_ui_data.draw_area_height = cy;

    CMusicPlayerDlg* pMainWindow = dynamic_cast<CMusicPlayerDlg*>(theApp.m_pMainWnd);
    if (pMainWindow != nullptr)
    {
        pMainWindow->m_ui_thread_para.ui_force_refresh = true;
    }
}
