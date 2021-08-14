#include "stdafx.h"
#include "DesktopLyric.h"
#include "MusicPlayer2.h"
#include "PlayListCtrl.h"
#include "GdiPlusTool.h"
#include "Define.h"
#include "CPlayerUIHelper.h"
#include "MusicPlayerDlg.h"
#include "WIC.h"

CDesktopLyric::CDesktopLyric()
{
    m_toobar_height = theApp.DPI(24);
}


CDesktopLyric::~CDesktopLyric()
{
}

BEGIN_MESSAGE_MAP(CDesktopLyric, CLyricsWindow)
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_MOUSEHOVER()
    ON_WM_MOUSELEAVE()
    ON_WM_SIZING()
    ON_WM_RBUTTONUP()
    ON_WM_GETMINMAXINFO()
//    ON_MESSAGE(WM_INITMENU, &CDesktopLyric::OnInitmenu)
    ON_WM_TIMER()

    ON_COMMAND(ID_LYRIC_DEFAULT_STYLE1, &CDesktopLyric::OnLyricDefaultStyle1)
    ON_COMMAND(ID_LYRIC_DEFAULT_STYLE2, &CDesktopLyric::OnLyricDefaultStyle2)
    ON_COMMAND(ID_LYRIC_DEFAULT_STYLE3, &CDesktopLyric::OnLyricDefaultStyle3)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEWHEEL()
	ON_WM_INITMENU()
END_MESSAGE_MAP()

void CDesktopLyric::Create()
{
	CLyricsWindow::Create(theApp.DPI(150));

    m_popupMenu.LoadMenu(IDR_DESKTOP_LYRIC_POPUP_MENU);

    //为右键添加图标
    CMenuIcon::AddIconToMenuItem(m_popupMenu.GetSafeHmenu(), ID_PLAY_PAUSE, FALSE, theApp.m_icon_set.play_pause);
    CMenuIcon::AddIconToMenuItem(m_popupMenu.GetSafeHmenu(), ID_STOP, FALSE, theApp.m_icon_set.stop_new);
    CMenuIcon::AddIconToMenuItem(m_popupMenu.GetSafeHmenu(), ID_PREVIOUS, FALSE, theApp.m_icon_set.previous_new.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_popupMenu.GetSafeHmenu(), ID_NEXT, FALSE, theApp.m_icon_set.next_new.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_popupMenu.GetSafeHmenu(), ID_PLAY_ORDER, FALSE, theApp.m_icon_set.play_oder.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_popupMenu.GetSafeHmenu(), ID_PLAY_SHUFFLE, FALSE, theApp.m_icon_set.play_shuffle.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_popupMenu.GetSafeHmenu(), ID_PLAY_RANDOM, FALSE, theApp.m_icon_set.play_random.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_popupMenu.GetSafeHmenu(), ID_LOOP_PLAYLIST, FALSE, theApp.m_icon_set.loop_playlist.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_popupMenu.GetSafeHmenu(), ID_LOOP_TRACK, FALSE, theApp.m_icon_set.loop_track.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_popupMenu.GetSafeHmenu(), ID_PLAY_TRACK, FALSE, theApp.m_icon_set.play_track.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_popupMenu.GetSafeHmenu(), ID_LYRIC_DISPLAYED_DOUBLE_LINE, FALSE, theApp.m_icon_set.double_line.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_popupMenu.GetSafeHmenu(), ID_LYRIC_BACKGROUND_PENETRATE, FALSE, theApp.m_icon_set.skin.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_popupMenu.GetSafeHmenu(), ID_LOCK_DESKTOP_LRYIC, FALSE, theApp.m_icon_set.lock.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_popupMenu.GetSubMenu(0)->GetSafeHmenu(), 10, TRUE, theApp.m_icon_set.media_lib.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_popupMenu.GetSafeHmenu(), ID_OPTION_SETTINGS, FALSE, theApp.m_icon_set.setting.GetIcon(true));
    CMenuIcon::AddIconToMenuItem(m_popupMenu.GetSafeHmenu(), ID_CLOSE_DESKTOP_LYRIC, FALSE, theApp.m_icon_set.close.GetIcon(true));

    //初始化提示信息
    m_tool_tip.Create(this, TTS_ALWAYSTIP);
    m_tool_tip.SetMaxTipWidth(theApp.DPI(400));

    //初始化定时器
    SetTimer(TIMER_DESKTOP_LYRIC, 200, NULL);

}

