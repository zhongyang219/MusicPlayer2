#include "stdafx.h"
#include "CPlayerUIBase.h"


CPlayerUIBase::CPlayerUIBase(UIData& ui_data)
	:m_ui_data(ui_data)
{
	m_popup_menu.LoadMenu(IDR_LYRIC_POPUP_MENU);	//装载歌词右键菜单
	m_main_popup_menu.LoadMenu(IDR_MAIN_POPUP_MENU);

	//m_font_time.CreatePointFont(80, CCommon::LoadText(IDS_DEFAULT_FONT));
}


CPlayerUIBase::~CPlayerUIBase()
{
}

void CPlayerUIBase::SetToolTip(CToolTipCtrl * pToolTip)
{
	m_tool_tip = pToolTip;
}

void CPlayerUIBase::Init(CDC * pDC)
{
	m_pDC = pDC;
	m_draw.Create(m_pDC, theApp.m_pMainWnd);
}

void CPlayerUIBase::DrawInfo(bool reset)
{
	if (m_first_draw)
	{
		AddToolTips();
	}
	m_first_draw = false;
}

void CPlayerUIBase::ClearInfo()
{
	PreDrawInfo();
	m_pDC->FillSolidRect(m_draw_rect, CONSTVAL::BACKGROUND_COLOR);
}

void CPlayerUIBase::LButtonDown(CPoint point)
{
	for (auto& btn : m_buttons)
	{
		if(btn.second.rect.PtInRect(point) != FALSE)
			btn.second.pressed = true;
	}
}

void CPlayerUIBase::RButtonUp(CPoint point)
{
	if (!m_draw_rect.PtInRect(point))
		return;

	if (m_buttons[BTN_VOLUME].rect.PtInRect(point) == FALSE)
		m_show_volume_adj = false;

	CPoint point1;		//定义一个用于确定光标位置的位置  
	GetCursorPos(&point1);	//获取当前光标的位置，以便使得菜单可以跟随光标，该位置以屏幕左上角点为原点，point则以客户区左上角为原点
	if (m_buttons[BTN_REPETEMODE].rect.PtInRect(point))		//如果在“循环模式”的矩形区域内点击鼠标右键，则弹出“循环模式”的子菜单
	{
		CMenu* pMenu = m_main_popup_menu.GetSubMenu(0)->GetSubMenu(1);
		if (pMenu != NULL)
			pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, theApp.m_pMainWnd);
		return;
	}

	for (auto& btn : m_buttons)
	{
		//按钮上点击右键不弹出菜单
		if (btn.first != BTN_COVER && btn.second.rect.PtInRect(point) != FALSE)
			return;
	}

	if (!m_draw_data.lyric_rect.PtInRect(point))	//如果在歌词区域点击了鼠标右键
	{
		m_main_popup_menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, theApp.m_pMainWnd);
	}
	else
	{
		m_popup_menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, theApp.m_pMainWnd);
	}

}

void CPlayerUIBase::MouseMove(CPoint point)
{
	for (auto& btn : m_buttons)
	{
		btn.second.hover = (btn.second.rect.PtInRect(point) != FALSE);
	}

	m_buttons[BTN_PROGRESS].hover = m_buttons[BTN_PROGRESS].hover && !(m_show_volume_adj && (m_buttons[BTN_VOLUME_UP].rect.PtInRect(point) || m_buttons[BTN_VOLUME_DOWN].rect.PtInRect(point)));

	//鼠标指向进度条时显示定位到几分几秒
	if (m_buttons[BTN_PROGRESS].hover)
	{
		__int64 song_pos;
		song_pos = static_cast<__int64>(point.x - m_buttons[BTN_PROGRESS].rect.left) * CPlayer::GetInstance().GetSongLength() / m_buttons[BTN_PROGRESS].rect.Width();
		Time song_pos_time;
		song_pos_time.int2time(static_cast<int>(song_pos));
		CString str;
		static int last_sec{};
		if (last_sec != song_pos_time.sec)		//只有鼠标指向位置对应的秒数变化了才更新鼠标提示
		{
			str.Format(CCommon::LoadText(IDS_SEEK_TO_MINUTE_SECOND), song_pos_time.min, song_pos_time.sec);
			UpdateMouseToolTip(BTN_PROGRESS, str);
			last_sec = song_pos_time.sec;
		}
	}

	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(tme);
	tme.hwndTrack = theApp.m_pMainWnd->GetSafeHwnd();
	tme.dwFlags = TME_LEAVE | TME_HOVER;
	tme.dwHoverTime = 1;
	_TrackMouseEvent(&tme);
}

void CPlayerUIBase::LButtonUp(CPoint point)
{
	if (!m_show_volume_adj)		//如果设有显示音量调整按钮，则点击音量区域就显示音量调整按钮
		m_show_volume_adj = (m_buttons[BTN_VOLUME].rect.PtInRect(point) != FALSE);
	else		//如果已经显示了音量调整按钮，则点击音量调整时保持音量调整按钮的显示
		m_show_volume_adj = (m_buttons[BTN_VOLUME_UP].rect.PtInRect(point) || m_buttons[BTN_VOLUME_DOWN].rect.PtInRect(point));

	for (auto& btn : m_buttons)
	{
		//if (btn.second.rect.PtInRect(point) != FALSE)
		//{
			//btn.second.hover = false;
			btn.second.pressed = false;
		//}

		if (btn.second.rect.PtInRect(point) && btn.second.enable)
		{
			switch (btn.first)
			{
			case BTN_REPETEMODE:
				CPlayer::GetInstance().SetRepeatMode();
				UpdateRepeatModeToolTip();
				return;

			case BTN_VOLUME:
				break;

			case BTN_TRANSLATE:
				m_ui_data.show_translate = !m_ui_data.show_translate;
				return;

			case BTN_SKIN:
				m_buttons[BTN_SKIN].hover = false;
				theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_SWITCH_UI);
				return;

			case BTN_EQ:
				m_buttons[BTN_EQ].hover = false;
				theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_EQUALIZER);
				return;

			case BTN_SETTING:
				m_buttons[BTN_SETTING].hover = false;
				theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_OPTION_SETTINGS);
				return;

			case BTN_MINI:
				m_buttons[BTN_MINI].hover = false;
				theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_MINI_MODE);
				return;

			case BTN_INFO:
				m_buttons[BTN_INFO].hover = false;
				theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_SONG_INFO);
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

			case BTN_SHOW_PLAYLIST:
				m_buttons[BTN_SHOW_PLAYLIST].hover = false;
				theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_SHOW_PLAYLIST);
				return;

			case BTN_SELECT_FOLDER:
				m_buttons[BTN_SELECT_FOLDER].hover = false;
				theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_SET_PATH);
				return;

			case BTN_VOLUME_UP:
				if (m_show_volume_adj)
				{
					CPlayer::GetInstance().MusicControl(Command::VOLUME_UP, theApp.m_nc_setting_data.volum_step);
					return;
				}
				break;

			case BTN_VOLUME_DOWN:
				if (m_show_volume_adj)
				{
					CPlayer::GetInstance().MusicControl(Command::VOLUME_DOWN, theApp.m_nc_setting_data.volum_step);
					return;
				}
				break;

			case BTN_PROGRESS:
			{
				int ckick_pos = point.x - m_buttons[BTN_PROGRESS].rect.left;
				double progress = static_cast<double>(ckick_pos) / m_buttons[BTN_PROGRESS].rect.Width();
				CPlayer::GetInstance().SeekTo(progress);
			}
			return;

			case BTN_FIND:
				m_buttons[BTN_FIND].hover = false;
				theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_FIND);
				return;
				break;

			case BTN_FULL_SCREEN:
				m_buttons[BTN_FULL_SCREEN].hover = false;
				theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_FULL_SCREEN);
				return;

			default:
				break;
			}
		}
	}

}

