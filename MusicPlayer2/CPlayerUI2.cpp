#include "stdafx.h"
#include "CPlayerUI2.h"


CPlayerUI2::CPlayerUI2(UIData& ui_data)
	:m_ui_data(ui_data)
{
	m_title_font.CreatePointFont(100, _T("微软雅黑"));
	m_artist_font.CreatePointFont(90, _T("微软雅黑"));
}


CPlayerUI2::~CPlayerUI2()
{
}

void CPlayerUI2::Init(CDC* pDC)
{
	m_pDC = pDC;
	m_draw.Create(m_pDC, theApp.m_pMainWnd);
	m_pLayout = std::make_shared<SLayoutData>();
}

void CPlayerUI2::DrawInfo(bool narrow_mode, bool reset)
{
	CPlayerUIBase::DrawInfo(narrow_mode, reset);

	//设置信息区域的矩形
	if (!narrow_mode)
	{
		m_draw_data.draw_rect = CRect{ CPoint{m_pLayout->margin, m_pLayout->control_bar_height + m_pLayout->margin}, 
		CPoint{m_ui_data.client_width / 2 - m_pLayout->margin, m_ui_data.client_height - m_pLayout->margin} };
	}
	else
	{
		m_draw_data.draw_rect = CRect{ CPoint{ m_pLayout->margin, m_pLayout->control_bar_height + m_pLayout->progress_bar_height},
		CSize{ m_ui_data.client_width - 2 * m_pLayout->margin, m_pLayout->info_height - 2 * m_pLayout->margin } };
	}

	//设置缓冲的DC
	CDC MemDC;
	CBitmap MemBitmap;
	MemDC.CreateCompatibleDC(NULL);

	CRect draw_rect = m_draw_data.draw_rect;
	draw_rect.MoveToXY(0, 0);

	MemBitmap.CreateCompatibleBitmap(m_pDC, m_draw_data.draw_rect.Width(), m_draw_data.draw_rect.Height());
	CBitmap *pOldBit = MemDC.SelectObject(&MemBitmap);
	m_draw.SetDC(&MemDC);	//将m_draw中的绘图DC设置为缓冲的DC
	m_draw.SetFont(theApp.m_pMainWnd->GetFont());

	//绘制背景
	if (theApp.m_app_setting_data.album_cover_as_background)
	{
		if (theApp.m_player.AlbumCoverExist())
		{
			CImage& back_image{ theApp.m_app_setting_data.background_gauss_blur ? theApp.m_player.GetAlbumCoverBlur() : theApp.m_player.GetAlbumCover() };
			m_draw.DrawBitmap(back_image, CPoint(0, 0), m_draw_data.draw_rect.Size(), CDrawCommon::StretchMode::FILL);
		}
		else
		{
			//MemDC.FillSolidRect(0, 0, m_draw_data.draw_rect.Width(), m_draw_data.draw_rect.Height(), GetSysColor(COLOR_BTNFACE));	//给缓冲DC的绘图区域填充对话框的背景颜色
			m_draw.DrawBitmap(m_ui_data.default_background, CPoint(0, 0), m_draw_data.draw_rect.Size(), CDrawCommon::StretchMode::FILL);
		}
	}

	//填充背景颜色
	bool draw_background{ theApp.m_app_setting_data.album_cover_as_background && (theApp.m_player.AlbumCoverExist() || !m_ui_data.default_background.IsNull()) };		//是否需要绘制图片背景
	if (draw_background)
		m_draw.FillAlphaRect(draw_rect, m_colors.color_back, ALPHA_CHG(m_colors.background_transparency));
	else
		m_draw.FillRect(draw_rect, m_colors.color_back);


	if (!narrow_mode)
	{
		wchar_t buff[64];

		//绘制播放状态
		int text_height{ theApp.DPI(18) };		//文本的高度
		CRect rc_tmp{ CPoint(m_pLayout->margin, m_pLayout->margin), CSize(theApp.DPI(52) ,text_height) };
		m_draw.DrawWindowText(rc_tmp, theApp.m_player.GetPlayingState().c_str(), m_colors.color_text_lable);

		//绘制歌曲序号
		rc_tmp.MoveToX(rc_tmp.right);
		rc_tmp.right = rc_tmp.left + theApp.DPI(30);
		swprintf_s(buff, sizeof(buff) / 2, L"%.3d", theApp.m_player.GetIndex() + 1);
		m_draw.DrawWindowText(rc_tmp, buff, m_colors.color_text_2);

		//绘制文件名
		rc_tmp.MoveToX(rc_tmp.right);
		rc_tmp.right = draw_rect.right - m_pLayout->margin;
		static CDrawCommon::ScrollInfo scroll_info1;
		m_draw.DrawScrollText(rc_tmp, theApp.m_player.GetFileName().c_str(), m_colors.color_text, theApp.DPI(1.5), false, scroll_info1, reset);

		rc_tmp.MoveToX(m_pLayout->margin);
		rc_tmp.MoveToY(rc_tmp.bottom);
		rc_tmp.right = draw_rect.right - m_pLayout->margin;
		const BASS_CHANNELINFO channel_info{ theApp.m_player.GetChannelInfo() };
		CString chans_str;
		if (channel_info.chans == 1)
			chans_str = _T("单声道");
		else if (channel_info.chans == 2)
			chans_str = _T("立体声");
		else if (channel_info.chans > 2)
			chans_str.Format(_T("%d声道"));
		if (!theApp.m_player.IsMidi())
			swprintf_s(buff, L"%s %.1fkHz %dkbps %s", theApp.m_player.GetCurrentFileType().c_str(), channel_info.freq / 1000.0f, theApp.m_player.GetCurrentSongInfo().bitrate, chans_str.GetString());
		else
			swprintf_s(buff, L"%s %.1fkHz %s", theApp.m_player.GetCurrentFileType().c_str(), channel_info.freq / 1000.0f, chans_str.GetString());

		static CDrawCommon::ScrollInfo scroll_info2;
		m_draw.DrawScrollText(rc_tmp, buff, m_colors.color_text, theApp.DPI(1.5), false, scroll_info2, reset);

		//计算专辑封面的位置
		int bottom_height = static_cast<int>(draw_rect.Height() * 0.4);
		CRect cover_frame_rect{ CPoint(0, text_height * 2), CSize(draw_rect.Width(), draw_rect.Height() - text_height * 2 - bottom_height) };
		int cover_side = min(cover_frame_rect.Width(), cover_frame_rect.Height());
		CPoint start_point;
		start_point.x = cover_frame_rect.left + (cover_frame_rect.Width() - cover_side) / 2;
		start_point.y = cover_frame_rect.top + (cover_frame_rect.Height() - cover_side) / 2;
		CRect cover_rect{ start_point, CSize(cover_side, cover_side) };
		cover_rect.DeflateRect(theApp.DPI(12), theApp.DPI(12));

		//绘制背景
		rc_tmp = cover_rect;
		rc_tmp.bottom += m_pLayout->margin / 2;
		if (draw_background)
			m_draw.FillAlphaRect(rc_tmp, m_colors.color_spectrum_back, ALPHA_CHG(m_colors.background_transparency) * 2 / 3);
		else
			m_draw.FillRect(rc_tmp, m_colors.color_spectrum_back);

		//绘制专辑封面
		cover_rect.DeflateRect(m_pLayout->margin / 2, m_pLayout->margin / 2);
		if (theApp.m_app_setting_data.show_album_cover && theApp.m_player.AlbumCoverExist())
		{
			m_draw.DrawBitmap(theApp.m_player.GetAlbumCover(), cover_rect.TopLeft(), cover_rect.Size(), theApp.m_app_setting_data.album_cover_fit);
		}

		//绘制播放进度
		CRect progress_rect = cover_rect;
		progress_rect.top = cover_rect.bottom;
		progress_rect.bottom = progress_rect.top + m_pLayout->margin / 2;
		double progress = static_cast<double>(theApp.m_player.GetCurrentPosition()) / theApp.m_player.GetSongLength();
		progress_rect.right = progress_rect.left + static_cast<int>(progress * cover_rect.Width());
		m_draw.FillRect(progress_rect, m_colors.color_spectrum);

		//绘制标题和艺术家
		int text_height2 = theApp.DPI(22);
		rc_tmp.MoveToXY(m_pLayout->margin, draw_rect.bottom - bottom_height);
		rc_tmp.right = draw_rect.right - m_pLayout->margin;
		rc_tmp.bottom = rc_tmp.top + text_height2;
		m_draw.SetFont(&m_title_font);
		static CDrawCommon::ScrollInfo scroll_info_title;
		m_draw.DrawScrollText(rc_tmp, theApp.m_player.GetCurrentSongInfo().title.c_str(), m_colors.color_text, theApp.DPI(1), true, scroll_info_title, reset);

		rc_tmp.MoveToY(rc_tmp.bottom);
		m_draw.SetFont(&m_artist_font);
		static CDrawCommon::ScrollInfo scroll_info_artist;
		m_draw.DrawScrollText(rc_tmp, theApp.m_player.GetCurrentSongInfo().artist.c_str(), m_colors.color_text, theApp.DPI(1), true, scroll_info_artist, reset);

		//绘制控制条
		rc_tmp.MoveToY(rc_tmp.bottom + theApp.DPI(4));
		rc_tmp.bottom = rc_tmp.top + theApp.DPI(24);
		DrawControlBar(draw_background, rc_tmp);

		//绘制歌词
		bool midi_lyric{ theApp.m_player.IsMidi() && theApp.m_general_setting_data.midi_use_inner_lyric && !theApp.m_player.MidiNoLyric() };

		rc_tmp.MoveToX(m_pLayout->margin);
		rc_tmp.MoveToY(rc_tmp.bottom + m_pLayout->margin);
		rc_tmp.right = draw_rect.right - m_pLayout->margin;
		rc_tmp.bottom = draw_rect.bottom - m_pLayout->margin;

		if (theApp.m_app_setting_data.lyric_background)
		{
			if (draw_background)
				m_draw.FillAlphaRect(rc_tmp, m_colors.color_lyric_back, ALPHA_CHG(m_colors.background_transparency) * 3 / 5);
			else
				m_draw.FillRect(rc_tmp, m_colors.color_lyric_back);
		}

		rc_tmp.DeflateRect(2 * m_pLayout->margin, 2 * m_pLayout->margin);
		CDrawCommon::SetDrawArea(m_draw.GetDC(), rc_tmp);

		DrawLyricTextMultiLine(rc_tmp, &m_ui_data.lyric_font, &m_ui_data.lyric_translate_font,
			m_colors.color_text, m_colors.color_text_2, m_ui_data.show_translate, midi_lyric);

	}

	//窄界面模式时
	else
	{
		//绘制专辑封面
		CRect rc_tmp = draw_rect;
		rc_tmp.DeflateRect(m_pLayout->margin, m_pLayout->margin);
		rc_tmp.right = rc_tmp.left + rc_tmp.Height();
		if (draw_background)
			m_draw.FillAlphaRect(rc_tmp, m_colors.color_spectrum_back, ALPHA_CHG(m_colors.background_transparency) * 2 / 3);
		else
			m_draw.FillRect(rc_tmp, m_colors.color_spectrum_back);

		rc_tmp.DeflateRect(m_pLayout->margin / 2, m_pLayout->margin / 2);
		if (theApp.m_app_setting_data.show_album_cover && theApp.m_player.AlbumCoverExist())
		{
			m_draw.DrawBitmap(theApp.m_player.GetAlbumCover(), rc_tmp.TopLeft(), rc_tmp.Size(), theApp.m_app_setting_data.album_cover_fit);
		}

		wchar_t buff[64];
		//绘制播放状态
		int text_height{ theApp.DPI(18) };		//文本的高度
		rc_tmp.MoveToX(draw_rect.Height());
		rc_tmp.MoveToY(m_pLayout->margin);
		rc_tmp.right = rc_tmp.left + theApp.DPI(52);
		rc_tmp.bottom = rc_tmp.top + text_height;
		m_draw.DrawWindowText(rc_tmp, theApp.m_player.GetPlayingState().c_str(), m_colors.color_text_lable);

		//绘制歌曲序号
		rc_tmp.MoveToX(rc_tmp.right);
		rc_tmp.right = rc_tmp.left + theApp.DPI(30);
		swprintf_s(buff, sizeof(buff) / 2, L"%.3d", theApp.m_player.GetIndex() + 1);
		m_draw.DrawWindowText(rc_tmp, buff, m_colors.color_text_2);

		//绘制文件名
		rc_tmp.MoveToX(rc_tmp.right);
		rc_tmp.right = draw_rect.right - m_pLayout->margin;
		static CDrawCommon::ScrollInfo scroll_info1;
		m_draw.DrawScrollText(rc_tmp, theApp.m_player.GetFileName().c_str(), m_colors.color_text, theApp.DPI(1.5), false, scroll_info1, reset);

		//绘制标题和艺术家
		int text_height2 = theApp.DPI(22);
		rc_tmp.MoveToXY(draw_rect.Height(), rc_tmp.bottom + theApp.DPI(4));
		rc_tmp.right = draw_rect.right - m_pLayout->margin;
		rc_tmp.bottom = rc_tmp.top + text_height2;
		m_draw.SetFont(&m_title_font);
		static CDrawCommon::ScrollInfo scroll_info_title;
		m_draw.DrawScrollText(rc_tmp, theApp.m_player.GetCurrentSongInfo().title.c_str(), m_colors.color_text, theApp.DPI(1), true, scroll_info_title, reset);

		rc_tmp.MoveToY(rc_tmp.bottom);
		m_draw.SetFont(&m_artist_font);
		static CDrawCommon::ScrollInfo scroll_info_artist;
		m_draw.DrawScrollText(rc_tmp, theApp.m_player.GetCurrentSongInfo().artist.c_str(), m_colors.color_text, theApp.DPI(1), true, scroll_info_artist, reset);

		//绘制控件条
		rc_tmp.MoveToY(rc_tmp.bottom + theApp.DPI(4));
		rc_tmp.bottom = rc_tmp.top + theApp.DPI(24);
		DrawControlBar(draw_background, rc_tmp);

		//绘制歌词
		bool midi_lyric{ theApp.m_player.IsMidi() && theApp.m_general_setting_data.midi_use_inner_lyric && !theApp.m_player.MidiNoLyric() };

		rc_tmp.MoveToY(rc_tmp.bottom + m_pLayout->margin);
		rc_tmp.bottom = draw_rect.bottom - m_pLayout->margin;

		if (theApp.m_app_setting_data.lyric_background)
		{
			if (draw_background)
				m_draw.FillAlphaRect(rc_tmp, m_colors.color_lyric_back, ALPHA_CHG(m_colors.background_transparency) * 3 / 5);
			else
				m_draw.FillRect(rc_tmp, m_colors.color_lyric_back);
		}

		rc_tmp.DeflateRect(m_pLayout->margin, m_pLayout->margin);
		if (midi_lyric)
		{
			wstring current_lyric{ theApp.m_player.GetMidiLyric() };
			m_draw.DrawWindowText(rc_tmp, current_lyric.c_str(), m_colors.color_text, Alignment::CENTER, false, true);
		}
		else if (theApp.m_player.m_Lyrics.IsEmpty())
		{
			m_draw.DrawWindowText(rc_tmp, _T("当前歌曲没有歌词"), m_colors.color_text_2, Alignment::CENTER);
		}
		else
		{
			CRect lyric_rect = rc_tmp;
			CLyrics::Lyric current_lyric{ theApp.m_player.m_Lyrics.GetLyric(Time(theApp.m_player.GetCurrentPosition()), 0) };	//获取当歌词
			if (current_lyric.text.empty())		//如果当前歌词为空白，就显示为省略号
				current_lyric.text = DEFAULT_LYRIC_TEXT;
			int progress{ theApp.m_player.m_Lyrics.GetLyricProgress(Time(theApp.m_player.GetCurrentPosition())) };		//获取当前歌词进度（范围为0~1000）

			if (m_ui_data.show_translate)
			{
				lyric_rect.bottom = lyric_rect.top + rc_tmp.Height() / 2;
				CRect translate_rect = lyric_rect;
				translate_rect.MoveToY(lyric_rect.bottom);

				m_draw.SetFont(&m_ui_data.lyric_translate_font);
				m_draw.DrawWindowText(translate_rect, current_lyric.translate.c_str(), m_colors.color_text, m_colors.color_text, progress, true, true);
			}

			m_draw.SetFont(&m_ui_data.lyric_font);
			if (theApp.m_lyric_setting_data.lyric_karaoke_disp)
				m_draw.DrawWindowText(lyric_rect, current_lyric.text.c_str(), m_colors.color_text, m_colors.color_text_2, progress, true, true);
			else
				m_draw.DrawWindowText(lyric_rect, current_lyric.text.c_str(), m_colors.color_text, m_colors.color_text, progress, true, true);
			m_draw.SetFont(theApp.m_pMainWnd->GetFont());
		}

		//CDrawCommon::SetDrawArea(m_draw.GetDC(), rc_tmp);

		//DrawLyricTextMultiLine(rc_tmp, &m_ui_data.lyric_font, &m_ui_data.lyric_translate_font,
		//	m_colors.color_text, m_colors.color_text_2, m_ui_data.show_translate, midi_lyric);

	}

	//将缓冲区DC中的图像拷贝到屏幕中显示
	m_pDC->BitBlt(m_draw_data.draw_rect.left, m_draw_data.draw_rect.top, m_draw_data.draw_rect.Width(), m_draw_data.draw_rect.Height(), &MemDC, 0, 0, SRCCOPY);
	MemDC.SelectObject(pOldBit);
	MemBitmap.DeleteObject();
	MemDC.DeleteDC();

}

