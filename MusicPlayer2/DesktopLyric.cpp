#include "stdafx.h"
#include "DesktopLyric.h"
#include "MusicPlayer2.h"
#include "PlayListCtrl.h"
#include "GdiPlusTool.h"

CDesktopLyric::CDesktopLyric()
{
    //��ʼ�����Ԥ����ʽ
    m_default_style[0].normal_style.color1 = RGB(37, 152, 10);
    m_default_style[0].normal_style.color2 = RGB(129, 249, 0);
    m_default_style[0].normal_style.gradient_mode = 1;
    m_default_style[0].highlight_style.color1 = RGB(253, 232, 0);
    m_default_style[0].highlight_style.color2 = RGB(255, 120, 0);
    m_default_style[0].highlight_style.gradient_mode = 2;

    m_default_style[1].normal_style.color1 = RGB(252, 82, 66);
    m_default_style[1].normal_style.color2 = RGB(255, 128, 0);
    m_default_style[1].normal_style.gradient_mode = 1;
    m_default_style[1].highlight_style.color1 = RGB(255, 255, 0);
    m_default_style[1].highlight_style.color2 = RGB(255, 192, 0);
    m_default_style[1].highlight_style.gradient_mode = 2;

    m_default_style[2].normal_style.color1 = RGB(210, 137, 255);
    m_default_style[2].normal_style.color2 = RGB(200, 227, 255);
    m_default_style[2].normal_style.gradient_mode = 1;
    m_default_style[2].highlight_style.color1 = RGB(98, 237, 245);
    m_default_style[2].highlight_style.color2 = RGB(74, 145, 253);
    m_default_style[2].highlight_style.gradient_mode = 2;

    m_toobar_height = theApp.DPI(24);
}


CDesktopLyric::~CDesktopLyric()
{
}

BEGIN_MESSAGE_MAP(CDesktopLyric, CLyricsWindow)
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_MOUSEHOVER()
    ON_WM_MOUSELEAVE()
    ON_WM_SIZING()
    ON_WM_RBUTTONUP()
    ON_WM_GETMINMAXINFO()
    ON_MESSAGE(WM_INITMENU, &CDesktopLyric::OnInitmenu)
    ON_WM_TIMER()

END_MESSAGE_MAP()

void CDesktopLyric::Create()
{
	CLyricsWindow::Create(theApp.DPI(150));

    m_popupMenu.LoadMenu(IDR_DESKTOP_LYRIC_POPUP_MENU);

    //��ʼ����ʾ��Ϣ
    m_tool_tip.Create(this, TTS_ALWAYSTIP);
    m_tool_tip.SetMaxTipWidth(theApp.DPI(400));

    //��ʼ����ʱ��
    SetTimer(TIMER_DESKTOP_LYRIC, 200, NULL);

}

void CDesktopLyric::ShowLyric()
{
	if (!IsWindowVisible())
		return;

	if(!CPlayer::GetInstance().m_Lyrics.IsEmpty())
	{
		Time time{ CPlayer::GetInstance().GetCurrentPosition() };
		int progress = CPlayer::GetInstance().m_Lyrics.GetLyricProgress(time);
		CLyrics::Lyric lyric = CPlayer::GetInstance().m_Lyrics.GetLyric(time, 0);
		if (lyric.text.empty())
			lyric.text = CCommon::LoadText(IDS_DEFAULT_LYRIC_TEXT_CORTANA);

        SetLyricDoubleLine(theApp.m_lyric_setting_data.desktop_lyric_data.lyric_double_line);
        SetShowTranslate(theApp.m_ui_data.show_translate);
        if(theApp.m_lyric_setting_data.desktop_lyric_data.lyric_double_line)
        {
            CLyrics::Lyric next_lyric = CPlayer::GetInstance().m_Lyrics.GetLyric(time, 1);
            if (next_lyric.text.empty())
                next_lyric.text = CCommon::LoadText(IDS_DEFAULT_LYRIC_TEXT_CORTANA);
            SetNextLyric(next_lyric.text.c_str());
        }

        int lyric_index = CPlayer::GetInstance().m_Lyrics.GetLyricIndex(time);
        static int last_lyric_index = -1;

		if (lyric_index != last_lyric_index)
		{
            SetLyricChangeFlag(true);
            last_lyric_index = lyric_index;
		}
		else
		{
            SetLyricChangeFlag(false);
		}
        UpdateLyricTranslate(lyric.translate.c_str());
        UpdateLyrics(lyric.text.c_str(), progress);
	}
	else
	{
		const SongInfo& cur_song_info = CPlayer::GetInstance().GetCurrentSongInfo();
		std::wstring display_text = CPlayListCtrl::GetDisplayStr(cur_song_info, DF_ARTIST_TITLE);
		if (display_text != GetLyricStr().GetString())
		{
			UpdateLyrics(display_text.c_str(), 0);
		}
		else
		{
			UpdateLyrics(0);
		}
        UpdateLyricTranslate(_T(""));
        SetNextLyric(_T(""));
	}

}