void CPlayerUIBase::OnSizeRedraw(int cx, int cy)
{
	CRect redraw_rect;
	if (!m_ui_data.narrow_mode)	//在普通界面模式下
	{
		if (cx < m_ui_data.client_width)	//如果界面宽度变窄了
		{
			//重新将绘图区域右侧区域的矩形区域填充为对话框背景色
			redraw_rect = m_draw_rect;
			if (m_ui_data.show_playlist)
			{
				redraw_rect.left = cx / 2/* - Margin()*/;
				redraw_rect.right = m_ui_data.client_width / 2 + Margin();
				m_pDC->FillSolidRect(redraw_rect, CONSTVAL::BACKGROUND_COLOR);
			}
			//else
			//{
			//	redraw_rect.left = cx - Margin();
			//	redraw_rect.right = cx;
			//}
		}
		//if (cy < m_ui_data.client_height)	//如果界面高度变小了
		//{
		//	//重新将绘图区域下方区域的矩形区域填充为对话框背景色
		//	redraw_rect = m_draw_rect;
		//	redraw_rect.top = cy - Margin();
		//	redraw_rect.bottom = cy;
		//	m_pDC->FillSolidRect(redraw_rect, CONSTVAL::BACKGROUND_COLOR);
		//}
	}
	else if (m_ui_data.narrow_mode)	//在窄界面模式下
	{
		//if (cx < m_ui_data.client_width)		//如果宽度变窄了
		//{
		//	//重新将绘图区域右侧区域的矩形区域填充为对话框背景色
		//	redraw_rect = m_draw_rect;
		//	redraw_rect.left = cx - Margin();
		//	redraw_rect.right = cx;
		//	m_pDC->FillSolidRect(redraw_rect, CONSTVAL::BACKGROUND_COLOR);
		//}
		//if (cy < m_ui_data.client_height)	//如果界面高度变小了
		//{
		//	if (!m_ui_data.show_playlist)
		//	{
		//		//重新将绘图区域下方区域的矩形区域填充为对话框背景色
		//		redraw_rect = m_draw_rect;
		//		redraw_rect.top = cy - Margin();
		//		redraw_rect.bottom = cy;
		//		m_pDC->FillSolidRect(redraw_rect, CONSTVAL::BACKGROUND_COLOR);
		//	}
		//}
	}
}

CRect CPlayerUIBase::GetThumbnailClipArea()
{
	int menu_bar_height = (m_ui_data.show_menu_bar ? DPI(20) : 0);
	CRect thumbnail_rect = DrawAreaToClient(m_draw_data.thumbnail_rect, m_draw_rect);
	thumbnail_rect.MoveToY(thumbnail_rect.top + menu_bar_height);
	return thumbnail_rect;
}

void CPlayerUIBase::UpdateRepeatModeToolTip()
{
	SetRepeatModeToolTipText();
	UpdateMouseToolTip(BTN_REPETEMODE, m_repeat_mode_tip);
}

void CPlayerUIBase::UpdateSongInfoToolTip()
{
	SetSongInfoToolTipText();
	UpdateMouseToolTip(BTN_INFO, m_info_tip);

	SetCoverToolTipText();
	UpdateMouseToolTip(BTN_COVER, m_cover_tip);
}

void CPlayerUIBase::UpdatePlayPauseButtonTip()
{
	if (CPlayer::GetInstance().IsPlaying() && !CPlayer::GetInstance().IsError())
		UpdateMouseToolTip(BTN_PLAY_PAUSE, CCommon::LoadText(IDS_PAUSE));
	else
		UpdateMouseToolTip(BTN_PLAY_PAUSE, CCommon::LoadText(IDS_PLAY));
}

void CPlayerUIBase::UpdateFullScreenTip()
{
	if (m_ui_data.full_screen)
		UpdateMouseToolTip(BTN_FULL_SCREEN, CCommon::LoadText(IDS_EXIT_FULL_SCREEN));
	else
		UpdateMouseToolTip(BTN_FULL_SCREEN, CCommon::LoadText(IDS_FULL_SCREEN));
}

bool CPlayerUIBase::SetCursor()
{
	if (m_buttons[BTN_PROGRESS].hover)
	{
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(32649)));
		return true;
	}
	return false;
}

void CPlayerUIBase::MouseLeave()
{
	for (auto& btn : m_buttons)
	{
		btn.second.hover = false;
	}
}

void CPlayerUIBase::ClearBtnRect()
{
	for (auto& btn : m_buttons)
	{
		btn.second.rect = CRect();
	}
}

void CPlayerUIBase::PreDrawInfo()
{
	//设置颜色
	m_colors = CPlayerUIHelper::GetUIColors(theApp.m_app_setting_data.theme_color);

	//if (m_repeat_mode_tip.IsEmpty())
	SetRepeatModeToolTipText();
	SetSongInfoToolTipText();

	//计算文本高度
	m_pDC->SelectObject(&theApp.m_font_set.lyric.GetFont(theApp.m_ui_data.full_screen));
	m_lyric_text_height = m_pDC->GetTextExtent(L"文").cy;	//根据当前的字体设置计算文本的高度

	//设置绘制的矩形区域
	SetDrawRect();
}

void CPlayerUIBase::SetDrawRect()
{
	if (!m_ui_data.show_playlist)
	{
		m_draw_rect = CRect(0, 0, m_ui_data.client_width, m_ui_data.client_height);
		//m_draw_rect.DeflateRect(Margin(), Margin());
	}
	else
	{
		if (!m_ui_data.narrow_mode)
		{
			m_draw_rect = CRect{ /*CPoint{Margin(), Margin()}*/ CPoint(),
			CPoint{m_ui_data.client_width / 2 /* - Margin()*/, m_ui_data.client_height} };
		}
		else
		{
			m_draw_rect = CRect{ /*CPoint{ Margin(), Margin() }*/ CPoint(),
			CSize{ m_ui_data.client_width /*- 2 * Margin()*/, m_layout.info_height - Margin() } };
		}

	}
}

