// CSearchEditCtrl.cpp: 实现文件
//

#include "stdafx.h"
#include "SearchEditCtrl.h"
#include "MusicPlayer2.h"
#include "DrawCommon.h"

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
    COLORREF back_color;
    if (bIsButtonPressed)
        back_color = m_theme_color.light1_5;
    else if (bIsButtonHot)
        back_color = m_theme_color.light2_5;
    else
        back_color = CColorConvert::m_gray_color.light3;
    pDC->FillSolidRect(rect, back_color);

    auto& icon = theApp.m_icon_set.close;
    CSize icon_size = icon.GetSize();
    CPoint icon_top_left;
    icon_top_left.x = rect.left + (rect.Width() - icon_size.cx) / 2;
    icon_top_left.y = rect.top + (rect.Height() - icon_size.cy) / 2;
    CDrawCommon drawer;
    drawer.Create(pDC, this);
    drawer.DrawIcon(icon.GetIcon(), icon_top_left, icon_size);
}


void CSearchEditCtrl::OnChangeLayout()
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

void CSearchEditCtrl::UpdateToolTipPosition()
{
    CRect rc_client;
    GetWindowRect(rc_client);
    rc_client.MoveToXY(0, 0);
    CRect rc_edit = rc_client;
    rc_edit.right = rc_client.right - m_nBrowseButtonWidth - theApp.DPI(4);
    CRect rc_btn = rc_client;
    rc_btn.left = rc_edit.right + theApp.DPI(2);
    m_tool_tip.SetToolRect(this, 1, rc_btn);
    m_tool_tip.SetToolRect(this, 2, rc_edit);
}

BEGIN_MESSAGE_MAP(CSearchEditCtrl, CMFCEditBrowseCtrl)
    ON_WM_SIZE()
END_MESSAGE_MAP()



// CSearchEditCtrl 消息处理程序




void CSearchEditCtrl::PreSubclassWindow()
{
    // TODO: 在此添加专用代码和/或调用基类
    m_tool_tip.Create(this, TTS_ALWAYSTIP);
    m_tool_tip.SetMaxTipWidth(theApp.DPI(400));
    m_tool_tip.AddTool(this, CCommon::LoadText(IDS_CLEAR_SEARCH_RESULT), CRect(), 1);
    m_tool_tip.AddTool(this, CCommon::LoadText(IDS_INPUT_KEY_WORD), CRect(), 2);
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