void CDesktopLyric::ClearLyric()
{
	UpdateLyricTranslate(_T(""));
	UpdateLyrics(_T(""), 0);
    SetNextLyric(_T(""));
}

void CDesktopLyric::ApplySettings(const DesktopLyricSettingData& data)
{
	SetLyricsFont(data.lyric_font.name.c_str(), theApp.DPI(data.lyric_font.size), ToGDIPluseFontStyle(data.lyric_font.style));
	SetLyricsColor(ToGDIPluseColor(data.text_color1), ToGDIPluseColor(data.text_color2), static_cast<LyricsGradientMode>(data.text_gradient));
	SetHighlightColor(ToGDIPluseColor(data.highlight_color1), ToGDIPluseColor(data.highlight_color2), static_cast<LyricsGradientMode>(data.highlight_gradient));
	SetLyricWindowLock(data.lock_desktop_lyric);
    SetLyricBackgroundPenetrate(data.lyric_background_penetrate);
}

void CDesktopLyric::SetLyricWindowVisible(bool visible)
{
	ShowWindow(visible);
}

void CDesktopLyric::SetLyricWindowLock(bool locked)
{
	if (locked)
	{
		//������괩͸
        ModifyStyleEx(NULL, WS_EX_TRANSPARENT);
        ModifyStyle(WS_THICKFRAME, NULL);
	}
	else
	{
		//ȡ����괩͸
        ModifyStyleEx(WS_EX_TRANSPARENT, NULL);
        ModifyStyle(NULL, WS_THICKFRAME);
	}
    m_bDrawBackground = !locked;
}

void CDesktopLyric::SetLyricOpacity(int opacity)
{
    SetAlpha(opacity * 255 / 100);
}

void CDesktopLyric::SetLyricBackgroundPenetrate(bool penetrate)
{
    m_lyricBackgroundPenetrate = penetrate;
    if(penetrate)
        ModifyStyle(WS_THICKFRAME, NULL);
    else
        ModifyStyle(NULL, WS_THICKFRAME);
}

LyricStyleDefaultData CDesktopLyric::GetDefaultStyle(int index)
{
    if (index < 0 || index >= LYRIC_DEFAULT_STYLE_NUM)
        index = 0;
    return m_default_style[index];
}

int CDesktopLyric::ToGDIPluseFontStyle(const FontStyle& style)
{
	int value = 0;
	if (style.bold)
		value |= Gdiplus::FontStyle::FontStyleBold;
	if (style.italic)
		value |= Gdiplus::FontStyle::FontStyleItalic;
	if (style.underline)
		value |= Gdiplus::FontStyle::FontStyleUnderline;
	if (style.strike_out)
		value |= Gdiplus::FontStyle::FontStyleStrikeout;
	return value;

}

Gdiplus::Color CDesktopLyric::ToGDIPluseColor(COLORREF color)
{
	Gdiplus::Color gdiplus_color;
	gdiplus_color.SetFromCOLORREF(color);
	return gdiplus_color;
}