void CPlayerUIBase::DrawBackground()
{
	CRect draw_rect = m_draw_rect;
	draw_rect.MoveToXY(0, 0);

	//绘制背景
	if (theApp.m_app_setting_data.album_cover_as_background)
	{
		if (CPlayer::GetInstance().AlbumCoverExist())
		{
			CImage& back_image{ theApp.m_app_setting_data.background_gauss_blur ? CPlayer::GetInstance().GetAlbumCoverBlur() : CPlayer::GetInstance().GetAlbumCover() };
			m_draw.DrawBitmap(back_image, CPoint(0, 0), m_draw_rect.Size(), CDrawCommon::StretchMode::FILL);
		}
		else
		{
			//MemDC.FillSolidRect(0, 0, m_draw_rect.Width(), m_draw_rect.Height(), GetSysColor(COLOR_BTNFACE));	//给缓冲DC的绘图区域填充对话框的背景颜色
			m_draw.DrawBitmap(m_ui_data.default_background, CPoint(0, 0), m_draw_rect.Size(), CDrawCommon::StretchMode::FILL);
		}
	}

	//填充背景颜色
	if (IsDrawBackgroundAlpha())
		m_draw.FillAlphaRect(draw_rect, m_colors.color_back, ALPHA_CHG(theApp.m_app_setting_data.background_transparency));
	else
		m_draw.FillRect(draw_rect, m_colors.color_back);
}

void CPlayerUIBase::DrawLryicCommon(CRect rect)
{
	CDrawCommon::SetDrawArea(m_draw.GetDC(), rect);

	if (rect.Height() < static_cast<int>(m_lyric_text_height*3.5))
		DrawLyricTextSingleLine(rect);
	else
		DrawLyricTextMultiLine(rect);
}

void CPlayerUIBase::DrawLyricTextMultiLine(CRect lyric_area)
{
	int line_space = theApp.m_app_setting_data.lyric_line_space;
	if (m_ui_data.full_screen)
		line_space = static_cast<int>(line_space * CONSTVAL::FULL_SCREEN_ZOOM_FACTOR);

	int lyric_height = m_lyric_text_height + line_space;			//文本高度加上行间距
	int lyric_height2 = lyric_height * 2 + line_space;		//包含翻译的歌词高度

	m_draw.SetFont(&theApp.m_font_set.lyric.GetFont(theApp.m_ui_data.full_screen));
	if (IsMidiLyric())
	{
		wstring current_lyric{ CPlayer::GetInstance().GetMidiLyric() };
		m_draw.DrawWindowText(lyric_area, current_lyric.c_str(), m_colors.color_text, Alignment::CENTER, false, true);
	}
	else if (CPlayer::GetInstance().m_Lyrics.IsEmpty())
	{
		m_draw.DrawWindowText(lyric_area, CCommon::LoadText(IDS_NO_LYRIC_INFO), m_colors.color_text_2, Alignment::CENTER);
	}
	else
	{
		//CRect arect{ lyric_area };		//一行歌词的矩形区域
		//arect.bottom = arect.top + lyric_height;
		//vector<CRect> rects(CPlayer::GetInstance().m_Lyrics.GetLyricCount() + 1, arect);
		//为每一句歌词创建一个矩形，保存在容器里
		vector<CRect> rects;
		int lyric_count = CPlayer::GetInstance().m_Lyrics.GetLyricCount() + 1;		//获取歌词数量（由于第一行歌词需要显示标题，所以这里要+1）
		for (int i{}; i < lyric_count; i++)
		{
			CRect arect{ lyric_area };
			if (!CPlayer::GetInstance().m_Lyrics.GetLyric(i).translate.empty() && m_ui_data.show_translate)
				arect.bottom = arect.top + lyric_height2;
			else
				arect.bottom = arect.top + lyric_height;
			rects.push_back(arect);
		}
		int center_pos = (lyric_area.top + lyric_area.bottom) / 2;		//歌词区域的中心y坐标
		Time time{ CPlayer::GetInstance().GetCurrentPosition() };		//当前播放时间
		int lyric_index = CPlayer::GetInstance().m_Lyrics.GetLyricIndex(time) + 1;		//当前歌词的序号（歌词的第一句GetLyricIndex返回的是0，由于显示时第一句歌词要显示标题，所以这里要+1）
		int progress = CPlayer::GetInstance().m_Lyrics.GetLyricProgress(time);		//当前歌词进度（范围为0~1000）
		int y_progress;			//当前歌词在y轴上的进度
		if (!CPlayer::GetInstance().m_Lyrics.GetLyric(lyric_index).translate.empty() && m_ui_data.show_translate)
			y_progress = progress * lyric_height2 / 1000;
		else
			y_progress = progress * lyric_height / 1000;
		//int start_pos = center_pos - y_progress - (lyric_index + 1)*lyric_height;		//第1句歌词的起始y坐标
		//计算第1句歌词的起始y坐标
		//由于当前歌词需要显示在歌词区域的中心位置，因此从中心位置开始，减去当前歌词在Y轴上的进度
		//再依次减去之前每一句歌词的高度，即得到了第一句歌词的起始位置
		int start_pos;
		start_pos = center_pos - y_progress;
		for (int i{ lyric_index - 1 }; i >= 0; i--)
		{
			if (!CPlayer::GetInstance().m_Lyrics.GetLyric(i).translate.empty() && m_ui_data.show_translate)
				start_pos -= lyric_height2;
			else
				start_pos -= lyric_height;
		}

		//依次绘制每一句歌词
		for (int i{}; i < rects.size(); i++)
		{
			//计算每一句歌词的位置
			if (i == 0)
				rects[i].MoveToY(start_pos);
			else
				rects[i].MoveToY(rects[i - 1].bottom);
			//绘制歌词文本
			if (!(rects[i] & lyric_area).IsRectEmpty())		//只有当一句歌词的矩形区域和歌词区域的矩形有交集时，才绘制歌词
			{
				//设置歌词文本和翻译文本的矩形区域
				CRect rect_text{ rects[i] };
				CRect rect_translate;
				if (!CPlayer::GetInstance().m_Lyrics.GetLyric(i).translate.empty() && m_ui_data.show_translate)
				{
					rect_text.MoveToY(rect_text.top + line_space);
					rect_text.bottom = rect_text.top + m_lyric_text_height;
					rect_translate = rect_text;
					rect_translate.MoveToY(rect_text.bottom + line_space);
				}

				if (i == lyric_index)		//绘制正在播放的歌词
				{
					//绘制歌词文本
					m_draw.SetFont(&theApp.m_font_set.lyric.GetFont(theApp.m_ui_data.full_screen));
					if (theApp.m_lyric_setting_data.lyric_karaoke_disp)
						m_draw.DrawWindowText(rect_text, CPlayer::GetInstance().m_Lyrics.GetLyric(i).text.c_str(), m_colors.color_text, m_colors.color_text_2, progress, true, true);
					else
						m_draw.DrawWindowText(rect_text, CPlayer::GetInstance().m_Lyrics.GetLyric(i).text.c_str(), m_colors.color_text, m_colors.color_text, progress, true, true);
					//绘制翻译文本
					if (!CPlayer::GetInstance().m_Lyrics.GetLyric(i).translate.empty() && m_ui_data.show_translate)
					{
						m_draw.SetFont(&theApp.m_font_set.lyric_translate.GetFont(theApp.m_ui_data.full_screen));
						m_draw.DrawWindowText(rect_translate, CPlayer::GetInstance().m_Lyrics.GetLyric(i).translate.c_str(), m_colors.color_text, m_colors.color_text, progress, true, true);
					}
				}
				else		//绘制非正在播放的歌词
				{
					//绘制歌词文本
					m_draw.SetFont(&theApp.m_font_set.lyric.GetFont(theApp.m_ui_data.full_screen));
					m_draw.DrawWindowText(rect_text, CPlayer::GetInstance().m_Lyrics.GetLyric(i).text.c_str(), m_colors.color_text_2, Alignment::CENTER, true);
					//绘制翻译文本
					if (!CPlayer::GetInstance().m_Lyrics.GetLyric(i).translate.empty() && m_ui_data.show_translate)
					{
						m_draw.SetFont(&theApp.m_font_set.lyric_translate.GetFont(theApp.m_ui_data.full_screen));
						m_draw.DrawWindowText(rect_translate, CPlayer::GetInstance().m_Lyrics.GetLyric(i).translate.c_str(), m_colors.color_text_2, Alignment::CENTER, true);
					}
				}
			}
		}
	}

}