void CDesktopLyric::ShowLyric()
{
	if (!IsWindowVisible())
		return;

	if (CPlayerUIHelper::IsMidiLyric())
	{
		wstring current_lyric{ CPlayer::GetInstance().GetMidiLyric() };
		if (current_lyric != GetLyricStr().GetString())
		{
			UpdateLyrics(current_lyric.c_str(), 0);
		}
		else
		{
			UpdateLyrics(0);
		}
		UpdateLyricTranslate(_T(""));
		SetNextLyric(_T(""));
	}
	else if (!CPlayer::GetInstance().m_Lyrics.IsEmpty())
	{
		Time time{ CPlayer::GetInstance().GetCurrentPosition() };
		int progress = CPlayer::GetInstance().m_Lyrics.GetLyricProgress(time);
		CLyrics::Lyric lyric = CPlayer::GetInstance().m_Lyrics.GetLyric(time, 0);
		if (lyric.text.empty())
			lyric.text = CCommon::LoadText(IDS_DEFAULT_LYRIC_TEXT_CORTANA);

        SetLyricDoubleLine(theApp.m_lyric_setting_data.desktop_lyric_data.lyric_double_line);
        SetShowTranslate(theApp.m_lyric_setting_data.show_translate);
        SetAlignment(theApp.m_lyric_setting_data.desktop_lyric_data.lyric_align);
        SetLyricKaraokeDisplay(theApp.m_lyric_setting_data.lyric_karaoke_disp);
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
	SetLyricsFont(data.lyric_font.name.c_str(), theApp.DPI(data.lyric_font.size), CGdiPlusTool::ToGDIPluseFontStyle(data.lyric_font.style));
	SetLyricsColor(CGdiPlusTool::COLORREFToGdiplusColor(data.text_color1), CGdiPlusTool::COLORREFToGdiplusColor(data.text_color2), static_cast<LyricsGradientMode>(data.text_gradient));
	SetHighlightColor(CGdiPlusTool::COLORREFToGdiplusColor(data.highlight_color1), CGdiPlusTool::COLORREFToGdiplusColor(data.highlight_color2), static_cast<LyricsGradientMode>(data.highlight_gradient));
	//SetLyricWindowLock(data.lock_desktop_lyric);
    //SetLyricBackgroundPenetrate(data.lyric_background_penetrate);
    m_bLocked = data.lock_desktop_lyric;
    m_lyricBackgroundPenetrate = data.lyric_background_penetrate;
    SetTimer(TIMER_DESKTOP_LYRIC_2, 200, NULL);     //延迟200毫秒设置锁定歌词窗口和背景穿透
}

void CDesktopLyric::SetLyricWindowVisible(bool visible)
{
	ShowWindow(visible);
}

void CDesktopLyric::SetLyricWindowLock(bool locked)
{
    m_bLocked = locked;
    SetWindowStyle();
}

void CDesktopLyric::SetLyricOpacity(int opacity)
{
    SetAlpha(opacity * 255 / 100);
}

void CDesktopLyric::SetLyricBackgroundPenetrate(bool penetrate)
{
    m_lyricBackgroundPenetrate = penetrate;
    SetWindowStyle();
}

LyricStyleDefaultData CDesktopLyric::GetDefaultStyle(int index) const
{
    if (index < 0 || index >= LYRIC_DEFAULT_STYLE_NUM)
        index = 0;
    return m_default_style[index];
}

void CDesktopLyric::SetDefaultStyle(const LyricStyleDefaultData& style_data, int index)
{
    if (index >= 0 && index < LYRIC_DEFAULT_STYLE_NUM)
    {
        m_default_style[index] = style_data;
    }
}

void CDesktopLyric::RestoreDefaultStyle()
{
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
}

void CDesktopLyric::LyricStyleDefaultDataToLyricSettingData(const LyricStyleDefaultData& style_data, DesktopLyricSettingData& setting_data)
{
    setting_data.text_color1 = style_data.normal_style.color1;
    setting_data.text_color2 = style_data.normal_style.color2;
    setting_data.text_gradient = style_data.normal_style.gradient_mode;
    setting_data.highlight_color1 = style_data.highlight_style.color1;
    setting_data.highlight_color2 = style_data.highlight_style.color2;
    setting_data.highlight_gradient = style_data.highlight_style.gradient_mode;
}

void CDesktopLyric::LyricSettingDatatOLyricStyleDefaultData(const DesktopLyricSettingData& setting_data, LyricStyleDefaultData& style_data)
{
    style_data.normal_style.color1 = setting_data.text_color1;
    style_data.normal_style.color2 = setting_data.text_color2;
    style_data.normal_style.gradient_mode = setting_data.text_gradient;
    style_data.highlight_style.color1 = setting_data.highlight_color1;
    style_data.highlight_style.color2 = setting_data.highlight_color2;
    style_data.highlight_style.gradient_mode = setting_data.highlight_gradient;

}

void CDesktopLyric::LoadDefaultStyle(CIniHelper & ini)
{
    m_default_style[0].normal_style.color1 = ini.GetInt(L"desktop_lyric_default_style", L"default1_text_color1", RGB(37, 152, 10));
    m_default_style[0].normal_style.color2 = ini.GetInt(L"desktop_lyric_default_style", L"default1_text_color2", RGB(129, 249, 0));
    m_default_style[0].normal_style.gradient_mode = ini.GetInt(L"desktop_lyric_default_style", L"default1_text_gradient_mode", 1);
    m_default_style[0].highlight_style.color1 = ini.GetInt(L"desktop_lyric_default_style", L"default1_highlight_color1", RGB(253, 232, 0));
    m_default_style[0].highlight_style.color2 = ini.GetInt(L"desktop_lyric_default_style", L"default1_highlight_color2", RGB(255, 120, 0));
    m_default_style[0].highlight_style.gradient_mode = ini.GetInt(L"desktop_lyric_default_style", L"default1_highlight_gradient_mode", 2);

    m_default_style[1].normal_style.color1 = ini.GetInt(L"desktop_lyric_default_style", L"default2_text_color1", RGB(252, 82, 66));
    m_default_style[1].normal_style.color2 = ini.GetInt(L"desktop_lyric_default_style", L"default2_text_color2", RGB(255, 128, 0));
    m_default_style[1].normal_style.gradient_mode = ini.GetInt(L"desktop_lyric_default_style", L"default2_text_gradient_mode", 1);
    m_default_style[1].highlight_style.color1 = ini.GetInt(L"desktop_lyric_default_style", L"default2_highlight_color1", RGB(255, 255, 0));
    m_default_style[1].highlight_style.color2 = ini.GetInt(L"desktop_lyric_default_style", L"default2_highlight_color2", RGB(255, 192, 0));
    m_default_style[1].highlight_style.gradient_mode = ini.GetInt(L"desktop_lyric_default_style", L"default2_highlight_gradient_mode", 2);

    m_default_style[2].normal_style.color1 = ini.GetInt(L"desktop_lyric_default_style", L"default3_text_color1", RGB(210, 137, 255));
    m_default_style[2].normal_style.color2 = ini.GetInt(L"desktop_lyric_default_style", L"default3_text_color2", RGB(200, 227, 255));
    m_default_style[2].normal_style.gradient_mode = ini.GetInt(L"desktop_lyric_default_style", L"default3_text_gradient_mode", 1);
    m_default_style[2].highlight_style.color1 = ini.GetInt(L"desktop_lyric_default_style", L"default3_highlight_color1", RGB(98, 237, 245));
    m_default_style[2].highlight_style.color2 = ini.GetInt(L"desktop_lyric_default_style", L"default3_highlight_color2", RGB(74, 145, 253));
    m_default_style[2].highlight_style.gradient_mode = ini.GetInt(L"desktop_lyric_default_style", L"default3_highlight_gradient_mode", 2);
}

void CDesktopLyric::SaveDefaultStyle(CIniHelper & ini) const
{
    ini.WriteInt(L"desktop_lyric_default_style", L"default1_text_color1", m_default_style[0].normal_style.color1);
    ini.WriteInt(L"desktop_lyric_default_style", L"default1_text_color2", m_default_style[0].normal_style.color2);
    ini.WriteInt(L"desktop_lyric_default_style", L"default1_text_gradient_mode", m_default_style[0].normal_style.gradient_mode);
    ini.WriteInt(L"desktop_lyric_default_style", L"default1_highlight_color1", m_default_style[0].highlight_style.color1);
    ini.WriteInt(L"desktop_lyric_default_style", L"default1_highlight_color2", m_default_style[0].highlight_style.color2);
    ini.WriteInt(L"desktop_lyric_default_style", L"default1_highlight_gradient_mode", m_default_style[0].highlight_style.gradient_mode);

    ini.WriteInt(L"desktop_lyric_default_style", L"default2_text_color1", m_default_style[1].normal_style.color1);
    ini.WriteInt(L"desktop_lyric_default_style", L"default2_text_color2", m_default_style[1].normal_style.color2);
    ini.WriteInt(L"desktop_lyric_default_style", L"default2_text_gradient_mode", m_default_style[1].normal_style.gradient_mode);
    ini.WriteInt(L"desktop_lyric_default_style", L"default2_highlight_color1", m_default_style[1].highlight_style.color1);
    ini.WriteInt(L"desktop_lyric_default_style", L"default2_highlight_color2", m_default_style[1].highlight_style.color2);
    ini.WriteInt(L"desktop_lyric_default_style", L"default2_highlight_gradient_mode", m_default_style[1].highlight_style.gradient_mode);

    ini.WriteInt(L"desktop_lyric_default_style", L"default3_text_color1", m_default_style[2].normal_style.color1);
    ini.WriteInt(L"desktop_lyric_default_style", L"default3_text_color2", m_default_style[2].normal_style.color2);
    ini.WriteInt(L"desktop_lyric_default_style", L"default3_text_gradient_mode", m_default_style[2].normal_style.gradient_mode);
    ini.WriteInt(L"desktop_lyric_default_style", L"default3_highlight_color1", m_default_style[2].highlight_style.color1);
    ini.WriteInt(L"desktop_lyric_default_style", L"default3_highlight_color2", m_default_style[2].highlight_style.color2);
    ini.WriteInt(L"desktop_lyric_default_style", L"default3_highlight_gradient_mode", m_default_style[2].highlight_style.gradient_mode);
}

void CDesktopLyric::DrawToolbar(Gdiplus::Graphics* pGraphics)
{
    bool bLocked = theApp.m_lyric_setting_data.desktop_lyric_data.lock_desktop_lyric;
    const int toolbar_num = bLocked ? 1 : BtnKey::MAX;
    const int btn_size = m_toobar_height;
    int toolbar_width = toolbar_num * btn_size;
    Gdiplus::Rect toolbar_rect;
    toolbar_rect.Y = 0;
    toolbar_rect.X = (m_rcWindow.Width() - toolbar_width) / 2;
    toolbar_rect.Width = toolbar_width;
    toolbar_rect.Height = btn_size;

    //绘制背景
    if(!bLocked || theApp.m_lyric_setting_data.desktop_lyric_data.show_unlock_when_locked)
    {
        Gdiplus::Color back_color = CGdiPlusTool::COLORREFToGdiplusColor(theApp.m_app_setting_data.theme_color.light2, 180);
        if (!theApp.m_app_setting_data.button_round_corners)
        {
            Gdiplus::Brush* pBrush = new Gdiplus::SolidBrush(back_color);
            pGraphics->FillRectangle(pBrush, toolbar_rect);
            delete pBrush;
        }
        else
        {
            CDrawCommon drawer;
            drawer.Create(nullptr, pGraphics);
            drawer.DrawRoundRect(toolbar_rect, back_color, theApp.DPI(4));
        }
    }

    CRect rcIcon = CRect(toolbar_rect.X, toolbar_rect.Y, toolbar_rect.GetRight(), toolbar_rect.GetBottom());
    rcIcon.right = rcIcon.left + btn_size;

    if (!bLocked)
    {
        DrawToolIcon(pGraphics, theApp.m_icon_set.app, rcIcon, BTN_APP);
        rcIcon.MoveToX(rcIcon.right);
        DrawToolIcon(pGraphics, theApp.m_icon_set.stop, rcIcon, BTN_STOP);
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
        DrawToolIcon(pGraphics, theApp.m_icon_set.media_lib, rcIcon, BTN_DEFAULT_STYLE);
        rcIcon.MoveToX(rcIcon.right);
		DrawToolIcon(pGraphics, theApp.m_icon_set.lyric_delay, rcIcon, BTN_LYRIC_DELAY);
		rcIcon.MoveToX(rcIcon.right);
		DrawToolIcon(pGraphics, theApp.m_icon_set.lyric_forward, rcIcon, BTN_LYRIC_FORWARD);
        rcIcon.MoveToX(rcIcon.right);
        DrawToolIcon(pGraphics, theApp.m_icon_set.double_line, rcIcon, BTN_DOUBLE_LINE, theApp.m_lyric_setting_data.desktop_lyric_data.lyric_double_line);
        rcIcon.MoveToX(rcIcon.right);
        DrawToolIcon(pGraphics, theApp.m_icon_set.skin, rcIcon, BTN_BACKGROUND_PENETRATE, theApp.m_lyric_setting_data.desktop_lyric_data.lyric_background_penetrate);
        rcIcon.MoveToX(rcIcon.right);
        DrawToolIcon(pGraphics, theApp.m_icon_set.lock, rcIcon, BTN_LOCK);
        rcIcon.MoveToX(rcIcon.right);
        DrawToolIcon(pGraphics, theApp.m_icon_set.close, rcIcon, BTN_CLOSE);

		bool lyric_disable{ CPlayer::GetInstance().m_Lyrics.IsEmpty() || CPlayerUIHelper::IsMidiLyric() };
		m_buttons[BTN_LYRIC_FORWARD].enable = !lyric_disable;
		m_buttons[BTN_LYRIC_DELAY].enable = !lyric_disable;

    }
    else if (theApp.m_lyric_setting_data.desktop_lyric_data.show_unlock_when_locked)    //如果处于锁定状态，只绘制一个解锁图标
    {
        for (auto& btn : m_buttons)
        {
            if (btn.first == BTN_LOCK)
                DrawToolIcon(pGraphics, theApp.m_icon_set.lock, rcIcon, BTN_LOCK);
            else
                btn.second = UIButton();
        }
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

    if (btn.pressed && btn.enable)
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
        if (!theApp.m_app_setting_data.button_round_corners)
        {
            Gdiplus::SolidBrush brush(back_color);
            pGraphics->FillRectangle(&brush, rect.left, rect.top, rect.Width(), rect.Height());
        }
        else
        {
            CDrawCommon drawer;
            drawer.Create(nullptr, pGraphics);
            drawer.DrawRoundRect(CGdiPlusTool::CRectToGdiplusRect(rect), back_color, theApp.DPI(3));
        }
    }

    CRect rc_tmp = rect;
    //使图标在矩形中居中
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
    AddMouseToolTip(BTN_LYRIC_FORWARD, CCommon::LoadText(IDS_LYRIC_FORWARD));
    AddMouseToolTip(BTN_LYRIC_DELAY, CCommon::LoadText(IDS_LYRIC_DELAY));
    AddMouseToolTip(BTN_DEFAULT_STYLE, CCommon::LoadText(IDS_DEFAULT_STYLE));
    AddMouseToolTip(BTN_DOUBLE_LINE, CCommon::LoadText(IDS_LYRIC_DOUBLE_LINE));
    AddMouseToolTip(BTN_BACKGROUND_PENETRATE, CCommon::LoadText(IDS_LYRIC_BACKGROUND_PENETRATE));
    if(theApp.m_lyric_setting_data.desktop_lyric_data.lock_desktop_lyric)
        AddMouseToolTip(BTN_LOCK, CCommon::LoadText(IDS_ULOCK_DESKTOP_LYRIC));
    else
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

void CDesktopLyric::UpdateMouseToolTip(BtnKey btn, LPCTSTR str)
{
    m_tool_tip.UpdateTipText(str, this, btn + 1000);
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


void CDesktopLyric::PreDrawLyric(Gdiplus::Graphics* pGraphics)
{
    //绘制半透明背景
    if (!m_bLocked && !m_lyricBackgroundPenetrate)
    {
        BYTE alpha = (m_bHover) ? 80 : 1;
        Gdiplus::Brush* pBrush = new Gdiplus::SolidBrush(Gdiplus::Color(alpha, 255, 255, 255));
        pGraphics->FillRectangle(pBrush, 0, 0, m_rcWindow.Width(), m_rcWindow.Height());
        delete pBrush;
    }
}

void CDesktopLyric::AfterDrawLyric(Gdiplus::Graphics* pGraphics)
{
    //绘制工具条
    bool bLocked = theApp.m_lyric_setting_data.desktop_lyric_data.lock_desktop_lyric;
    if (m_lyricBackgroundPenetrate || m_bLocked ? m_bMouseInWindowRect : m_bHover)
    {
        DrawToolbar(pGraphics);

        if (m_first_draw)
        {
            AddToolTips();
            m_first_draw = false;
        }
    }
}

void CDesktopLyric::SetWindowStyle()
{
    if (m_bLocked)
        ModifyStyleEx(NULL, WS_EX_TRANSPARENT);
    else
        ModifyStyleEx(WS_EX_TRANSPARENT, NULL);

    if (m_bLocked || m_lyricBackgroundPenetrate)
        ModifyStyle(WS_THICKFRAME, NULL);
    else
        ModifyStyle(NULL, WS_THICKFRAME);
}

void CDesktopLyric::OnLButtonDown(UINT nFlags, CPoint point)
{
    CPoint point1 = point;
    point1.x += m_frameSize.cx;
    point1.y += m_frameSize.cy;

    bool point_in_btns = false;
    for (auto& btn : m_buttons)
    {
        if (btn.second.enable && btn.second.rect.PtInRect(point1) != FALSE)
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
    // TODO: 在此添加消息处理程序代码和/或调用默认值
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
                theApp.m_pMainWnd->SendMessage(WM_OPTION_SETTINGS, 0, 0);
                return;

            case BTN_LYRIC_FORWARD:
                theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_LYRIC_FORWARD, 0);
                return;

            case BTN_LYRIC_DELAY:
				theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_LYRIC_DELAY, 0);
				return;

            case BTN_DEFAULT_STYLE:
            {
                CPoint cur_point;
                cur_point.x = m_buttons[BTN_DEFAULT_STYLE].rect.left - m_frameSize.cx;
                cur_point.y = m_buttons[BTN_DEFAULT_STYLE].rect.bottom - m_frameSize.cy;
                ClientToScreen(&cur_point);
                m_bMenuPopedUp = true;
                CMenu* pMenu = theApp.m_menu_set.m_lyric_default_style.GetSubMenu(0);
                if (pMenu != nullptr)
                    pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, cur_point.x, cur_point.y, this);
                m_bMenuPopedUp = false;
            }
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
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    CPoint point1 = point;
    point1.x += m_frameSize.cx;
    point1.y += m_frameSize.cy;
    for (auto& btn : m_buttons)
    {
		if (btn.second.enable)
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
    // TODO: 在此添加消息处理程序代码和/或调用默认值
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
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    if (!m_bMenuPopedUp)
    {
        m_bHover = false;
        //Invalidate();
        for (auto& btn : m_buttons)
        {
            btn.second.pressed = false;
            btn.second.hover = false;
        }
    }

    CLyricsWindow::OnMouseLeave();
}


void CDesktopLyric::OnSizing(UINT fwSide, LPRECT pRect)
{
    CLyricsWindow::OnSizing(fwSide, pRect);

    // TODO: 在此处添加消息处理程序代码
    m_bHover = true;
    UpdateToolTipPosition();
}


void CDesktopLyric::OnRButtonUp(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    m_bMenuPopedUp = true;
    CPoint point1;		//定义一个用于确定光标位置的位置
    GetCursorPos(&point1);	//获取当前光标的位置，以便使得菜单可以跟随光标，该位置以屏幕左上角点为原点，point则以客户区左上角为原点
    CMenu* pMenu = m_popupMenu.GetSubMenu(0);
    if (pMenu != NULL)
        pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, this);
    m_bMenuPopedUp = false;
    //CLyricsWindow::OnRButtonUp(nFlags, point);
}


BOOL CDesktopLyric::OnCommand(WPARAM wParam, LPARAM lParam)
{
    // TODO: 在此添加专用代码和/或调用基类
    WORD command = LOWORD(wParam);
    if (CCommon::IsMenuItemInMenu(m_popupMenu.GetSubMenu(0), command) || CCommon::IsMenuItemInMenu(&theApp.m_menu_set.m_main_menu, command))
        AfxGetMainWnd()->SendMessage(WM_COMMAND, wParam, lParam);		//将菜单命令转发到主窗口

    return CLyricsWindow::OnCommand(wParam, lParam);
}


void CDesktopLyric::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    lpMMI->ptMinTrackSize.x = theApp.DPI(400);
    lpMMI->ptMinTrackSize.y = theApp.DPI(100);

    CLyricsWindow::OnGetMinMaxInfo(lpMMI);
}