void CDesktopLyric::DrawToolbar(Gdiplus::Graphics* pGraphics)
{
    bool bLocked = theApp.m_lyric_setting_data.desktop_lyric_data.lock_desktop_lyric;
    const int toolbar_num = bLocked ? 1 : 10;            //
    const int btn_size = m_toobar_height;
    int toolbar_width = toolbar_num * btn_size;
    Gdiplus::Rect toolbar_rect;
    toolbar_rect.Y = 0;
    toolbar_rect.X = (m_rcWindow.Width() - toolbar_width) / 2;
    toolbar_rect.Width = toolbar_width;
    toolbar_rect.Height = btn_size;

    //���Ʊ���
    Gdiplus::Color back_color = CGdiPlusTool::COLORREFToGdiplusColor(theApp.m_app_setting_data.theme_color.light2, 180);
    Gdiplus::Brush* pBrush = new Gdiplus::SolidBrush(back_color);
    pGraphics->FillRectangle(pBrush, toolbar_rect);
    delete pBrush;

    CRect rcIcon = CRect(toolbar_rect.X, toolbar_rect.Y, toolbar_rect.GetRight(), toolbar_rect.GetBottom());
    rcIcon.right = rcIcon.left + btn_size;

    if (bLocked)    //�����������״̬��ֻ����һ������ͼ��
    {
        for (auto& btn : m_buttons)
        {
            if (btn.first == BTN_LOCK)
                DrawToolIcon(pGraphics, theApp.m_icon_set.lock, rcIcon, BTN_LOCK);
            else
                btn.second = UIButton();
        }
    }
    else
    {
        DrawToolIcon(pGraphics, theApp.m_icon_set.app, rcIcon, BTN_APP);
        rcIcon.MoveToX(rcIcon.right);
        DrawToolIcon(pGraphics, theApp.m_icon_set.stop_l, rcIcon, BTN_STOP);
        rcIcon.MoveToX(rcIcon.right);
        DrawToolIcon(pGraphics, theApp.m_icon_set.previous, rcIcon, BTN_PREVIOUS);
        rcIcon.MoveToX(rcIcon.right);
        IconRes hPlayPauseIcon = (CPlayer::GetInstance().IsPlaying() ? theApp.m_icon_set.pause : theApp.m_icon_set.play);
        DrawToolIcon(pGraphics, hPlayPauseIcon, rcIcon, BTN_PLAY_PAUSE);
        rcIcon.MoveToX(rcIcon.right);
        DrawToolIcon(pGraphics, theApp.m_icon_set.next, rcIcon, BTN_NEXT);
        rcIcon.MoveToX(rcIcon.right);
        DrawToolIcon(pGraphics, theApp.m_icon_set.setting, rcIcon, BTN_SETTING);
        rcIcon.MoveToX(rcIcon.right);
        DrawToolIcon(pGraphics, theApp.m_icon_set.double_line, rcIcon, BTN_DOUBLE_LINE, theApp.m_lyric_setting_data.desktop_lyric_data.lyric_double_line);
        rcIcon.MoveToX(rcIcon.right);
        DrawToolIcon(pGraphics, theApp.m_icon_set.skin, rcIcon, BTN_BACKGROUND_PENETRATE, theApp.m_lyric_setting_data.desktop_lyric_data.lyric_background_penetrate);
        rcIcon.MoveToX(rcIcon.right);
        DrawToolIcon(pGraphics, theApp.m_icon_set.lock, rcIcon, BTN_LOCK);
        rcIcon.MoveToX(rcIcon.right);
        DrawToolIcon(pGraphics, theApp.m_icon_set.close, rcIcon, BTN_CLOSE);
    }
    static bool last_locked = !bLocked;
    if (last_locked != bLocked)
    {
        UpdateToolTipPosition();
        last_locked = bLocked;
    }
}

