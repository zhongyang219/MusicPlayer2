// ProgressStatic.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "PlayerToolBar.h"


// CPlayerToolBar

IMPLEMENT_DYNAMIC(CPlayerToolBar, CStatic)

CPlayerToolBar::CPlayerToolBar()
    : m_theme_color(theApp.m_app_setting_data.theme_color)
{
}

CPlayerToolBar::~CPlayerToolBar()
{
}

void CPlayerToolBar::AddToolButton(IconRes icon, LPCTSTR strText, LPCTSTR strToolTip, UINT cmdId, bool showText)
{
    ToolBtn btn;
    btn.cmd_id = cmdId;
    btn.is_cmd = true;
    btn.icon = icon;
    if (strText != nullptr)
        btn.text = strText;
    btn.show_text = showText;
    if (strToolTip != nullptr)
        btn.tooltip_text = strToolTip;
    m_buttons.push_back(btn);
}

void CPlayerToolBar::AddToolButton(IconRes icon, LPCTSTR strText, LPCTSTR strToolTip, CMenu * pMenu, bool showText)
{
    ToolBtn btn;
    btn.pMenu = pMenu;
    btn.is_cmd = false;
    btn.icon = icon;
    if (strText != nullptr)
        btn.text = strText;
    btn.show_text = showText;
    if(strToolTip != nullptr)
        btn.tooltip_text = strToolTip;
    m_buttons.push_back(btn);
}

void CPlayerToolBar::ModifyToolButton(int index, IconRes icon, LPCTSTR strText, LPCTSTR strToolTip, CMenu* pMenu, bool showText /*= false*/)
{
    if (index >= 0 && index < static_cast<int>(m_buttons.size()))
    {
        ToolBtn& btn{ m_buttons[index] };
        btn.pMenu = pMenu;
        btn.is_cmd = false;
        btn.icon = icon;
        if (strText != nullptr)
            btn.text = strText;
        btn.show_text = showText;
        if (strToolTip != nullptr)
        {
            btn.tooltip_text = strToolTip;
            m_tool_tip.UpdateTipText(btn.tooltip_text, this, index + 100);
        }
        m_btn_updated = true;
    }
}

void CPlayerToolBar::ModifyToolButton(int index, IconRes icon, LPCTSTR strText, LPCTSTR strToolTip, UINT cmdId, bool showText /*= false*/)
{
    if (index >= 0 && index < static_cast<int>(m_buttons.size()))
    {
        ToolBtn& btn{ m_buttons[index] };
        btn.cmd_id = cmdId;
        btn.is_cmd = true;
        btn.icon = icon;
        if (strText != nullptr)
            btn.text = strText;
        btn.show_text = showText;
        if (strToolTip != nullptr)
        {
            btn.tooltip_text = strToolTip;
            m_tool_tip.UpdateTipText(btn.tooltip_text, this, index + 100);
        }
        m_btn_updated = true;
    }
}

void CPlayerToolBar::SetIconSize(int size)
{
    m_icon_size = size;
}


void CPlayerToolBar::SetCmdReciveWindow(CWnd* pWnd)
{
    m_pCmdReciveWnd = pWnd;
}

void CPlayerToolBar::AddToolTips()
{
    for (size_t i = 0; i < m_buttons.size(); i++)
    {
        if(!m_buttons[i].tooltip_text.IsEmpty())
            m_tool_tip.AddTool(this, m_buttons[i].tooltip_text, m_buttons[i].rect, i + 100);
    }
}

void CPlayerToolBar::UpdateToolTipsPosition()
{
    for (size_t i = 0; i < m_buttons.size(); i++)
    {
        m_tool_tip.SetToolRect(this,i + 100, m_buttons[i].rect);
    }
}


CWnd* CPlayerToolBar::GetCmdReciveWindow()
{
    CWnd* pWnd;
    if (m_pCmdReciveWnd != nullptr)
        pWnd = m_pCmdReciveWnd;
    else if (GetParent() != nullptr)
        pWnd = GetParent();
    else
        pWnd = AfxGetMainWnd();
    return pWnd;
}

