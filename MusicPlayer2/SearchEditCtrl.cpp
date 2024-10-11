// CSearchEditCtrl.cpp: 实现文件
//

#include "stdafx.h"
#include "SearchEditCtrl.h"
#include "MusicPlayer2.h"
#include "DrawCommon.h"
#include <algorithm>
#undef min
#undef max

// CSearchEditCtrl

IMPLEMENT_DYNAMIC(CSearchEditCtrl, CMFCEditBrowseCtrl)

CSearchEditCtrl::CSearchEditCtrl()
    : m_theme_color(theApp.m_app_setting_data.theme_color)
{
}

CSearchEditCtrl::~CSearchEditCtrl()
{
}

void CSearchEditCtrl::OnBrowse()
{
    CWnd* pParent = GetParent();
    if (pParent != nullptr)
        pParent->SendMessage(WM_SEARCH_EDIT_BTN_CLICKED, (WPARAM)this, 0);
}

void CSearchEditCtrl::OnDrawBrowseButton(CDC * pDC, CRect rect, BOOL bIsButtonPressed, BOOL bIsButtonHot)
{
    //使用双缓冲绘图
    CDrawDoubleBuffer drawDoubleBuffer(pDC, rect);
    CDrawCommon drawer;
    drawer.Create(drawDoubleBuffer.GetMemDC());
    CRect rc_draw{ rect };
    rc_draw.MoveToXY(0, 0);

    m_draw_clear_btn = (GetWindowTextLength() > 0);

    //使用圆角风格时，先填充背景色，再画按钮
    if (theApp.m_app_setting_data.button_round_corners)
    {
        COLORREF back_color;
        bool is_read_only = (GetStyle() & ES_READONLY) != 0;
        if (is_read_only || !IsWindowEnabled())
            back_color = GetSysColor(COLOR_3DFACE);
        else
            back_color = GetSysColor(COLOR_WINDOW);
        drawer.GetDC()->FillSolidRect(rc_draw, back_color);
    }

    COLORREF btn_color;
    if (m_draw_clear_btn)
    {
        if (bIsButtonPressed)
            btn_color = m_theme_color.light1_5;
        else if (bIsButtonHot)
            btn_color = m_theme_color.light2_5;
        else
            btn_color = CColorConvert::m_gray_color.light3;
    }
    else
    {
        btn_color = GetSysColor(COLOR_WINDOW);
    }

    if (theApp.m_app_setting_data.button_round_corners)
    {
        CRect rc_btn{ rc_draw };
        rc_btn.DeflateRect(theApp.DPI(1), theApp.DPI(1));
        drawer.DrawRoundRect(rc_btn, btn_color, theApp.DPI(3));
    }
    else
    {
        drawer.GetDC()->FillSolidRect(rc_draw, btn_color);
    }

    IconMgr::IconType icon_type = IconMgr::IconType::IT_Find;
    if (m_draw_clear_btn)   // 文本框为空时显示搜索图标，否则显示关闭图标
        icon_type = IconMgr::IconType::IT_Cancel;
    IconMgr::IconSize size_type = m_big_icon ? IconMgr::IconSize::IS_DPI_16_Full_Screen : IconMgr::IconSize::IS_DPI_16;
    HICON hIcon = theApp.m_icon_mgr.GetHICON(icon_type, IconMgr::IconStyle::IS_OutlinedDark, size_type);
    CSize icon_size = IconMgr::GetIconSize(size_type);
    CPoint icon_top_left;
    icon_top_left.x = rc_draw.left + (rc_draw.Width() - icon_size.cx) / 2;
    icon_top_left.y = rc_draw.top + (rc_draw.Height() - icon_size.cy) / 2;
    drawer.DrawIcon(hIcon, icon_top_left, icon_size);

    static bool last_draw_clear_btn{ false };
    if (last_draw_clear_btn != m_draw_clear_btn)
    {
        UpdateToolTipPosition();
        last_draw_clear_btn = m_draw_clear_btn;
    }
}