void CDesktopLyric::DrawToolIcon(Gdiplus::Graphics* pGraphics, IconRes icon, CRect rect, BtnKey btn_key, bool checked)
{
    rect.DeflateRect(theApp.DPI(2), theApp.DPI(2));
    auto& btn = m_buttons[btn_key];
    btn.rect = rect;

    if (btn.pressed)
        rect.MoveToXY(rect.left + theApp.DPI(1), rect.top + theApp.DPI(1));

    Gdiplus::Color back_color;
    bool draw_background = false;
    if (btn.pressed && btn.hover)
    {
        back_color = CGdiPlusTool::COLORREFToGdiplusColor(theApp.m_app_setting_data.theme_color.dark1, 180);
        draw_background = true;
    }
    else if (btn.hover)
    {
        back_color = CGdiPlusTool::COLORREFToGdiplusColor(theApp.m_app_setting_data.theme_color.light1, 180);
        draw_background = true;
    }
    else if (checked)
    {
        back_color = CGdiPlusTool::COLORREFToGdiplusColor(theApp.m_app_setting_data.theme_color.light1, 110);
        draw_background = true;
    }
    if (draw_background)
    {
        Gdiplus::SolidBrush brush(back_color);
        pGraphics->FillRectangle(&brush, rect.left, rect.top, rect.Width(), rect.Height());
    }

    CRect rc_tmp = rect;
    //ʹͼ���ھ����о���
    CSize icon_size = icon.GetSize();
    rc_tmp.left = rect.left + (rect.Width() - icon_size.cx) / 2;
    rc_tmp.top = rect.top + (rect.Height() - icon_size.cy) / 2;
    rc_tmp.right = rc_tmp.left + icon_size.cx;
    rc_tmp.bottom = rc_tmp.top + icon_size.cy;

    HDC hDC = pGraphics->GetHDC();
    CDrawCommon drawer;
    drawer.Create(CDC::FromHandle(hDC));
    drawer.DrawIcon(icon.GetIcon(true), rc_tmp.TopLeft(), rc_tmp.Size());
    pGraphics->ReleaseHDC(hDC);
}

void CDesktopLyric::AddToolTips()
{
    AddMouseToolTip(BTN_APP, CCommon::LoadText(IDS_MAIN_MENU));
    AddMouseToolTip(BTN_STOP, CCommon::LoadText(IDS_STOP));
    AddMouseToolTip(BTN_PREVIOUS, CCommon::LoadText(IDS_PREVIOUS));
    AddMouseToolTip(BTN_PLAY_PAUSE, CCommon::LoadText(IDS_PLAY_PAUSE));
    AddMouseToolTip(BTN_NEXT, CCommon::LoadText(IDS_NEXT));
    AddMouseToolTip(BTN_SETTING, CCommon::LoadText(IDS_SETTINGS));
    AddMouseToolTip(BTN_DOUBLE_LINE, CCommon::LoadText(IDS_LYRIC_DOUBLE_LINE));
    AddMouseToolTip(BTN_BACKGROUND_PENETRATE, CCommon::LoadText(IDS_LYRIC_BACKGROUND_PENETRATE));
    AddMouseToolTip(BTN_LOCK, CCommon::LoadText(IDS_LOCK_DESKTOP_LYRIC));
    AddMouseToolTip(BTN_CLOSE, CCommon::LoadText(IDS_CLOSE_DESKTOP_LYRIC));

    m_tool_tip.SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
}

void CDesktopLyric::AddMouseToolTip(BtnKey btn, LPCTSTR str)
{
    CRect rcBtn = m_buttons[btn].rect;
    rcBtn.MoveToX(rcBtn.left - m_frameSize.cx);
    rcBtn.MoveToY(rcBtn.top - m_frameSize.cy);
    m_tool_tip.AddTool(this, str, rcBtn, btn + 1000);

}

void CDesktopLyric::UpdateToolTipPosition()
{
    for (const auto& btn : m_buttons)
    {
        CRect rcBtn = btn.second.rect;
        rcBtn.MoveToX(rcBtn.left - m_frameSize.cx);
        rcBtn.MoveToY(rcBtn.top - m_frameSize.cy);
        m_tool_tip.SetToolRect(this, btn.first + 1000, rcBtn);
    }
}


void CDesktopLyric::DrawBackgroundAndToolbar(Gdiplus::Graphics* pGraphics)
{
    //���ư�͸������
    if (m_bDrawBackground && !m_lyricBackgroundPenetrate)
    {
        BYTE alpha = (m_bHover) ? 80 : 1;
        Gdiplus::Brush* pBrush = new Gdiplus::SolidBrush(Gdiplus::Color(alpha, 255, 255, 255));
        pGraphics->FillRectangle(pBrush, 0, 0, m_rcWindow.Width(), m_rcWindow.Height());
        delete pBrush;
    }

    //���ƹ�����
    bool bLocked = theApp.m_lyric_setting_data.desktop_lyric_data.lock_desktop_lyric;
    if (m_lyricBackgroundPenetrate || !m_bDrawBackground ? m_bMouseInWindowRect : m_bHover)
    {
        DrawToolbar(pGraphics);

        if (m_first_draw)
        {
            AddToolTips();
            m_first_draw = false;
        }
    }

}