BEGIN_MESSAGE_MAP(CPlayerToolBar, CStatic)
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
    ON_WM_MOUSELEAVE()
    ON_WM_MOUSEHOVER()
    ON_MESSAGE(WM_INITMENU, &CPlayerToolBar::OnInitmenu)
    ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()



// CPlayerToolBar 消息处理程序

void CPlayerToolBar::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CStatic::OnPaint()

    CRect rect;
    GetClientRect(rect);

    //双缓冲绘图
    {
        CDrawDoubleBuffer drawDoubleBuffer(&dc, rect);
        CDrawCommon drawer;
        drawer.Create(drawDoubleBuffer.GetMemDC(), this);

        //绘制背景
        if(!theApp.m_app_setting_data.button_round_corners)
        {
            drawer.FillRect(rect, CColorConvert::m_gray_color.light3);
        }
        else
        {
            drawer.FillRect(rect, CONSTVAL::BACKGROUND_COLOR);
            drawer.DrawRoundRect(rect, CColorConvert::m_gray_color.light3, theApp.DPI(4));
        }

        //绘制图标
        CRect rc_icon = rect;
        rc_icon.left += theApp.DPI(2);
        rc_icon.top = (rect.Height() - m_icon_size) / 2;
        rc_icon.right = rc_icon.left;
        rc_icon.bottom = rc_icon.top + m_icon_size;

        for (auto iter = m_buttons.begin(); iter != m_buttons.end(); iter++)
        {
            if (iter != m_buttons.begin())
            {
                rc_icon.left = (iter - 1)->rect.right + theApp.DPI(2);
            }

            bool draw_text = iter->show_text && !(iter->text.IsEmpty());
            if (draw_text)
            {
                CSize text_size = drawer.GetTextExtent(iter->text);
                rc_icon.right = rc_icon.left + m_icon_size + text_size.cx + theApp.DPI(4);
            }
            else
            {
                rc_icon.right = rc_icon.left + m_icon_size;
            }

            iter->rect = rc_icon;
            COLORREF back_color{};
            if(iter->hover || iter->pressed)
            {
                if (iter->pressed)
                    back_color = m_theme_color.light1_5;
                else
                    back_color = m_theme_color.light2_5;
                if (!theApp.m_app_setting_data.button_round_corners)
                    drawer.FillRect(rc_icon, back_color);
                else
                {
                    drawer.SetDrawArea(rc_icon);
                    drawer.DrawRoundRect(rc_icon, back_color, theApp.DPI(3));
                }
            }

            CRect rc_tmp = rc_icon;
            //使图标在矩形中居中
            CSize icon_size = iter->icon.GetSize();
            if (draw_text)
                rc_tmp.left = rc_icon.left + theApp.DPI(2);
            else
                rc_tmp.left = rc_icon.left + (rc_icon.Width() - icon_size.cx) / 2;
            rc_tmp.top = rc_icon.top + (rc_icon.Height() - icon_size.cy) / 2;
            rc_tmp.right = rc_tmp.left + icon_size.cx;
            rc_tmp.bottom = rc_tmp.top + icon_size.cy;
            if (iter->pressed)
            {
                rc_tmp.MoveToX(rc_tmp.left + theApp.DPI(1));
                rc_tmp.MoveToY(rc_tmp.top + theApp.DPI(1));
            }
            drawer.SetDrawArea(rc_tmp);
            drawer.DrawIcon(iter->icon.GetIcon(true), rc_tmp.TopLeft(), rc_tmp.Size());
            //绘制文本
            if (draw_text)
            {
                CRect rc_text = rc_icon;
                rc_text.left = rc_tmp.right + theApp.DPI(2);
                COLORREF text_color;
                if (IsWindowEnabled())
                    text_color = CColorConvert::m_gray_color.dark3;
                else
                    text_color = CColorConvert::m_gray_color.dark1;
                if (iter->pressed)
                    rc_text.MoveToY(rc_text.top + theApp.DPI(1));
                drawer.DrawWindowText(rc_text, iter->text, text_color);
            }
        }
    }

    if (m_first_draw && !m_buttons.empty())
    {
        AddToolTips();              //在第一次绘制完成之后添加鼠标提示，因为在绘制之前无法确定按钮矩形区域
        m_first_draw = false;
    }

    if (m_btn_updated && !m_buttons.empty())        //如果更新过按钮，则在绘制后更新一次鼠标提示的位置
    {
        UpdateToolTipsPosition();
        m_btn_updated = false;
    }
}