void CPlayerUI2::RButtonUp(CPoint point, bool narrow_mode)
{
}

void CPlayerUI2::MouseMove(CPoint point)
{
}

void CPlayerUI2::LButtonUp(CPoint point)
{
}

void CPlayerUI2::OnSizeRedraw(int cx, int cy, bool narrow_mode)
{
	CRect redraw_rect{ m_draw_data.draw_rect };
	if (!narrow_mode)	//在普通界面模式下
	{
		if (cx < m_ui_data.client_width)	//如果界面宽度变窄了
		{
			//重新将绘图区域右侧区域的矩形区域填充为对话框背景色
			redraw_rect.left = cx / 2 - 3 * m_pLayout->margin;
			redraw_rect.right = m_ui_data.client_width / 2 + m_pLayout->margin;
			m_pDC->FillSolidRect(redraw_rect, GetSysColor(COLOR_BTNFACE));
		}
		else if (cy < m_ui_data.client_height)	//如果界面高度变小了
		{
			//重新将绘图区域下方区域的矩形区域填充为对话框背景色
			redraw_rect.top = cy - 2 * m_pLayout->margin;
			redraw_rect.bottom = cy;
			m_pDC->FillSolidRect(redraw_rect, GetSysColor(COLOR_BTNFACE));
		}
	}
	else if (narrow_mode && cx < m_ui_data.client_width)	//在窄界面模式下，如果宽度变窄了
	{
		//重新将绘图区域右侧区域的矩形区域填充为对话框背景色
		redraw_rect.left = cx - 2 * m_pLayout->margin;
		redraw_rect.right = cx;
		m_pDC->FillSolidRect(redraw_rect, GetSysColor(COLOR_BTNFACE));
	}
}