void CPlayerUIBase::DrawLyricTextSingleLine(CRect rect)
{
	m_draw.SetFont(&theApp.m_font_set.lyric.GetFont(theApp.m_ui_data.full_screen));
	if (IsMidiLyric())
	{
		wstring current_lyric{ CPlayer::GetInstance().GetMidiLyric() };
		m_draw.DrawWindowText(rect, current_lyric.c_str(), m_colors.color_text, Alignment::CENTER, false, true);
	}
	else if (CPlayer::GetInstance().m_Lyrics.IsEmpty())
	{
		m_draw.DrawWindowText(rect, CCommon::LoadText(IDS_NO_LYRIC_INFO), m_colors.color_text_2, Alignment::CENTER);
	}
	else
	{
		CRect lyric_rect = rect;
		CLyrics::Lyric current_lyric{ CPlayer::GetInstance().m_Lyrics.GetLyric(Time(CPlayer::GetInstance().GetCurrentPosition()), 0) };	//获取当歌词
		if (current_lyric.text.empty())		//如果当前歌词为空白，就显示为省略号
			current_lyric.text = CCommon::LoadText(IDS_DEFAULT_LYRIC_TEXT);
		int progress{ CPlayer::GetInstance().m_Lyrics.GetLyricProgress(Time(CPlayer::GetInstance().GetCurrentPosition())) };		//获取当前歌词进度（范围为0~1000）

		if ((!CPlayer::GetInstance().m_Lyrics.IsTranslated() || !m_ui_data.show_translate) && rect.Height() > static_cast<int>(m_lyric_text_height*1.73))
		{
			wstring next_lyric_text = CPlayer::GetInstance().m_Lyrics.GetLyric(Time(CPlayer::GetInstance().GetCurrentPosition()), 1).text;
			if (next_lyric_text.empty())
				next_lyric_text = CCommon::LoadText(IDS_DEFAULT_LYRIC_TEXT);
			DrawLyricDoubleLine(lyric_rect, current_lyric.text.c_str(), next_lyric_text.c_str(), progress);
		}
		else
		{
			if (m_ui_data.show_translate && !current_lyric.translate.empty() && rect.Height() > static_cast<int>(m_lyric_text_height*1.73))
			{
				lyric_rect.bottom = lyric_rect.top + rect.Height() / 2;
				CRect translate_rect = lyric_rect;
				translate_rect.MoveToY(lyric_rect.bottom);

				m_draw.SetFont(&theApp.m_font_set.lyric_translate.GetFont(theApp.m_ui_data.full_screen));
				m_draw.DrawWindowText(translate_rect, current_lyric.translate.c_str(), m_colors.color_text, m_colors.color_text, progress, true, true);
			}

			m_draw.SetFont(&theApp.m_font_set.lyric.GetFont(theApp.m_ui_data.full_screen));
			if (theApp.m_lyric_setting_data.lyric_karaoke_disp)
				m_draw.DrawWindowText(lyric_rect, current_lyric.text.c_str(), m_colors.color_text, m_colors.color_text_2, progress, true, true);
			else
				m_draw.DrawWindowText(lyric_rect, current_lyric.text.c_str(), m_colors.color_text, m_colors.color_text, progress, true, true);
		}

		m_draw.SetFont(theApp.m_pMainWnd->GetFont());
	}

}

void CPlayerUIBase::DrawSongInfo(CRect rect, bool reset)
{
	wchar_t buff[64];
	if (CPlayer::GetInstance().m_loading)
	{
		static CDrawCommon::ScrollInfo scroll_info0;
		CString info;
		info = CCommon::LoadTextFormat(IDS_PLAYLIST_INIT_INFO, { CPlayer::GetInstance().GetSongNum(), CPlayer::GetInstance().m_thread_info.process_percent });
		m_draw.DrawScrollText(rect, info, m_colors.color_text, DPI(1.5), false, scroll_info0, reset);
	}
	else
	{
		//绘制播放状态
		CString play_state_str = CPlayer::GetInstance().GetPlayingState().c_str();
		CRect rc_tmp{ rect };
		//m_draw.GetDC()->SelectObject(theApp.m_pMainWnd->GetFont());
		rc_tmp.right = rc_tmp.left + m_draw.GetTextExtent(play_state_str).cx + DPI(4);
		m_draw.DrawWindowText(rc_tmp, play_state_str, m_colors.color_text_lable);

		//绘制歌曲序号
		rc_tmp.MoveToX(rc_tmp.right);
		rc_tmp.right = rc_tmp.left + DPI(30);
		swprintf_s(buff, sizeof(buff) / 2, L"%.3d", CPlayer::GetInstance().GetIndex() + 1);
		m_draw.DrawWindowText(rc_tmp, buff, m_colors.color_text_2);

		//绘制文件名
		rc_tmp.MoveToX(rc_tmp.right);
		rc_tmp.right = rect.right;
		static CDrawCommon::ScrollInfo scroll_info1;
		m_draw.DrawScrollText(rc_tmp, CPlayer::GetInstance().GetFileName().c_str(), m_colors.color_text, DPI(1.5), false, scroll_info1, reset);
	}
}

