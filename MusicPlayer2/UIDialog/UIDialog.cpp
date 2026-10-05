// UIDialog.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "UIDialog.h"
#include "WinVersionHelper.h"

#define UI_DIALOG_TIMER_ID 1365

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
    m_ui_data.show_default_context_menu = false;
}

CUIDialog::~CUIDialog()
{
}

BOOL CUIDialog::Create(CWnd* pParent)
{
    return CDialog::Create(IDD_UI_DIALOG, pParent);
}

void CUIDialog::RePaintUi()
{
    Invalidate(FALSE);
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
    ON_WM_GETMINMAXINFO()
    ON_WM_TIMER()
    ON_WM_SETCURSOR()
    ON_WM_KILLFOCUS()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CUIDialog 消息处理程序


BOOL CUIDialog::OnInitDialog()
{
    CDialog::OnInitDialog();

    //深色模式下，为对话框启用深色标题栏
    bool dark_mode = false;
    if (theApp.m_app_setting_data.dark_mode && CWinVersionHelper::IsWindows10Version1809OrLater())
    {
        dark_mode = true;
        BOOL darkMode = TRUE;
        DwmSetWindowAttribute(m_hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &darkMode, sizeof(darkMode));
    }

    //初始化UI
    m_pDC = GetDC();
    m_ui.Init(m_pDC);

    //设置窗口标题和图标
    SetWindowText(m_ui.GetUIName().c_str());
    IconMgr::IconStyle icon_style = dark_mode ? IconMgr::IconStyle::IS_OutlinedLight : IconMgr::IconStyle::IS_OutlinedDark;
    SetIcon(theApp.m_icon_mgr.GetHICON(m_ui.GetUiIcon(), icon_style, IconMgr::IS_DPI_16), FALSE);
    
    //初始化对话框大小
    int width = m_ui.GetCurrentTypeUi()->GetWidth(CRect());
    int height = m_ui.GetCurrentTypeUi()->GetHeight(CRect());
    if (width > 0 && height > 0)
        SetWindowPos(nullptr, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);

    m_min_size.cx = m_ui.GetCurrentTypeUi()->MinWidth().GetValue(CRect());
    m_min_size.cy = m_ui.GetCurrentTypeUi()->MinHeight().GetValue(CRect());

    //隐藏文本编辑框
    //m_ui_edit.ShowWindow(SW_HIDE);

    SetTimer(UI_DIALOG_TIMER_ID, 20, NULL);

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}


BOOL CUIDialog::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_MOUSEMOVE)
    {
        m_ui.GetToolTipCtrl().RelayEvent(pMsg);
    }
    else if (pMsg->message == WM_KEYUP)
    {
        RePaintUi();
    }

    return CDialog::PreTranslateMessage(pMsg);
}


void CUIDialog::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    // 只绘制非控件区域
    CRect rect;
    GetClientRect(&rect);
    CRgn rgn;
    rgn.CreateRectRgnIndirect(&rect);

    CWnd* pChild = GetWindow(GW_CHILD);
    while (pChild)
    {
        if (pChild->IsWindowVisible())
        {
            CRect childRect;
            pChild->GetWindowRect(&childRect);
            ScreenToClient(&childRect);

            CRgn childRgn;
            childRgn.CreateRectRgnIndirect(&childRect);
            rgn.CombineRgn(&rgn, &childRgn, RGN_DIFF);
        }

        pChild = pChild->GetNextWindow();
    }

    m_ui.DrawInfo(false, &rgn);
}


void CUIDialog::OnLButtonUp(UINT nFlags, CPoint point)
{
    m_ui.LButtonUp(point);
    RePaintUi();
    CDialog::OnLButtonUp(nFlags, point);
}


void CUIDialog::OnLButtonDown(UINT nFlags, CPoint point)
{
    m_ui.LButtonDown(point);
    RePaintUi();
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
    RePaintUi();
    CDialog::OnMouseMove(nFlags, point);
}


BOOL CUIDialog::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    CPoint point = pt;
    ScreenToClient(&point);
    m_ui.MouseWheel(zDelta, point);
    RePaintUi();
    return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}


void CUIDialog::OnMouseLeave()
{
    m_ui.MouseLeave();
    RePaintUi();
    CDialog::OnMouseLeave();
}


void CUIDialog::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    m_ui_data.draw_area_width = cx;
    m_ui_data.draw_area_height = cy;
    RePaintUi();
}


void CUIDialog::OnDestroy()
{
    CDialog::OnDestroy();
    ReleaseDC(m_pDC);
}

void CUIDialog::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    //限制窗口最小大小
    lpMMI->ptMinTrackSize.x = m_min_size.cx;
    lpMMI->ptMinTrackSize.y = m_min_size.cy;

    CDialog::OnGetMinMaxInfo(lpMMI);
}

void CUIDialog::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == UI_DIALOG_TIMER_ID)
    {
        //窗口打开时刷新一次界面
        RePaintUi();
        KillTimer(UI_DIALOG_TIMER_ID);
    }
    CDialog::OnTimer(nIDEvent);
}

BOOL CUIDialog::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    if (m_ui.SetCursor())
        return TRUE;

    return CDialog::OnSetCursor(pWnd, nHitTest, message);
}


BOOL CUIDialog::OnEraseBkgnd(CDC* pDC)
{
    //return CDialog::OnEraseBkgnd(pDC);
    return TRUE;
}
