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
    ON_WM_MOUSELEAVE()
END_MESSAGE_MAP()


void CUIWindow::OnLButtonUp(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    m_bTitlebarLButtonDown = false; // 此次仅为点击不是拖动

    CMusicPlayerDlg* pMainWindow = dynamic_cast<CMusicPlayerDlg*>(theApp.m_pMainWnd);
    auto pUi = pMainWindow->GetCurrentUi();

    //如果点击了应用图标，则弹出系统菜单
    if (pUi->PointInAppIconArea(point))
    {
        CPoint point1{};
        SLayoutData lyout;
        point1.y = lyout.titlabar_height;
        ClientToScreen(&point1);
        theApp.m_menu_set.m_main_menu_popup.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, theApp.m_pMainWnd);
    }
    else
    {
        if (pMainWindow != nullptr && !pMainWindow->m_no_lbtnup)
            m_pUI->LButtonUp(point);

        CStatic::OnLButtonUp(nFlags, point);
    }
}


void CUIWindow::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    CMusicPlayerDlg* pMainWindow = dynamic_cast<CMusicPlayerDlg*>(theApp.m_pMainWnd);
    auto pUi = pMainWindow->GetCurrentUi();
    if (pUi->PointInTitlebarArea(point) && !pUi->PointInControlArea(point) && !pUi->PointInAppIconArea(point))        //如果鼠标按下的地方是绘制的标题栏区域，并且不是按钮，则拖动窗口
    {
        if(pMainWindow->IsZoomed())     // 最大化窗口需要先退出最大化再拖动，按下不做反应仅记录，如果鼠标移动再处理
            m_bTitlebarLButtonDown = true, GetCursorPos(&m_ptLButtonDown);
        else
            pMainWindow->SendMessage(WM_SYSCOMMAND, SC_MOVE | HTCAPTION);
    }

    m_pUI->LButtonDown(point);

    CStatic::OnLButtonDown(nFlags, point);
}


void CUIWindow::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    CMusicPlayerDlg* pMainWindow = dynamic_cast<CMusicPlayerDlg*>(theApp.m_pMainWnd);
    auto pUi = pMainWindow->GetCurrentUi();
    if (pUi->PointInAppIconArea(point))
    {
        pMainWindow->SendMessage(WM_CLOSE);
    }
    else if (pUi->PointInTitlebarArea(point) && !pUi->PointInControlArea(point))
    {
        if(pMainWindow->IsZoomed())
            pMainWindow->SendMessage(WM_SYSCOMMAND, SC_RESTORE);
        else
            pMainWindow->SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE);
    }

    //CStatic::OnLButtonDblClk(nFlags, point);
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

    // 此处处理最大化窗口的自绘标题栏拖动，需要先设置窗口尺寸退出最大化，移动窗口到适当位置之后进入拖动
    if (m_bTitlebarLButtonDown)
    {
        m_bTitlebarLButtonDown = false;
        CMusicPlayerDlg* pMainWindow = dynamic_cast<CMusicPlayerDlg*>(theApp.m_pMainWnd);
        // 以下处理为屏幕坐标
        CRect rect_max;
        pMainWindow->GetWindowRect(rect_max);                   // 获取最大化窗口位置信息
        pMainWindow->SendMessage(WM_SYSCOMMAND, SC_RESTORE);
        CRect rect;
        pMainWindow->GetWindowRect(rect);                       // 获取还原后窗口位置信息

        CPoint offset{ m_ptLButtonDown - rect_max.TopLeft() };  // 最大化时从窗口原点指向点击位置的向量
        if (theApp.m_ui_data.show_playlist)                     // 将此向量映射为窗口大小还原后的对应向量（忽略边框大小）
            offset.x *= (float)(rect.Width() / 2 - theApp.DPI(30) * 6) / (rect_max.Width() / 2 - theApp.DPI(30) * 6);
        else
            offset.x *= (float)(rect.Width() - theApp.DPI(30) * 6) / (rect_max.Width() - theApp.DPI(30) * 6);
        offset = m_ptLButtonDown - rect.TopLeft() - offset;     // 计算所需偏移量
        pMainWindow->MoveWindow(rect + offset);
        pMainWindow->SendMessage(WM_SYSCOMMAND, SC_MOVE | HTCAPTION);
    }

    m_pUI->MouseMove(point);

    CStatic::OnMouseMove(nFlags, point);
}


void CUIWindow::OnRButtonUp(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    CMusicPlayerDlg* pMainWindow = dynamic_cast<CMusicPlayerDlg*>(theApp.m_pMainWnd);
    auto pUi = pMainWindow->GetCurrentUi();
    if (pUi->PointInTitlebarArea(point) && !pUi->PointInControlArea(point))        //如果鼠标按下的地方是绘制的标题栏区域，并且不是按钮，则弹出系统菜单
    {
        CPoint point1;
        GetCursorPos(&point1);
        theApp.m_menu_set.m_main_menu_popup.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, theApp.m_pMainWnd);
    }
    else if (nFlags == MK_SHIFT)		//按住Shift键点击鼠标右键时，弹出系统菜单
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


void CUIWindow::OnMouseLeave()
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    m_pUI->MouseLeave();

    CStatic::OnMouseLeave();
}