void CPlayerUIBase::DrawToolBar(CRect rect, bool draw_translate_button)
{
	bool draw_background{ IsDrawBackgroundAlpha() };
	//绘制背景
	BYTE alpha;
	if (theApp.m_app_setting_data.dark_mode)
		alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency) * 2 / 3;
	else
		alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency);

	if (draw_background)
		m_draw.FillAlphaRect(rect, m_colors.color_control_bar_back, alpha);
	else
		m_draw.FillRect(rect, m_colors.color_control_bar_back);

	CRect rc_tmp = rect;

	//绘制循环模式
	rc_tmp.right = rect.left + rect.Height();
	IconRes* pIcon = nullptr;
	switch (CPlayer::GetInstance().GetRepeatMode())
	{
	case RepeatMode::RM_PLAY_ORDER:
		pIcon = &theApp.m_icon_set.play_oder;
		break;
	case RepeatMode::RM_LOOP_PLAYLIST:
		pIcon = &theApp.m_icon_set.loop_playlist;
		break;
	case RepeatMode::RM_LOOP_TRACK:
		pIcon = &theApp.m_icon_set.loop_track;
		break;
	case RepeatMode::RM_PLAY_SHUFFLE:
		pIcon = &theApp.m_icon_set.play_shuffle;
		break;
	}
	if (pIcon != nullptr)
		DrawUIButton(rc_tmp, m_buttons[BTN_REPETEMODE], *pIcon);

	//绘制设置按钮
	rc_tmp.MoveToX(rc_tmp.right);
	DrawUIButton(rc_tmp, m_buttons[BTN_SETTING], theApp.m_icon_set.setting);

	//绘制均衡器按钮
	rc_tmp.MoveToX(rc_tmp.right);
	DrawUIButton(rc_tmp, m_buttons[BTN_EQ], theApp.m_icon_set.eq);

	//绘制切换界面按钮
	rc_tmp.MoveToX(rc_tmp.right);
	DrawUIButton(rc_tmp, m_buttons[BTN_SKIN], theApp.m_icon_set.skin);

	//绘制迷你模式按钮
	if (rect.Width() >= DPI(190))
	{
		rc_tmp.MoveToX(rc_tmp.right);
		DrawUIButton(rc_tmp, m_buttons[BTN_MINI], theApp.m_icon_set.mini);
	}
	else
	{
		m_buttons[BTN_MINI].rect = CRect();
	}

	//绘制曲目信息按钮
	if (rect.Width() >= DPI(214))
	{
		rc_tmp.MoveToX(rc_tmp.right);
		DrawUIButton(rc_tmp, m_buttons[BTN_INFO], theApp.m_icon_set.info);
	}
	else
	{
		m_buttons[BTN_INFO].rect = CRect();
	}

	//绘制查找按钮
	if (rect.Width() >= DPI(238))
	{
		rc_tmp.MoveToX(rc_tmp.right);
		DrawUIButton(rc_tmp, m_buttons[BTN_FIND], theApp.m_icon_set.find_songs);
	}
	else
	{
		m_buttons[BTN_FIND].rect = CRect();
	}


	//绘制翻译按钮
	if (draw_translate_button && rect.Width() >= DPI(262))
	{
		rc_tmp.MoveToX(rc_tmp.right);
		CRect translate_rect = rc_tmp;
		translate_rect.DeflateRect(DPI(2), DPI(2));
		DrawTranslateButton(translate_rect);
	}
	else
	{
		m_buttons[BTN_TRANSLATE].rect = CRect();
	}

	rc_tmp.left = rc_tmp.right = rect.right;

	//显示<<<<
	if (rect.Width() >= DPI(313))
	{
		int progress;
		Time time{ CPlayer::GetInstance().GetCurrentPosition() };
		if (CPlayer::GetInstance().IsMidi())
		{
			////progress = (CPlayer::GetInstance().GetMidiInfo().midi_position % 16 + 1) *1000 / 16;
			//if (CPlayer::GetInstance().GetMidiInfo().tempo == 0)
			//	progress = 0;
			//else
			//	progress = (time.time2int() * 1000 / CPlayer::GetInstance().GetMidiInfo().tempo % 4 + 1) * 250;
			progress = (CPlayer::GetInstance().GetMidiInfo().midi_position % 4 + 1) * 250;
		}
		else
		{
			progress = (time.sec % 4 * 1000 + time.msec) / 4;
		}
		rc_tmp.right = rc_tmp.left;
		rc_tmp.left = rc_tmp.right - DPI(44);
		m_draw.DrawWindowText(rc_tmp, _T("<<<<"), m_colors.color_text, m_colors.color_text_2, progress, false);
	}


	//显示音量
	wchar_t buff[64];
	rc_tmp.right = rc_tmp.left;
	rc_tmp.left = rc_tmp.right - DPI(72);
	swprintf_s(buff, CCommon::LoadText(IDS_VOLUME, _T(": %d%%")), CPlayer::GetInstance().GetVolume());
	if (m_buttons[BTN_VOLUME].hover)		//鼠标指向音量区域时，以另外一种颜色显示
		m_draw.DrawWindowText(rc_tmp, buff, m_colors.color_text_heighlight);
	else
		m_draw.DrawWindowText(rc_tmp, buff, m_colors.color_text);
	//设置音量调整按钮的位置
	m_buttons[BTN_VOLUME].rect = DrawAreaToClient(rc_tmp, m_draw_rect);
	m_buttons[BTN_VOLUME].rect.DeflateRect(0, DPI(4));
	m_buttons[BTN_VOLUME].rect.right -= DPI(12);
	m_buttons[BTN_VOLUME_DOWN].rect = m_buttons[BTN_VOLUME].rect;
	m_buttons[BTN_VOLUME_DOWN].rect.bottom += DPI(4);
	m_buttons[BTN_VOLUME_DOWN].rect.MoveToY(m_buttons[BTN_VOLUME].rect.bottom);
	m_buttons[BTN_VOLUME_DOWN].rect.right = m_buttons[BTN_VOLUME].rect.left + m_buttons[BTN_VOLUME].rect.Width() / 2;
	m_buttons[BTN_VOLUME_UP].rect = m_buttons[BTN_VOLUME_DOWN].rect;
	m_buttons[BTN_VOLUME_UP].rect.MoveToX(m_buttons[BTN_VOLUME_DOWN].rect.right);


}

CRect CPlayerUIBase::DrawAreaToClient(CRect rect, CRect draw_area)
{
	//rect.MoveToXY(rect.left + draw_area.left, rect.top + draw_area.top);
	return rect;
}

CRect CPlayerUIBase::ClientAreaToDraw(CRect rect, CRect draw_area)
{
	//rect.MoveToXY(rect.left - draw_area.left, rect.top - draw_area.top);
	return rect;
}