void CSearchEditCtrl::OnChangeLayout()
{
    ASSERT_VALID(this);
    ENSURE(GetSafeHwnd() != NULL);

    CRect rc_client;
    GetWindowRect(rc_client);
    m_nBrowseButtonWidth = std::max({ theApp.DPI(20), rc_client.Height() - theApp.DPI(3), static_cast<int>(m_sizeImage.cx + 8) });

    SetWindowPos(NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOZORDER | SWP_NOMOVE);

    if (m_Mode != BrowseMode_None)
    {
        GetWindowRect(m_rectBtn);
        m_rectBtn.left = m_rectBtn.right - m_nBrowseButtonWidth;

        ScreenToClient(&m_rectBtn);
    }
    else
    {
        m_rectBtn.SetRectEmpty();
    }

}

void CSearchEditCtrl::SetBigIcon(bool big_icon)
{
    m_big_icon = big_icon;
}

void CSearchEditCtrl::UpdateToolTipPosition()
{
    CRect rc_client;
    GetWindowRect(rc_client);
    rc_client.MoveToXY(0, 0);
    if(m_draw_clear_btn)
    {
        CRect rc_edit = rc_client;
        rc_edit.right = rc_client.right - m_nBrowseButtonWidth - theApp.DPI(4);
        CRect rc_btn = rc_client;
        rc_btn.left = rc_edit.right + theApp.DPI(2);
        m_tool_tip.SetToolRect(this, 1, rc_btn);
        m_tool_tip.SetToolRect(this, 2, rc_edit);
    }
    else
    {
        m_tool_tip.SetToolRect(this, 1, CRect());
        m_tool_tip.SetToolRect(this, 2, rc_client);
    }
}

BEGIN_MESSAGE_MAP(CSearchEditCtrl, CMFCEditBrowseCtrl)
    ON_WM_SIZE()
    ON_CONTROL_REFLECT_EX(EN_CHANGE, &CSearchEditCtrl::OnEnChange)
    ON_WM_NCLBUTTONDOWN()
    ON_MESSAGE(WM_TABLET_QUERYSYSTEMGESTURESTATUS, &CSearchEditCtrl::OnTabletQuerysystemgesturestatus)
END_MESSAGE_MAP()



// CSearchEditCtrl 消息处理程序




void CSearchEditCtrl::PreSubclassWindow()
{
    // TODO: 在此添加专用代码和/或调用基类
    m_tool_tip.Create(this, TTS_ALWAYSTIP);
    m_tool_tip.SetMaxTipWidth(theApp.DPI(400));
    m_tool_tip.AddTool(this, theApp.m_str_table.LoadText(L"TIP_SEARCH_EDIT_CLEAN").c_str(), CRect(), 1);
    m_tool_tip.AddTool(this, theApp.m_str_table.LoadText(L"TIP_SEARCH_EDIT_INPUT").c_str(), CRect(), 2);
    UpdateToolTipPosition();

    CMFCEditBrowseCtrl::PreSubclassWindow();
}


BOOL CSearchEditCtrl::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if (pMsg->message == WM_MOUSEMOVE)
        m_tool_tip.RelayEvent(pMsg);


    return CMFCEditBrowseCtrl::PreTranslateMessage(pMsg);
}


void CSearchEditCtrl::OnSize(UINT nType, int cx, int cy)
{
    CMFCEditBrowseCtrl::OnSize(nType, cx, cy);

    // TODO: 在此处添加消息处理程序代码
    UpdateToolTipPosition();
}


BOOL CSearchEditCtrl::OnEnChange()
{
    // TODO:  如果该控件是 RICHEDIT 控件，它将不
    // 发送此通知，除非重写 CMFCEditBrowseCtrl::OnInitDialog()
    // 函数并调用 CRichEditCtrl().SetEventMask()，
    // 同时将 ENM_CHANGE 标志“或”运算到掩码中。

    // TODO:  在此添加控件通知处理程序代码
    OnNcPaint();
    return FALSE;           //这里返回FALSE表示EN_CHANGE的响应还没有响应完，此消息仍然会被发送到父窗口，否则表示这里已经做完所有的事情，消息不会被发送到父窗口
}


void CSearchEditCtrl::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
    if (HTCAPTION == nHitTest)
    {
        return;
    }
    CMFCEditBrowseCtrl::OnNcLButtonDown(nHitTest, point);
}


afx_msg LRESULT CSearchEditCtrl::OnTabletQuerysystemgesturestatus(WPARAM wParam, LPARAM lParam)
{
    return 0;
}