CRect CPlayerUI2::GetThumbnailClipArea(bool narrow_mode)
{
	return m_draw_data.draw_rect;
}

void CPlayerUI2::DrawControlBar(bool draw_background, CRect rect)
{
	CRect rc_tmp = rect;
	if (draw_background)
		m_draw.FillAlphaRect(rc_tmp, m_colors.color_control_bar_back, ALPHA_CHG(m_colors.background_transparency));
	else
		m_draw.FillRect(rc_tmp, m_colors.color_control_bar_back);

	//绘制循环模式
	rc_tmp.right = rc_tmp.left + theApp.DPI(112);
	rc_tmp.DeflateRect(theApp.DPI(4), 0);
	CString repeat_mode_str;
	switch (theApp.m_player.GetRepeatMode())
	{
	case RepeatMode::RM_PLAY_ORDER: repeat_mode_str += _T("顺序播放"); break;
	case RepeatMode::RM_LOOP_PLAYLIST: repeat_mode_str += _T("列表循环"); break;
	case RepeatMode::RM_LOOP_TRACK: repeat_mode_str += _T("单曲循环"); break;
	case RepeatMode::RM_PLAY_SHUFFLE: repeat_mode_str += _T("随机播放"); break;
	}
	m_draw.DrawWindowText(rc_tmp, repeat_mode_str, m_colors.color_text);

	//绘制音量
	wchar_t buff[16];
	rc_tmp.right = rect.right - m_pLayout->margin;
	rc_tmp.left = rc_tmp.right - theApp.DPI(70);
	swprintf_s(buff, L"音量：%d%%", theApp.m_player.GetVolume());
	m_draw.DrawWindowText(rc_tmp, buff, m_colors.color_text);
}