void CDesktopLyric::OnLButtonDown(UINT nFlags, CPoint point)
{
    CPoint point1 = point;
    point1.x += m_frameSize.cx;
    point1.y += m_frameSize.cy;

    bool point_in_btns = false;
    for (auto& btn : m_buttons)
    {
        if (btn.second.rect.PtInRect(point1) != FALSE)
        {
            btn.second.pressed = true;
            point_in_btns = true;
        }
    }
    if (!point_in_btns)
        PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point1.x, point1.y));

    CLyricsWindow::OnLButtonDown(nFlags, point);
}

void CDesktopLyric::OnLButtonUp(UINT nFlags, CPoint point)
{
    // TODO: �ڴ�������Ϣ������������/�����Ĭ��ֵ
    CPoint point1 = point;
    point1.x += m_frameSize.cx;
    point1.y += m_frameSize.cy;

    for (auto& btn : m_buttons)
    {
        btn.second.pressed = false;

        if (btn.second.rect.PtInRect(point1) && btn.second.enable)
        {
            switch (btn.first)
            {
            case BTN_APP:
            {
                CPoint cur_point;
                GetCursorPos(&cur_point);
                m_bMenuPopedUp = true;
                theApp.m_menu_set.m_main_menu_popup.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, cur_point.x, cur_point.y, this);
                m_bMenuPopedUp = false;
            }
            return;

            case BTN_STOP:
                theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_STOP);
                return;

            case BTN_PREVIOUS:
                theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_PREVIOUS);
                return;
            case BTN_PLAY_PAUSE:
                theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_PLAY_PAUSE);
                return;

            case BTN_NEXT:
                theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_NEXT);
                return;

            case BTN_SETTING:
                theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_OPTION_SETTINGS);
                return;

            case BTN_DOUBLE_LINE:
                theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_LYRIC_DISPLAYED_DOUBLE_LINE);
                return;

            case BTN_BACKGROUND_PENETRATE:
                theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_LYRIC_BACKGROUND_PENETRATE);
                return;

            case BTN_LOCK:
                theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_LOCK_DESKTOP_LRYIC);
                btn.second.hover = false;
                return;

            case BTN_CLOSE:
                theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_CLOSE_DESKTOP_LYRIC);
                return;

            default:
                break;
            }
        }
    }

    CLyricsWindow::OnLButtonUp(nFlags, point);
}


void CDesktopLyric::OnMouseMove(UINT nFlags, CPoint point)
{
    // TODO: �ڴ�������Ϣ������������/�����Ĭ��ֵ
    CPoint point1 = point;
    point1.x += m_frameSize.cx;
    point1.y += m_frameSize.cy;
    for (auto& btn : m_buttons)
    {
        btn.second.hover = (btn.second.rect.PtInRect(point1) != FALSE);
    }

    TRACKMOUSEEVENT tme;
    tme.cbSize = sizeof(tme);
    tme.hwndTrack = m_hWnd;
    tme.dwFlags = TME_LEAVE | TME_HOVER;
    tme.dwHoverTime = 1;
    _TrackMouseEvent(&tme);

    CLyricsWindow::OnMouseMove(nFlags, point);
}


void CDesktopLyric::OnMouseHover(UINT nFlags, CPoint point)
{
    // TODO: �ڴ�������Ϣ������������/�����Ĭ��ֵ
    if (!m_bHover)
    {
        m_bHover = true;
        //Invalidate();
    }
    else
    {
        CLyricsWindow::OnMouseHover(nFlags, point);
    }
}


void CDesktopLyric::OnMouseLeave()
{
    // TODO: �ڴ�������Ϣ������������/�����Ĭ��ֵ
    if (!m_bMenuPopedUp)
    {
        m_bHover = false;
        //Invalidate();
        for (auto& btn : m_buttons)
        {
            btn.second.pressed = false;
        }
    }

    CLyricsWindow::OnMouseLeave();
}