void CPlayerUIBase::DrawLyricDoubleLine(CRect rect, LPCTSTR lyric, LPCTSTR next_lyric, int progress)
{
	m_draw.SetFont(&theApp.m_font_set.lyric.GetFont(theApp.m_ui_data.full_screen));
	static bool swap;
	static int last_progress;
	if (last_progress > progress)		//如果当前的歌词进度比上次的小，说明歌词切换到了下一句
	{
		swap = !swap;
	}
	last_progress = progress;


	CRect up_rect{ rect }, down_rect{ rect };		//上半部分和下半部分歌词的矩形区域
	up_rect.bottom = up_rect.top + (up_rect.Height() / 2);
	down_rect.top = down_rect.bottom - (down_rect.Height() / 2);
	//根据下一句歌词的文本计算需要的宽度，从而实现下一行歌词右对齐
	//m_draw.GetDC()->SelectObject(&theApp.m_font_set.lyric.GetFont(theApp.m_ui_data.full_screen));
	int width;
	if (!swap)
		width = m_draw.GetTextExtent(next_lyric).cx;
	else
		width = m_draw.GetTextExtent(lyric).cx;
	if (width < rect.Width())
		down_rect.left = down_rect.right - width;

	COLORREF color1, color2;
	if (theApp.m_lyric_setting_data.lyric_karaoke_disp)
	{
		color1 = m_colors.color_text;
		color2 = m_colors.color_text_2;
	}
	else
	{
		color1 = color2 = m_colors.color_text;
	}

	if (!swap)
	{
		m_draw.DrawWindowText(up_rect, lyric, color1, color2, progress, false);
		m_draw.DrawWindowText(down_rect, next_lyric, m_colors.color_text_2);
	}
	else
	{
		m_draw.DrawWindowText(up_rect, next_lyric, m_colors.color_text_2);
		m_draw.DrawWindowText(down_rect, lyric, color1, color2, progress, false);
	}

}

void CPlayerUIBase::DrawUIButton(CRect rect, UIButton & btn, const IconRes& icon)
{
	CRect rc_tmp = rect;
	rc_tmp.DeflateRect(DPI(2), DPI(2));
	m_draw.SetDrawArea(rc_tmp);

	BYTE alpha;
	if (IsDrawBackgroundAlpha())
		alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency) * 2 / 3;
	else
		alpha = 255;
	if(btn.pressed && btn.hover)
		m_draw.FillAlphaRect(rc_tmp, m_colors.color_button_pressed, alpha);
	else if (btn.hover)
		m_draw.FillAlphaRect(rc_tmp, m_colors.color_text_2, alpha);

	//else if (!theApp.m_app_setting_data.dark_mode)
	//	m_draw.FillAlphaRect(rc_tmp, m_colors.color_button_back, alpha);

	btn.rect = DrawAreaToClient(rc_tmp, m_draw_rect);

	rc_tmp = rect;
	rc_tmp.DeflateRect(DPI(4), DPI(4));
	const HICON& hIcon = icon.GetIcon(!theApp.m_app_setting_data.dark_mode, theApp.m_ui_data.full_screen);
	m_draw.DrawIcon(hIcon, rc_tmp.TopLeft(), rc_tmp.Size());

}

void CPlayerUIBase::DrawControlButton(CRect rect, UIButton & btn, const IconRes & icon)
{
	CRect rc_tmp = rect;
	m_draw.SetDrawArea(rc_tmp);

	BYTE alpha;
	if (IsDrawBackgroundAlpha())
		alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency) * 2 / 3;
	else
		alpha = 255;
	if (btn.pressed && btn.hover)
		m_draw.FillAlphaRect(rc_tmp, m_colors.color_button_pressed, alpha);
	else if (btn.hover)
		m_draw.FillAlphaRect(rc_tmp, m_colors.color_text_2, alpha);

	//else if (!theApp.m_app_setting_data.dark_mode)
	//	m_draw.FillAlphaRect(rc_tmp, m_colors.color_button_back, alpha);

	btn.rect = DrawAreaToClient(rc_tmp, m_draw_rect);

	rc_tmp = rect;
	//使图标在矩形中居中
	CSize icon_size = icon.GetSize(theApp.m_ui_data.full_screen);
	rc_tmp.left = rect.left + (rect.Width() - icon_size.cx) / 2;
	rc_tmp.right = rc_tmp.left + icon_size.cx;
	rc_tmp.top = rect.top + (rect.Height() - icon_size.cy) / 2;
	rc_tmp.bottom = rc_tmp.top + icon_size.cy;

	const HICON& hIcon = icon.GetIcon(!theApp.m_app_setting_data.dark_mode, theApp.m_ui_data.full_screen);
	m_draw.DrawIcon(hIcon, rc_tmp.TopLeft(), rc_tmp.Size());
}

void CPlayerUIBase::SetRepeatModeToolTipText()
{
	m_repeat_mode_tip = CCommon::LoadText(IDS_REPEAT_MODE, _T(" (M): "));
	switch (CPlayer::GetInstance().GetRepeatMode())
	{
	case RepeatMode::RM_PLAY_ORDER:
		m_repeat_mode_tip += CCommon::LoadText(IDS_PLAY_ODER);
		break;
	case RepeatMode::RM_LOOP_PLAYLIST:
		m_repeat_mode_tip += CCommon::LoadText(IDS_LOOP_PLAYLIST);
		break;
	case RepeatMode::RM_LOOP_TRACK:
		m_repeat_mode_tip += CCommon::LoadText(IDS_LOOP_TRACK);
		break;
	case RepeatMode::RM_PLAY_SHUFFLE:
		m_repeat_mode_tip += CCommon::LoadText(IDS_PLAY_SHUFFLE);
		break;
	}
}

void CPlayerUIBase::SetSongInfoToolTipText()
{
	const SongInfo& songInfo = CPlayer::GetInstance().GetCurrentSongInfo();
	
	m_info_tip = CCommon::LoadText(IDS_SONG_INFO, _T(" (Ctrl+N)\r\n"));

	m_info_tip += CCommon::LoadText(IDS_TITLE, _T(": "));
	m_info_tip += songInfo.title.c_str();
	m_info_tip += _T("\r\n");

	m_info_tip += CCommon::LoadText(IDS_ARTIST, _T(": "));
	m_info_tip += songInfo.artist.c_str();
	m_info_tip += _T("\r\n");

	m_info_tip += CCommon::LoadText(IDS_ALBUM, _T(": "));
	m_info_tip += songInfo.album.c_str();
	//m_info_tip += _T("\r\n");

	//m_info_tip += CCommon::LoadText(IDS_BITRATE, _T(": "));
	//CString strTmp;
	//strTmp.Format(_T("%d kbps"), songInfo.bitrate);
	//m_info_tip += strTmp;
}

