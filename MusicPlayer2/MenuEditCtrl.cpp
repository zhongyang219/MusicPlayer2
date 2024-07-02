// CMenuEditCtrl.cpp: 实现文件
//

#include "stdafx.h"
#include "MenuEditCtrl.h"
#include "MusicPlayer2.h"
#include "DrawCommon.h"

// CMenuEditCtrl

IMPLEMENT_DYNAMIC(CMenuEditCtrl, CMFCEditBrowseCtrl)

CMenuEditCtrl::CMenuEditCtrl()
    : m_theme_color(theApp.m_app_setting_data.theme_color)
{
}

CMenuEditCtrl::~CMenuEditCtrl()
{
}

void CMenuEditCtrl::SetTooltopText(const CString& tooltip_text)
{
    m_tooltip_text = tooltip_text;
}

void CMenuEditCtrl::OnBrowse()
{
    PostMessage(WM_KILLFOCUS, 0, 0);
    CRect rect;
    GetWindowRect(rect);
    //ClientToScreen(rect);
    theApp.m_menu_mgr.GetMenu(MenuMgr::RecentFolderPlaylistMenu)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, rect.left, rect.bottom, AfxGetMainWnd());
}

void CMenuEditCtrl::OnDrawBrowseButton(CDC* pDC, CRect rect, BOOL bIsButtonPressed, BOOL bIsButtonHot)
{
    if (m_btn_rect != rect)
    {
        m_btn_rect = rect;
        UpdateToolTipPosition();
    }
    //使用双缓冲绘图
    CDrawDoubleBuffer drawDoubleBuffer(pDC, rect);
    CDrawCommon drawer;
    drawer.Create(drawDoubleBuffer.GetMemDC());
    CRect rc_draw{ rect };
    rc_draw.MoveToXY(0, 0);
    
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
    if (bIsButtonPressed)
        btn_color = m_theme_color.light1_5;
    else if (bIsButtonHot)
        btn_color = m_theme_color.light2_5;
    else
        btn_color = CColorConvert::m_gray_color.light3;

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

    CSize icon_size(theApp.DPI(16), theApp.DPI(16));
    CPoint icon_top_left;
    icon_top_left.x = rc_draw.left + (rc_draw.Width() - icon_size.cx) / 2;
    icon_top_left.y = rc_draw.top + (rc_draw.Height() - icon_size.cy) / 2;
    HICON hIcon = theApp.m_icon_mgr.GetHICON(IconMgr::IconType::IT_Triangle_Down, IconMgr::IconStyle::IS_OutlinedDark, IconMgr::IconSize::IS_DPI_16);
    drawer.DrawIcon(hIcon, icon_top_left, icon_size);
}


void CMenuEditCtrl::OnChangeLayout()
{
    ASSERT_VALID(this);
    ENSURE(GetSafeHwnd() != NULL);

    m_nBrowseButtonWidth = max(theApp.DPI(20), m_sizeImage.cx + 8);

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

void CMenuEditCtrl::UpdateToolTipPosition()
{
    if (!m_btn_rect.IsRectEmpty())
        m_tool_tip.SetToolRect(this, 1, m_btn_rect);
}

BEGIN_MESSAGE_MAP(CMenuEditCtrl, CMFCEditBrowseCtrl)
    ON_WM_NCLBUTTONDOWN()
    ON_MESSAGE(WM_TABLET_QUERYSYSTEMGESTURESTATUS, &CMenuEditCtrl::OnTabletQuerysystemgesturestatus)
END_MESSAGE_MAP()



void CMenuEditCtrl::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
    if (HTCAPTION == nHitTest)
    {
        return;
    }
    CMFCEditBrowseCtrl::OnNcLButtonDown(nHitTest, point);
}


afx_msg LRESULT CMenuEditCtrl::OnTabletQuerysystemgesturestatus(WPARAM wParam, LPARAM lParam)
{
    return 0;
}


void CMenuEditCtrl::PreSubclassWindow()
{
    // TODO: 在此添加专用代码和/或调用基类
    m_tool_tip.Create(this, TTS_ALWAYSTIP);
    m_tool_tip.SetMaxTipWidth(theApp.DPI(400));
    m_tool_tip.AddTool(this, m_tooltip_text, CRect(), 1);
    UpdateToolTipPosition();

    CMFCEditBrowseCtrl::PreSubclassWindow();
}


BOOL CMenuEditCtrl::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if (pMsg->message == WM_MOUSEMOVE)
        m_tool_tip.RelayEvent(pMsg);

    return CMFCEditBrowseCtrl::PreTranslateMessage(pMsg);
}