//afx_msg LRESULT CDesktopLyric::OnInitmenu(WPARAM wParam, LPARAM lParam)
//{
//    AfxGetMainWnd()->SendMessage(WM_INITMENU, wParam, lParam);        //将WM_INITMENU消息转发到主窗口
//    return 0;
//}


BOOL CDesktopLyric::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if (pMsg->message == WM_MOUSEMOVE)
        m_tool_tip.RelayEvent(pMsg);

    return CLyricsWindow::PreTranslateMessage(pMsg);
}


void CDesktopLyric::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    if (nIDEvent == TIMER_DESKTOP_LYRIC)
    {
        CPoint point;
        GetCursorPos(&point);
        m_bMouseInWindowRect = m_rcWindow.PtInRect(point);

        bool bLocked = theApp.m_lyric_setting_data.desktop_lyric_data.lock_desktop_lyric;
        if (bLocked && theApp.m_lyric_setting_data.desktop_lyric_data.show_unlock_when_locked)        //处于锁定状态时，如果指针处于“锁定”按钮区域内，则取消鼠标穿透状态，以使得“锁定”按钮可以点击
        {
            CRect rcLockBtn = m_buttons[BTN_LOCK].rect;
            rcLockBtn.MoveToX(rcLockBtn.left + m_rcWindow.left);
            rcLockBtn.MoveToY(rcLockBtn.top + m_rcWindow.top);
            if (rcLockBtn.PtInRect(point))
            {
                SetWindowLong(GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong(GetSafeHwnd(), GWL_EXSTYLE) & (~WS_EX_TRANSPARENT));		//取消鼠标穿透
                m_buttons[BTN_LOCK].hover = true;
            }
            else
            {
                SetWindowLong(GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong(GetSafeHwnd(), GWL_EXSTYLE) | WS_EX_TRANSPARENT);		//设置鼠标穿透
                m_buttons[BTN_LOCK].hover = false;
            }
        }
    }
    else if (nIDEvent == TIMER_DESKTOP_LYRIC_2)
    {
        KillTimer(TIMER_DESKTOP_LYRIC_2);
        SetWindowStyle();
    }

    CLyricsWindow::OnTimer(nIDEvent);
}