void CPlayerUIBase::SetCoverToolTipText()
{
	if (theApp.m_nc_setting_data.show_cover_tip && theApp.m_app_setting_data.show_album_cover && CPlayer::GetInstance().AlbumCoverExist())
	{
		m_cover_tip = CCommon::LoadText(IDS_ALBUM_COVER, _T(": "));

		if (CPlayer::GetInstance().IsInnerCover())
		{
			m_cover_tip += CCommon::LoadText(IDS_INNER_ALBUM_COVER_TIP_INFO);
			switch (CPlayer::GetInstance().GetAlbumCoverType())
			{
			case 0: m_cover_tip += _T("jpg"); break;
			case 1: m_cover_tip += _T("png"); break;
			case 2: m_cover_tip += _T("gif"); break;
			}
		}
		else
		{
			m_cover_tip += CCommon::LoadText(IDS_OUT_IMAGE, _T("\r\n"));
			m_cover_tip += CPlayer::GetInstance().GetAlbumCoverPath().c_str();
		}
	}
	else
	{
		m_cover_tip.Empty();
	}
}

bool CPlayerUIBase::IsMidiLyric()
{
	return CPlayer::GetInstance().IsMidi() && theApp.m_general_setting_data.midi_use_inner_lyric && !CPlayer::GetInstance().MidiNoLyric();
}

int CPlayerUIBase::Margin() const
{
	int margin = m_layout.margin;
	if(m_ui_data.full_screen)
		margin = static_cast<int>(margin * CONSTVAL::FULL_SCREEN_ZOOM_FACTOR * 1.5);

	return margin;
}

int CPlayerUIBase::EdgeMargin() const
{
	if (m_ui_data.full_screen)
		return theApp.DPI(40);
	else
		return m_layout.margin;
}

int CPlayerUIBase::WidthThreshold() const
{
	int width = m_layout.width_threshold;
	if (m_ui_data.full_screen)
		width = static_cast<int>(width * CONSTVAL::FULL_SCREEN_ZOOM_FACTOR);

	return width;
}

bool CPlayerUIBase::IsDrawBackgroundAlpha() const
{
	return theApp.m_app_setting_data.album_cover_as_background && (CPlayer::GetInstance().AlbumCoverExist() || !m_ui_data.default_background.IsNull());
}

int CPlayerUIBase::DPI(int pixel)
{
	if (m_ui_data.full_screen)
		return static_cast<int>(theApp.DPI(pixel) * CONSTVAL::FULL_SCREEN_ZOOM_FACTOR);
	else
		return theApp.DPI(pixel);
}

int CPlayerUIBase::DPI(double pixel)
{
	if (m_ui_data.full_screen)
		return static_cast<int>(theApp.DPI(pixel) * CONSTVAL::FULL_SCREEN_ZOOM_FACTOR);
	else
		return theApp.DPI(pixel);
}

bool CPlayerUIBase::IsDrawNarrowMode()
{
	if (!m_ui_data.show_playlist)
		return false;
	else
		return m_ui_data.narrow_mode;
}

void CPlayerUIBase::DrawVolumnAdjBtn()
{
	if (m_show_volume_adj)
	{
		CRect volume_down_rect = ClientAreaToDraw(m_buttons[BTN_VOLUME_DOWN].rect, m_draw_rect);
		CRect volume_up_rect = ClientAreaToDraw(m_buttons[BTN_VOLUME_UP].rect, m_draw_rect);

		BYTE alpha;
		if (IsDrawBackgroundAlpha())
			alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency);
		else
			alpha = 255;

		COLORREF btn_up_back_color, btn_down_back_color;

		if (m_buttons[BTN_VOLUME_UP].pressed && m_buttons[BTN_VOLUME_UP].hover)
			btn_up_back_color = m_colors.color_button_pressed;
		//else if (m_buttons[BTN_VOLUME_UP].hover)
		//	btn_up_back_color = m_colors.color_control_bar_back;
		else
			btn_up_back_color = m_colors.color_text_2;

		if (m_buttons[BTN_VOLUME_DOWN].pressed && m_buttons[BTN_VOLUME_DOWN].hover)
			btn_down_back_color = m_colors.color_button_pressed;
		//else if (m_buttons[BTN_VOLUME_DOWN].hover)
		//	btn_down_back_color = m_colors.color_control_bar_back;
		else
			btn_down_back_color = m_colors.color_text_2;


		m_draw.FillAlphaRect(volume_up_rect, btn_up_back_color, alpha);
		m_draw.FillAlphaRect(volume_down_rect, btn_down_back_color, alpha);

		m_draw.DrawWindowText(volume_down_rect, L"-", ColorTable::WHITE, Alignment::CENTER);
		m_draw.DrawWindowText(volume_up_rect, L"+", ColorTable::WHITE, Alignment::CENTER);
	}
}

void CPlayerUIBase::DrawControlBar(CRect rect)
{
	bool progress_on_top = rect.Width() < m_progress_on_top_threshold;
	const int progress_height = DPI(4);
	CRect progress_rect;
	if (progress_on_top)
	{
		progress_rect = rect;
		int progressbar_height = rect.Height() / 3;
		progress_rect.bottom = progress_rect.top + progressbar_height;
		DrawProgressBar(progress_rect);
		rect.top = progress_rect.bottom;
	}

	//绘制播放控制按钮
	const int btn_width = DPI(36);
	const int btn_height = min(rect.Height(), btn_width);

	CRect rc_btn{ CPoint(rect.left, rect.top + (rect.Height() - btn_height) / 2), CSize(btn_width, btn_height) };
	DrawControlButton(rc_btn, m_buttons[BTN_STOP], theApp.m_icon_set.stop_l);

	rc_btn.MoveToX(rc_btn.right);
	DrawControlButton(rc_btn, m_buttons[BTN_PREVIOUS], theApp.m_icon_set.previous_l);

	rc_btn.MoveToX(rc_btn.right);
	IconRes& paly_pause_icon = CPlayer::GetInstance().IsPlaying() ? theApp.m_icon_set.pause_l : theApp.m_icon_set.play_l;
	DrawControlButton(rc_btn, m_buttons[BTN_PLAY_PAUSE], paly_pause_icon);

	rc_btn.MoveToX(rc_btn.right);
	DrawControlButton(rc_btn, m_buttons[BTN_NEXT], theApp.m_icon_set.next_l);

	int progressbar_left = rc_btn.right + Margin();

	//绘制右侧按钮
	const int btn_side = DPI(24);
	rc_btn.right = rect.right;
	rc_btn.left = rc_btn.right - btn_side;
	rc_btn.top = rect.top + (rect.Height() - btn_side) / 2;
	rc_btn.bottom = rc_btn.top + btn_side;
	DrawUIButton(rc_btn, m_buttons[BTN_SHOW_PLAYLIST], theApp.m_icon_set.show_playlist);

	rc_btn.MoveToX(rc_btn.left - btn_side);
	DrawUIButton(rc_btn, m_buttons[BTN_SELECT_FOLDER], theApp.m_icon_set.select_folder);

	if (!progress_on_top)
	{
		progress_rect = rect;
		progress_rect.left = progressbar_left;
		progress_rect.right = rc_btn.left - Margin();
		DrawProgressBar(progress_rect);
	}
}