void CPlayerToolBar::PreSubclassWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	//为控件设置SS_NOTIFY属性
    ModifyStyle(NULL, SS_NOTIFY);

    //初始化提示信息
    m_tool_tip.Create(this, TTS_ALWAYSTIP);
    m_tool_tip.SetMaxTipWidth(theApp.DPI(400));

	CStatic::PreSubclassWindow();
}


void CPlayerToolBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
    for (auto& btn : m_buttons)
    {
        if (btn.rect.PtInRect(point) != FALSE)
        {
            btn.pressed = true;
            InvalidateRect(btn.rect);
        }
    }

	CStatic::OnLButtonDown(nFlags, point);
}


void CPlayerToolBar::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
    for (auto& btn : m_buttons)
    {
        btn.pressed = false;
        if (btn.rect.PtInRect(point) && btn.enable)
        {
            if (btn.is_cmd)
            {
                GetCmdReciveWindow()->SendMessage(WM_COMMAND, btn.cmd_id, 0);
            }
            else if (btn.pMenu != nullptr)
            {
                CPoint point;
                point.x = btn.rect.left;
                point.y = btn.rect.bottom;
                ClientToScreen(&point);
                m_menu_poped_up = true;
                btn.pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
                m_menu_poped_up = false;
            }
        }
    }
    Invalidate();
    CStatic::OnLButtonUp(nFlags, point);
}


void CPlayerToolBar::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
    for (auto& btn : m_buttons)
    {
        bool hover = (btn.rect.PtInRect(point) != FALSE);
        if(hover != btn.hover)
        {
            btn.hover = hover;
            InvalidateRect(btn.rect);
        }
    }

    TRACKMOUSEEVENT tme;
    tme.cbSize = sizeof(tme);
    tme.hwndTrack = m_hWnd;
    tme.dwFlags = TME_LEAVE | TME_HOVER;
    tme.dwHoverTime = 1;
    _TrackMouseEvent(&tme);

	CStatic::OnMouseMove(nFlags, point);
}


BOOL CPlayerToolBar::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (m_tool_tip.GetSafeHwnd() && pMsg->message == WM_MOUSEMOVE)
	{
		m_tool_tip.RelayEvent(pMsg);
	}


	return CStatic::PreTranslateMessage(pMsg);
}


void CPlayerToolBar::OnMouseLeave()
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    if(!m_menu_poped_up)
    {
        for (auto& btn : m_buttons)
        {
            btn.pressed = false;
            btn.hover = false;
        }
        Invalidate();
    }

    CStatic::OnMouseLeave();
}


void CPlayerToolBar::OnMouseHover(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    CStatic::OnMouseHover(nFlags, point);
}


afx_msg LRESULT CPlayerToolBar::OnInitmenu(WPARAM wParam, LPARAM lParam)
{
    AfxGetMainWnd()->SendMessage(WM_INITMENU, wParam, lParam);        //将WM_INITMENU消息转发到主窗口
    return 0;
}


BOOL CPlayerToolBar::OnCommand(WPARAM wParam, LPARAM lParam)
{
    // TODO: 在此添加专用代码和/或调用基类
    GetCmdReciveWindow()->SendMessage(WM_COMMAND, wParam, lParam);        //转发WM_COMMAND消息

    return CStatic::OnCommand(wParam, lParam);
}


void CPlayerToolBar::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    //CStatic::OnLButtonDblClk(nFlags, point);
}