void CDesktopLyric::OnSizing(UINT fwSide, LPRECT pRect)
{
    CLyricsWindow::OnSizing(fwSide, pRect);

    // TODO: �ڴ˴�������Ϣ�����������
    m_bHover = true;
    UpdateToolTipPosition();
}


void CDesktopLyric::OnRButtonUp(UINT nFlags, CPoint point)
{
    // TODO: �ڴ�������Ϣ������������/�����Ĭ��ֵ
    m_bMenuPopedUp = true;
    CPoint point1;		//����һ������ȷ�����λ�õ�λ��
    GetCursorPos(&point1);	//��ȡ��ǰ����λ�ã��Ա�ʹ�ò˵����Ը����꣬��λ������Ļ���Ͻǵ�Ϊԭ�㣬point���Կͻ������Ͻ�Ϊԭ��
    CMenu* pMenu = m_popupMenu.GetSubMenu(0);
    if (pMenu != NULL)
        pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, this);
    m_bMenuPopedUp = false;
    //CLyricsWindow::OnRButtonUp(nFlags, point);
}


BOOL CDesktopLyric::OnCommand(WPARAM wParam, LPARAM lParam)
{
    // TODO: �ڴ�����ר�ô����/����û���
    WORD command = LOWORD(wParam);
    if (CCommon::IsMenuItemInMenu(m_popupMenu.GetSubMenu(0), command) || CCommon::IsMenuItemInMenu(&theApp.m_menu_set.m_main_menu, command))
        AfxGetMainWnd()->SendMessage(WM_COMMAND, wParam, lParam);		//���˵�����ת����������

    return CLyricsWindow::OnCommand(wParam, lParam);
}


void CDesktopLyric::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    // TODO: �ڴ�������Ϣ������������/�����Ĭ��ֵ
    lpMMI->ptMinTrackSize.x = theApp.DPI(400);
    lpMMI->ptMinTrackSize.y = theApp.DPI(100);

    CLyricsWindow::OnGetMinMaxInfo(lpMMI);
}


afx_msg LRESULT CDesktopLyric::OnInitmenu(WPARAM wParam, LPARAM lParam)
{
    AfxGetMainWnd()->SendMessage(WM_INITMENU, wParam, lParam);        //��WM_INITMENU��Ϣת����������
    return 0;
}


BOOL CDesktopLyric::PreTranslateMessage(MSG* pMsg)
{
    // TODO: �ڴ�����ר�ô����/����û���
    if (pMsg->message == WM_MOUSEMOVE)
        m_tool_tip.RelayEvent(pMsg);

    return CLyricsWindow::PreTranslateMessage(pMsg);
}


void CDesktopLyric::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: �ڴ�������Ϣ������������/�����Ĭ��ֵ
    if (nIDEvent == TIMER_DESKTOP_LYRIC)
    {
        CPoint point;
        GetCursorPos(&point);
        m_bMouseInWindowRect = m_rcWindow.PtInRect(point);

        bool bLocked = theApp.m_lyric_setting_data.desktop_lyric_data.lock_desktop_lyric;
        if (bLocked)        //��������״̬ʱ�����ָ�봦������������ť�����ڣ���ȡ����괩͸״̬����ʹ�á���������ť���Ե��
        {
            CRect rcLockBtn = m_buttons[BTN_LOCK].rect;
            rcLockBtn.MoveToX(rcLockBtn.left + m_rcWindow.left);
            rcLockBtn.MoveToY(rcLockBtn.top + m_rcWindow.top);
            if (rcLockBtn.PtInRect(point))
            {
                SetWindowLong(GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong(GetSafeHwnd(), GWL_EXSTYLE) & (~WS_EX_TRANSPARENT));		//ȡ����괩͸
                m_buttons[BTN_LOCK].hover = true;
            }
            else
            {
                SetWindowLong(GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong(GetSafeHwnd(), GWL_EXSTYLE) | WS_EX_TRANSPARENT);		//������괩͸
                m_buttons[BTN_LOCK].hover = false;
            }
        }
    }

    CLyricsWindow::OnTimer(nIDEvent);
}