void CPlayerUIBase::DrawProgressBar(CRect rect)
{
	//绘制播放时间
	CRect rc_time = rect;
	wstring strTime = CPlayer::GetInstance().GetTimeString();

	m_draw.SetFont(&theApp.m_font_set.time.GetFont(m_ui_data.full_screen));
	CSize strSize = m_draw.GetTextExtent(strTime.c_str());
	rc_time.left = rc_time.right - strSize.cx;
	//rc_time.InflateRect(0, DPI(2));
	rc_time.top -= DPI(1);
	m_draw.DrawWindowText(rc_time, strTime.c_str(), m_colors.color_text);

	//绘制进度条
	const int progress_height = DPI(4);
	CRect progress_rect = rect;
	progress_rect.right = rc_time.left - Margin();
	progress_rect.top = rect.top + (rect.Height() - progress_height) / 2;
	progress_rect.bottom = progress_rect.top + progress_height;

	if (IsDrawBackgroundAlpha())
		m_draw.FillAlphaRect(progress_rect, m_colors.color_spectrum_back, ALPHA_CHG(theApp.m_app_setting_data.background_transparency) * 2 / 3);
	else
		m_draw.FillRect(progress_rect, m_colors.color_spectrum_back);

	m_buttons[BTN_PROGRESS].rect = DrawAreaToClient(progress_rect, m_draw_rect);
	m_buttons[BTN_PROGRESS].rect.InflateRect(0, DPI(3));

	double progress = static_cast<double>(CPlayer::GetInstance().GetCurrentPosition()) / CPlayer::GetInstance().GetSongLength();
	progress_rect.right = progress_rect.left + static_cast<int>(progress * progress_rect.Width());
	if (progress_rect.right > progress_rect.left)
		m_draw.FillRect(progress_rect, m_colors.color_spectrum);
}

void CPlayerUIBase::DrawTranslateButton(CRect rect)
{
	m_buttons[BTN_TRANSLATE].enable = CPlayer::GetInstance().m_Lyrics.IsTranslated();
	if (m_buttons[BTN_TRANSLATE].enable)
	{
		BYTE alpha;
		if (IsDrawBackgroundAlpha())
			alpha = ALPHA_CHG(theApp.m_app_setting_data.background_transparency) * 2 / 3;
		else
			alpha = 255;
		if(m_buttons[BTN_TRANSLATE].pressed)
			m_draw.FillAlphaRect(rect, m_colors.color_button_pressed, alpha);
		else if (m_buttons[BTN_TRANSLATE].hover)
			m_draw.FillAlphaRect(rect, m_colors.color_text_2, alpha);
		else if (m_ui_data.show_translate)
			m_draw.FillAlphaRect(rect, m_colors.color_button_back, alpha);
		m_draw.DrawWindowText(rect, CCommon::LoadText(IDS_TRAS), m_colors.color_text, Alignment::CENTER);
	}
	else
	{
		m_draw.DrawWindowText(rect, CCommon::LoadText(IDS_TRAS), GRAY(200), Alignment::CENTER);
	}
	m_buttons[BTN_TRANSLATE].rect = DrawAreaToClient(rect, m_draw_rect);
}

void CPlayerUIBase::DrawCurrentTime()
{
	wchar_t buff[64];
	CRect rc_tmp;
	SYSTEMTIME curTime;
	GetLocalTime(&curTime);
	std::swprintf(buff, 64, L"%d:%.2d", curTime.wHour, curTime.wMinute);
	CSize size = m_draw.GetTextExtent(buff);
	rc_tmp.top = theApp.DPI(4);
	rc_tmp.right = m_draw_rect.right - theApp.DPI(4);
	rc_tmp.bottom = rc_tmp.top + size.cy;
	rc_tmp.left = rc_tmp.right - size.cx;
	m_draw.SetFont(&theApp.m_font_set.time.GetFont(m_ui_data.full_screen));
	m_draw.DrawWindowText(rc_tmp, buff, m_colors.color_text);
	m_draw.SetFont(&theApp.m_font_set.normal.GetFont(theApp.m_ui_data.full_screen));
}

//void CPlayerUIBase::AddMouseToolTip(BtnKey btn, LPCTSTR str)
//{
//	m_tool_tip->AddTool(theApp.m_pMainWnd, str, m_buttons[btn].rect, btn + 1);
//}
//
//void CPlayerUIBase::UpdateMouseToolTip(BtnKey btn, LPCTSTR str)
//{
//	m_tool_tip->UpdateTipText(str, theApp.m_pMainWnd, btn + 1);
//}

void CPlayerUIBase::AddToolTips()
{
	AddMouseToolTip(BTN_REPETEMODE, m_repeat_mode_tip);
	AddMouseToolTip(BTN_TRANSLATE, CCommon::LoadText(IDS_SHOW_LYRIC_TRANSLATION));
	AddMouseToolTip(BTN_VOLUME, CCommon::LoadText(IDS_MOUSE_WHEEL_ADJUST_VOLUME));
	AddMouseToolTip(BTN_SKIN, CCommon::LoadText(IDS_SWITCH_UI));
	AddMouseToolTip(BTN_EQ, CCommon::LoadText(IDS_SOUND_EFFECT_SETTING, _T(" (Ctrl+E)")));
	AddMouseToolTip(BTN_SETTING, CCommon::LoadText(IDS_SETTINGS, _T(" (Ctrl+I)")));
	AddMouseToolTip(BTN_MINI, CCommon::LoadText(IDS_MINI_MODE, _T(" (Ctrl+M)")));
	AddMouseToolTip(BTN_INFO, m_info_tip);
	AddMouseToolTip(BTN_STOP, CCommon::LoadText(IDS_STOP));
	AddMouseToolTip(BTN_PREVIOUS, CCommon::LoadText(IDS_PREVIOUS));
	AddMouseToolTip(BTN_PLAY_PAUSE, CPlayer::GetInstance().IsPlaying() ? CCommon::LoadText(IDS_PAUSE) : CCommon::LoadText(IDS_PLAY));
	AddMouseToolTip(BTN_NEXT, CCommon::LoadText(IDS_NEXT));
	AddMouseToolTip(BTN_PROGRESS, CCommon::LoadText(IDS_SEEK_TO));
	AddMouseToolTip(BTN_SHOW_PLAYLIST, CCommon::LoadText(IDS_SHOW_HIDE_PLAYLIST));
	AddMouseToolTip(BTN_SELECT_FOLDER, CCommon::LoadText(IDS_SELECT_FOLDER, _T(" (Ctrl+T)")));
	AddMouseToolTip(BTN_FIND, CCommon::LoadText(IDS_FIND_SONGS, _T(" (Ctrl+F)")));
	AddMouseToolTip(BTN_COVER, m_cover_tip);
	AddMouseToolTip(BTN_FULL_SCREEN, CCommon::LoadText(IDS_FULL_SCREEN));
}