void CDesktopLyric::OnLyricDefaultStyle1()
{
    // TODO: 在此添加命令处理程序代码
    auto style = GetDefaultStyle(0);
    LyricStyleDefaultDataToLyricSettingData(style, theApp.m_lyric_setting_data.desktop_lyric_data);
    ApplySettings(theApp.m_lyric_setting_data.desktop_lyric_data);
}


void CDesktopLyric::OnLyricDefaultStyle2()
{
    // TODO: 在此添加命令处理程序代码
    auto style = GetDefaultStyle(1);
    LyricStyleDefaultDataToLyricSettingData(style, theApp.m_lyric_setting_data.desktop_lyric_data);
    ApplySettings(theApp.m_lyric_setting_data.desktop_lyric_data);
}


void CDesktopLyric::OnLyricDefaultStyle3()
{
    // TODO: 在此添加命令处理程序代码
    auto style = GetDefaultStyle(2);
    LyricStyleDefaultDataToLyricSettingData(style, theApp.m_lyric_setting_data.desktop_lyric_data);
    ApplySettings(theApp.m_lyric_setting_data.desktop_lyric_data);
}


void CDesktopLyric::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_SHOW_MAIN_WINDOW);

	CLyricsWindow::OnLButtonDblClk(nFlags, point);
}


BOOL CDesktopLyric::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (zDelta > 0)
	{
		AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_VOLUME_UP);
	}
	if (zDelta < 0)
	{
		AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_VOLUME_DOWN);
	}

	return CLyricsWindow::OnMouseWheel(nFlags, zDelta, pt);
}


void CDesktopLyric::OnInitMenu(CMenu* pMenu)
{
	CLyricsWindow::OnInitMenu(pMenu);

	// TODO: 在此处添加消息处理程序代码
	CMusicPlayerDlg* pPlayerDlg = dynamic_cast<CMusicPlayerDlg*>(AfxGetMainWnd());
	if(pPlayerDlg != nullptr)
		pPlayerDlg->SetMenuState(pMenu);
	pMenu->SetDefaultItem(ID_SHOW_MAIN_WINDOW);
}
