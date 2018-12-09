#include "stdafx.h"
#include "CPlayerUI.h"


CPlayerUI::CPlayerUI(UIData& ui_data)
	:m_ui_data(ui_data)
{
}


CPlayerUI::~CPlayerUI()
{
}

void CPlayerUI::Init(CDC* pDC)
{
	m_pDC = pDC;
	m_draw.Create(m_pDC, theApp.m_pMainWnd);
	m_pLayout = std::make_shared<SLayoutData>();
}

void CPlayerUI::DrawInfo(bool narrow_mode, bool reset)
{
	//调用基类的函数，以设置绘图颜色
	CPlayerUIBase::DrawInfo(narrow_mode, reset);

	//设置信息区域的矩形
	CRect info_rect;
	if (!narrow_mode)
		info_rect = CRect{ CPoint{m_pLayout->margin, m_pLayout->control_bar_height + m_pLayout->margin}, CSize{m_ui_data.client_width / 2 - 2 * m_pLayout->margin, m_pLayout->info_height2 - 3 * m_pLayout->margin } };
	else
		info_rect = CRect{ CPoint{ m_pLayout->margin, m_pLayout->control_bar_height + m_pLayout->progress_bar_height}, CSize{ m_ui_data.client_width - 2 * m_pLayout->margin, m_pLayout->info_height - 2 * m_pLayout->margin } };

	//设置缓冲的DC
	CDC MemDC;
	CBitmap MemBitmap;
	MemDC.CreateCompatibleDC(NULL);
	m_draw_data.draw_rect = info_rect;		//绘图区域
	if (!narrow_mode)
		m_draw_data.draw_rect.bottom = m_ui_data.client_height - m_pLayout->margin;
	CRect draw_rect{ m_draw_data.draw_rect };
	draw_rect.MoveToXY(0, 0);
	MemBitmap.CreateCompatibleBitmap(m_pDC, m_draw_data.draw_rect.Width(), m_draw_data.draw_rect.Height());
	CBitmap *pOldBit = MemDC.SelectObject(&MemBitmap);
	m_draw.SetDC(&MemDC);	//将m_draw中的绘图DC设置为缓冲的DC
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
	else
	{
		m_draw.FillRect(draw_rect, m_colors.color_back);
	}

	//由于设置了缓冲绘图区域，m_draw_data.draw_rect的左上角点变成了绘图的原点
	info_rect.MoveToXY(0, 0);

	//填充背景颜色
	//CDrawCommon::SetDrawArea(&MemDC, info_rect);
	bool draw_background{ theApp.m_app_setting_data.album_cover_as_background && (theApp.m_player.AlbumCoverExist() || !m_ui_data.default_background.IsNull()) };		//是否需要绘制图片背景
	if (draw_background)
		m_draw.FillAlphaRect(draw_rect, m_colors.color_back, ALPHA_CHG(m_colors.background_transparency));
	else
		m_draw.FillRect(draw_rect, m_colors.color_back);

	//if (!narrow_mode)
	//{
	//	CRect gap_rect{ info_rect };
	//	gap_rect.top = info_rect.bottom;
	//	gap_rect.bottom = gap_rect.top + 2*m_pLayout->margin;
	//	//CDrawCommon::SetDrawArea(&MemDC, gap_rect);
	//	//MemDC.FillSolidRect(gap_rect, GetSysColor(COLOR_BTNFACE));
	//	m_draw.FillAlphaRect(gap_rect, color_back, ALPHA_CHG(m_colors.background_transparency));
	//}

	CPoint text_start{ info_rect.left + m_pLayout->spectral_size.cx + 2 * m_pLayout->margin, info_rect.top + m_pLayout->margin };		//文本的起始坐标
	int text_height{ theApp.DPI(18) };		//文本的高度

	//显示歌曲信息
	m_draw.SetFont(theApp.m_pMainWnd->GetFont());
	//m_draw.SetBackColor(color_back);
	CRect tmp{ text_start, CSize{1,text_height} };
	wchar_t buff[64];
	if (theApp.m_player.m_loading)
	{
		tmp.right = info_rect.right - m_pLayout->margin;
		static CDrawCommon::ScrollInfo scroll_info0;
		CString info;
		info.Format(_T("找到%d首歌曲，正在读取音频文件信息，已完成%d%%，请稍候……"), theApp.m_player.GetSongNum(), theApp.m_player.m_thread_info.process_percent);
		m_draw.DrawScrollText(tmp, info, m_colors.color_text, theApp.DPI(1.5), false, scroll_info0, reset);
	}
	else
	{
		//显示正在播放的文件名
		tmp.MoveToX(text_start.x + theApp.DPI(82));
		tmp.right = info_rect.right - m_pLayout->margin;
		static CDrawCommon::ScrollInfo scroll_info1;
		m_draw.DrawScrollText(tmp, theApp.m_player.GetFileName().c_str(), m_colors.color_text, theApp.DPI(1.5), false, scroll_info1, reset);
		//显示正在播放的曲目序号
		tmp.MoveToX(text_start.x + theApp.DPI(52));
		tmp.right = tmp.left + theApp.DPI(30);
		swprintf_s(buff, sizeof(buff) / 2, L"%.3d", theApp.m_player.GetIndex() + 1);
		m_draw.DrawWindowText(tmp, buff, m_colors.color_text_2);
		//显示播放状态
		tmp.MoveToX(text_start.x);
		tmp.right = tmp.left + theApp.DPI(52);
		m_draw.DrawWindowText(tmp, theApp.m_player.GetPlayingState().c_str(), m_colors.color_text_lable);
	}

	wstring lable1_str, lable1_content;
	wstring lable2_str, lable2_content;
	wstring lable3_str, lable3_content;
	wstring lable4_str, lable4_content;
	lable1_str = _T("标题：");
	lable1_content = theApp.m_player.GetCurrentSongInfo().title;
	if (theApp.m_player.IsMidi())
	{
		const MidiInfo& midi_info{ theApp.m_player.GetMidiInfo() };
		lable2_str = _T("节拍：");
		swprintf_s(buff, L"%d/%d (%dbpm)", midi_info.midi_position, midi_info.midi_length, midi_info.speed);
		lable2_content = buff;

		//lable3_str = _T("速度：");
		//swprintf_s(buff, L"%d bpm", midi_info.speed);
		//lable3_content = buff;

		lable3_str = _T("音色库：");
		lable3_content = theApp.m_player.GetSoundFontName();
	}
	else
	{
		lable2_str = _T("艺术家：");
		lable2_content = theApp.m_player.GetCurrentSongInfo().artist;
		lable3_str = _T("唱片集：");
		lable3_content = theApp.m_player.GetCurrentSongInfo().album;
	}
	lable4_str = _T("格式：");
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
	lable4_content = buff;
	//显示标题
	tmp.MoveToXY(text_start.x, text_start.y + text_height);
	tmp.right = tmp.left + theApp.DPI(52);
	m_draw.DrawWindowText(tmp, lable1_str.c_str(), m_colors.color_text_lable);
	tmp.MoveToX(tmp.left + theApp.DPI(52));
	tmp.right = info_rect.right - m_pLayout->margin;
	static CDrawCommon::ScrollInfo scroll_info2;
	m_draw.DrawScrollText2(tmp, lable1_content.c_str(), m_colors.color_text, theApp.DPI(1), false, scroll_info2, reset);
	//显示艺术家
	tmp.MoveToXY(text_start.x, text_start.y + 2 * text_height);
	tmp.right = tmp.left + theApp.DPI(52);
	m_draw.DrawWindowText(tmp, lable2_str.c_str(), m_colors.color_text_lable);
	tmp.MoveToX(tmp.left + theApp.DPI(52));
	tmp.right = info_rect.right - m_pLayout->margin;
	static CDrawCommon::ScrollInfo scroll_info3;
	if (theApp.m_player.IsMidi())
		m_draw.DrawWindowText(tmp, lable2_content.c_str(), m_colors.color_text);
	else
		m_draw.DrawScrollText2(tmp, lable2_content.c_str(), m_colors.color_text, theApp.DPI(1), false, scroll_info3, reset);
	//显示唱片集
	tmp.MoveToXY(text_start.x, text_start.y + 3 * text_height);
	tmp.right = tmp.left + theApp.DPI(52);
	m_draw.DrawWindowText(tmp, lable3_str.c_str(), m_colors.color_text_lable);
	tmp.MoveToX(tmp.left + theApp.DPI(52));
	tmp.right = info_rect.right - m_pLayout->margin;
	static CDrawCommon::ScrollInfo scroll_info4;
	m_draw.DrawScrollText2(tmp, lable3_content.c_str(), m_colors.color_text, theApp.DPI(1), false, scroll_info4, reset);
	//显示文件格式和比特率
	tmp.MoveToXY(text_start.x, text_start.y + 4 * text_height);
	tmp.right = tmp.left + theApp.DPI(52);
	m_draw.DrawWindowText(tmp, lable4_str.c_str(), m_colors.color_text_lable);
	tmp.MoveToX(tmp.left + theApp.DPI(52));
	tmp.right = info_rect.right - m_pLayout->margin;
	static CDrawCommon::ScrollInfo scroll_info5;
	m_draw.DrawScrollText2(tmp, lable4_content.c_str(), m_colors.color_text, theApp.DPI(1), false, scroll_info5, reset);

	//显示频谱分析
	CRect spectral_rect{ CPoint{info_rect.left + m_pLayout->margin, info_rect.top + m_pLayout->margin}, m_pLayout->spectral_size };
	//绘制背景
	if (draw_background)
		m_draw.FillAlphaRect(spectral_rect, m_colors.color_spectrum_back, ALPHA_CHG(m_colors.background_transparency) * 2 / 3);
	else
		m_draw.FillRect(spectral_rect, m_colors.color_spectrum_back);
	if (theApp.m_app_setting_data.show_album_cover)
	{
		//绘制专辑封面
		m_draw_data.cover_rect = spectral_rect;
		m_draw_data.cover_rect.DeflateRect(m_pLayout->margin / 2, m_pLayout->margin / 2);
		if (theApp.m_player.AlbumCoverExist())
		{
			m_draw.DrawBitmap(theApp.m_player.GetAlbumCover(), m_draw_data.cover_rect.TopLeft(), m_draw_data.cover_rect.Size(), theApp.m_app_setting_data.album_cover_fit);
		}
		else
		{
			int cover_side = m_draw_data.cover_rect.Height() * 3 / 4;
			int x = m_draw_data.cover_rect.left + (m_draw_data.cover_rect.Width() - cover_side) / 2;
			int y = m_draw_data.cover_rect.top + (m_draw_data.cover_rect.Height() - cover_side) / 2;
			::DrawIconEx(m_draw.GetDC()->GetSafeHdc(), x, y, theApp.m_default_cover, cover_side, cover_side, 0, NULL, DI_NORMAL);
		}
	}

	if (theApp.m_app_setting_data.show_spectrum)
	{
		const int ROWS = 31;		//要显示的频谱柱形的数量
		int gap_width{ theApp.DPIRound(1, 0.4) };		//频谱柱形间隙宽度
		CRect rects[ROWS];
		int width = (spectral_rect.Width() - (ROWS - 1)*gap_width) / (ROWS - 1);
		rects[0] = spectral_rect;
		rects[0].DeflateRect(m_pLayout->margin / 2, m_pLayout->margin / 2);
		rects[0].right = rects[0].left + width;
		for (int i{ 1 }; i < ROWS; i++)
		{
			rects[i] = rects[0];
			rects[i].left += (i * (width + gap_width));
			rects[i].right += (i * (width + gap_width));
		}
		for (int i{}; i < ROWS; i++)
		{
			float spetral_data = (theApp.m_player.GetSpectralData()[i * 2] + theApp.m_player.GetSpectralData()[i * 2 + 1]) / 2;
			float peak_data = (theApp.m_player.GetSpectralPeakData()[i * 2] + theApp.m_player.GetSpectralPeakData()[i * 2 + 1]) / 2;

			CRect rect_tmp{ rects[i] };
			int spetral_height = static_cast<int>(spetral_data * rects[0].Height() / 30 * theApp.m_app_setting_data.sprctrum_height / 100);
			int peak_height = static_cast<int>(peak_data * rects[0].Height() / 30 * theApp.m_app_setting_data.sprctrum_height / 100);
			if (spetral_height <= 0 || theApp.m_player.IsError()) spetral_height = 1;		//频谱高度最少为1个像素，如果播放出错，也不显示频谱
			if (peak_height <= 0 || theApp.m_player.IsError()) peak_height = 1;		//频谱高度最少为1个像素，如果播放出错，也不显示频谱
			rect_tmp.top = rect_tmp.bottom - spetral_height;
			if (rect_tmp.top < rects[0].top) rect_tmp.top = rects[0].top;
			COLORREF color;
			if (theApp.m_app_setting_data.show_album_cover && theApp.m_player.AlbumCoverExist())
				color = m_colors.color_spectrum_cover;
			else
				color = m_colors.color_spectrum;
			MemDC.FillSolidRect(rect_tmp, color);

			CRect rect_peak{ rect_tmp };
			rect_peak.bottom = rect_tmp.bottom - peak_height - theApp.DPIRound(1.1);
			rect_peak.top = rect_peak.bottom - theApp.DPIRound(1.1);
			//if (peak_height > 1)
			MemDC.FillSolidRect(rect_peak, color);
		}
	}

	//显示控制条的信息
	//绘制背景
	CPoint point{ spectral_rect.left, spectral_rect.bottom };
	point.y += 2 * m_pLayout->margin;
	CRect other_info_rect{ point, CSize(info_rect.Width() - 2 * m_pLayout->margin,theApp.DPI(24)) };
	if (draw_background)
		m_draw.FillAlphaRect(other_info_rect, m_colors.color_control_bar_back, ALPHA_CHG(m_colors.background_transparency));
	else
		m_draw.FillRect(other_info_rect, m_colors.color_control_bar_back);
	//显示文字信息
	//m_draw.SetBackColor(theApp.m_app_setting_data.theme_color.light3);
	//显示循环模式
	tmp = other_info_rect;
	tmp.left += m_pLayout->margin;
	tmp.right = tmp.left + theApp.DPI(60);
	m_draw_data.repetemode_rect = tmp;
	m_draw_data.repetemode_rect.DeflateRect(0, theApp.DPI(4));
	CString repeat_mode_str;
	switch (theApp.m_player.GetRepeatMode())
	{
	case RepeatMode::RM_PLAY_ORDER: repeat_mode_str += _T("顺序播放"); break;
	case RepeatMode::RM_LOOP_PLAYLIST: repeat_mode_str += _T("列表循环"); break;
	case RepeatMode::RM_LOOP_TRACK: repeat_mode_str += _T("单曲循环"); break;
	case RepeatMode::RM_PLAY_SHUFFLE: repeat_mode_str += _T("随机播放"); break;
	}
	if (m_draw_data.repetemode_hover)		//鼠标指向“循环模式”时，以另外一种颜色显示
		m_draw.DrawWindowText(m_draw_data.repetemode_rect, repeat_mode_str, m_colors.color_text_heighlight);
	else
		m_draw.DrawWindowText(m_draw_data.repetemode_rect, repeat_mode_str, m_colors.color_text);
	m_draw_data.repetemode_rect.MoveToXY(CPoint{ m_draw_data.repetemode_rect.left + m_draw_data.draw_rect.left, m_draw_data.repetemode_rect.top + m_draw_data.draw_rect.top });	//将矩形坐标变换为以客户区左上角为原点

	//绘制切换界面按钮
	tmp.right = other_info_rect.right;
	tmp.left = tmp.right - other_info_rect.Height();
	CRect rc_icon = tmp;
	rc_icon.DeflateRect(theApp.DPI(2), theApp.DPI(2));
	m_draw.SetDrawArea(rc_icon);

	BYTE alpha;
	if (draw_background)
		alpha = ALPHA_CHG(m_colors.background_transparency);
	else
		alpha = 255;
	if (m_draw_data.skin_btn.hover)
		m_draw.FillAlphaRect(rc_icon, m_colors.color_text_2, alpha);
	else if (!theApp.m_app_setting_data.dark_mode)
		m_draw.FillAlphaRect(rc_icon, m_colors.color_button_back, alpha);

	m_draw_data.skin_btn.rect = DrawAreaToClient(rc_icon, m_draw_data.draw_rect);

	rc_icon = tmp;
	rc_icon.DeflateRect(theApp.DPI(4), theApp.DPI(4));
	m_draw.DrawIcon(theApp.m_skin_icon, rc_icon.TopLeft(), rc_icon.Size());

	//显示<<<<
	int progress;
	Time time{ theApp.m_player.GetCurrentPosition() };
	if (theApp.m_player.IsMidi())
	{
		////progress = (theApp.m_player.GetMidiInfo().midi_position % 16 + 1) *1000 / 16;
		//if (theApp.m_player.GetMidiInfo().tempo == 0)
		//	progress = 0;
		//else
		//	progress = (time.time2int() * 1000 / theApp.m_player.GetMidiInfo().tempo % 4 + 1) * 250;
		progress = (theApp.m_player.GetMidiInfo().midi_position % 4 + 1) * 250;
	}
	else
	{
		progress = (time.sec % 4 * 1000 + time.msec) / 4;
	}
	tmp.right = tmp.left;
	tmp.left = tmp.right - theApp.DPI(49);
	m_draw.DrawWindowText(tmp, _T("<<<<"), m_colors.color_text, m_colors.color_text_2, progress, false);

	//显示音量																																									//显示音量
	tmp.right = tmp.left;
	tmp.left = tmp.right - theApp.DPI(75);
	m_draw_data.volume_btn.rect = tmp;
	m_draw_data.volume_btn.rect.DeflateRect(0, theApp.DPI(4));
	m_draw_data.volume_btn.rect.right -= theApp.DPI(12);
	swprintf_s(buff, L"音量：%d%%", theApp.m_player.GetVolume());
	if (m_draw_data.volume_btn.hover)		//鼠标指向音量区域时，以另外一种颜色显示
		m_draw.DrawWindowText(tmp, buff, m_colors.color_text_heighlight);
	else
		m_draw.DrawWindowText(tmp, buff, m_colors.color_text);
	//设置音量调整按钮的位置
	m_draw_data.volume_down_rect = m_draw_data.volume_btn.rect;
	m_draw_data.volume_down_rect.bottom += theApp.DPI(4);
	m_draw_data.volume_down_rect.MoveToY(m_draw_data.volume_btn.rect.bottom);
	m_draw_data.volume_down_rect.right = m_draw_data.volume_btn.rect.left + m_draw_data.volume_btn.rect.Width() / 2;
	m_draw_data.volume_up_rect = m_draw_data.volume_down_rect;
	m_draw_data.volume_up_rect.MoveToX(m_draw_data.volume_down_rect.right);
	m_draw_data.volume_btn.rect.MoveToXY(CPoint{ m_draw_data.volume_btn.rect.left + m_draw_data.draw_rect.left, m_draw_data.volume_btn.rect.top + m_draw_data.draw_rect.top });	//将矩形坐标变换为以客户区左上角为原点

	//显示歌词
	m_draw.SetFont(&m_ui_data.lyric_font);
	CRect lyric_rect;
	if (narrow_mode)
	{
		lyric_rect = other_info_rect;
		lyric_rect.MoveToY(other_info_rect.bottom + m_pLayout->margin);
		DrawLyricsSingleLine(lyric_rect);
	}
	else
	{
		//if (theApp.m_player.IsPlaying() || reset)
		//{
		lyric_rect = info_rect;
		lyric_rect.MoveToY(info_rect.bottom/* + 2*m_pLayout->margin*/);
		lyric_rect.bottom = m_draw_data.draw_rect.Height()/* - m_pLayout->margin*/;
		DrawLyricsMulityLine(lyric_rect, &MemDC);
		//}
	}

	//绘制音量调按钮，因为必须在上层，所以必须在歌词绘制完成后绘制
	if (m_draw_data.show_volume_adj)
	{
		//m_draw.SetBackColor(theApp.m_app_setting_data.theme_color.light1);
		if (draw_background)
		{
			m_draw.FillAlphaRect(m_draw_data.volume_down_rect, m_colors.color_text_2, ALPHA_CHG(m_colors.background_transparency));
			m_draw.FillAlphaRect(m_draw_data.volume_up_rect, m_colors.color_text_2, ALPHA_CHG(m_colors.background_transparency));
		}
		else
		{
			m_draw.FillRect(m_draw_data.volume_down_rect, m_colors.color_text_2);
			m_draw.FillRect(m_draw_data.volume_up_rect, m_colors.color_text_2);
		}
		m_draw.DrawWindowText(m_draw_data.volume_down_rect, L"-", ColorTable::WHITE, Alignment::CENTER);
		m_draw.DrawWindowText(m_draw_data.volume_up_rect, L"+", ColorTable::WHITE, Alignment::CENTER);
	}
	m_draw_data.volume_down_rect.MoveToXY(CPoint{ m_draw_data.volume_down_rect.left + m_draw_data.draw_rect.left, m_draw_data.volume_down_rect.top + m_draw_data.draw_rect.top });	//将矩形坐标变换为以客户区左上角为原点
	m_draw_data.volume_up_rect.MoveToXY(CPoint{ m_draw_data.volume_up_rect.left + m_draw_data.draw_rect.left, m_draw_data.volume_up_rect.top + m_draw_data.draw_rect.top });	//将矩形坐标变换为以客户区左上角为原点

	//将缓冲区DC中的图像拷贝到屏幕中显示
	m_pDC->BitBlt(m_draw_data.draw_rect.left, m_draw_data.draw_rect.top, m_draw_data.draw_rect.Width(), m_draw_data.draw_rect.Height(), &MemDC, 0, 0, SRCCOPY);
	MemDC.SelectObject(pOldBit);
	MemBitmap.DeleteObject();
	MemDC.DeleteDC();
}

void CPlayerUI::DrawLyricsSingleLine(CRect lyric_rect)
{
	if (theApp.m_app_setting_data.lyric_background)
	{
		bool draw_background{ theApp.m_app_setting_data.album_cover_as_background && (theApp.m_player.AlbumCoverExist() || !m_ui_data.default_background.IsNull()) };
		if (draw_background)
			m_draw.FillAlphaRect(lyric_rect, m_colors.color_lyric_back, ALPHA_CHG(m_colors.background_transparency) * 3 / 5);
		else
			m_draw.FillRect(lyric_rect, m_colors.color_lyric_back);
	}
	if (theApp.m_player.IsMidi() && theApp.m_general_setting_data.midi_use_inner_lyric && !theApp.m_player.MidiNoLyric())
	{
		wstring current_lyric{ theApp.m_player.GetMidiLyric() };
		m_draw.DrawWindowText(lyric_rect, current_lyric.c_str(), m_colors.color_text, Alignment::CENTER, false, false, true);
	}
	else if (theApp.m_player.m_Lyrics.IsEmpty())
	{
		m_draw.DrawWindowText(lyric_rect, _T("当前歌曲没有歌词"), m_colors.color_text_2, Alignment::CENTER);
	}
	else
	{
		wstring current_lyric{ theApp.m_player.m_Lyrics.GetLyric(Time(theApp.m_player.GetCurrentPosition()), 0).text };	//获取当歌词
		if (current_lyric.empty())		//如果当前歌词为空白，就显示为省略号
			current_lyric = DEFAULT_LYRIC_TEXT;
		if (theApp.m_lyric_setting_data.lyric_karaoke_disp)		//歌词以卡拉OK样式显示时
		{
			int progress{ theApp.m_player.m_Lyrics.GetLyricProgress(Time(theApp.m_player.GetCurrentPosition())) };		//获取当前歌词进度（范围为0~1000）
			m_draw.DrawWindowText(lyric_rect, current_lyric.c_str(), m_colors.color_text, m_colors.color_text_2, progress, true);
		}
		else				//歌词不以卡拉OK样式显示时
		{
			m_draw.DrawWindowText(lyric_rect, current_lyric.c_str(), m_colors.color_text, Alignment::CENTER);
		}
	}
}

void CPlayerUI::DrawLyricsMulityLine(CRect lyric_rect, CDC * pDC)
{
	bool draw_background{ theApp.m_app_setting_data.album_cover_as_background && (theApp.m_player.AlbumCoverExist() || !m_ui_data.default_background.IsNull()) };
	bool midi_lyric{ theApp.m_player.IsMidi() && theApp.m_general_setting_data.midi_use_inner_lyric && !theApp.m_player.MidiNoLyric() };
	//显示“歌词秀”
	CRect tmp;
	tmp = lyric_rect;
	tmp.left += 2 * m_pLayout->margin;
	tmp.bottom = tmp.top + theApp.DPI(28);
	m_draw.SetFont(theApp.m_pMainWnd->GetFont());
	m_draw.DrawWindowText(tmp, _T("歌词秀："), m_colors.color_text);
	//显示翻译按钮
	CRect translate_rect{ tmp };
	translate_rect.DeflateRect(theApp.DPI(4), theApp.DPI(4));
	translate_rect.right = lyric_rect.right - 2 * m_pLayout->margin;
	translate_rect.left = translate_rect.right - translate_rect.Height();
	m_draw_data.translate_btn.rect = translate_rect;
	m_draw_data.translate_btn.rect.MoveToXY(CPoint{ translate_rect.left + m_draw_data.draw_rect.left, translate_rect.top + m_draw_data.draw_rect.top });	//将矩形坐标变换为以客户区左上角为原点
	m_draw_data.translate_btn.enable = theApp.m_player.m_Lyrics.IsTranslated() && !midi_lyric;
	if (m_draw_data.translate_btn.enable)
	{
		BYTE alpha;
		if (draw_background)
			alpha = ALPHA_CHG(m_colors.background_transparency);
		else
			alpha = 255;
		if (m_draw_data.translate_btn.hover)
			m_draw.FillAlphaRect(translate_rect, m_colors.color_text_2, alpha);
		else if (m_ui_data.show_translate)
			m_draw.FillAlphaRect(translate_rect, m_colors.color_button_back, alpha);
		m_draw.DrawWindowText(translate_rect, L"译", m_colors.color_text, Alignment::CENTER);
	}
	else
	{
		m_draw.DrawWindowText(translate_rect, L"译", GRAY(200), Alignment::CENTER);
	}
	//填充歌词区域背景色
	m_draw.SetFont(&m_ui_data.lyric_font);
	CRect lyric_area = lyric_rect;
	lyric_area.DeflateRect(2 * m_pLayout->margin, 2 * m_pLayout->margin);
	lyric_area.top += theApp.DPI(20);
	if (theApp.m_app_setting_data.lyric_background)
	{
		if (draw_background)
			m_draw.FillAlphaRect(lyric_area, m_colors.color_lyric_back, ALPHA_CHG(m_colors.background_transparency) * 3 / 5);
		else
			m_draw.FillRect(lyric_area, m_colors.color_lyric_back);
	}
	//设置歌词文字区域
	lyric_area.DeflateRect(2 * m_pLayout->margin, 2 * m_pLayout->margin);
	CDrawCommon::SetDrawArea(pDC, lyric_area);

	//绘制歌词文本
	DrawLyricTextMultiLine(lyric_area, &m_ui_data.lyric_font, &m_ui_data.lyric_translate_font,
		m_colors.color_text, m_colors.color_text_2, m_ui_data.show_translate, midi_lyric);
}

void CPlayerUI::RButtonUp(CPoint point, bool narrow_mode)
{
	if (m_draw_data.volume_btn.rect.PtInRect(point) == FALSE)
		m_draw_data.show_volume_adj = false;

	//计算显示信息和显示歌词的区域
	CRect info_rect{ m_draw_data.draw_rect }, lyric_rect{ m_draw_data.draw_rect };
	if (!narrow_mode)
	{
		int height = m_pLayout->info_height2 - 3 * m_pLayout->margin;
		info_rect.bottom = info_rect.top + height;
		lyric_rect.top = info_rect.bottom + 2 * m_pLayout->margin;
	}
	else
	{
		info_rect.bottom -= theApp.DPI(30);
		lyric_rect.top = info_rect.bottom;
	}

	CPoint point1;		//定义一个用于确定光标位置的位置  
	GetCursorPos(&point1);	//获取当前光标的位置，以便使得菜单可以跟随光标，该位置以屏幕左上角点为原点，point则以客户区左上角为原点
	if (m_draw_data.repetemode_rect.PtInRect(point))		//如果在“循环模式”的矩形区域内点击鼠标右键，则弹出“循环模式”的子菜单
	{
		CMenu* pMenu = m_main_popup_menu.GetSubMenu(0)->GetSubMenu(1);
		if (pMenu != NULL)
			pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, theApp.m_pMainWnd);
	}
	else if (info_rect.PtInRect(point))
	{
		m_main_popup_menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, theApp.m_pMainWnd);
	}
	else if (lyric_rect.PtInRect(point))
	{
		m_popup_menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, theApp.m_pMainWnd);
	}
}

void CPlayerUI::MouseMove(CPoint point)
{
	m_draw_data.repetemode_hover = (m_draw_data.repetemode_rect.PtInRect(point) != FALSE);		//当鼠标移动到“循环模式”所在的矩形框内时，将m_draw_data.repetemode_hover置为true
	m_draw_data.volume_btn.hover = (m_draw_data.volume_btn.rect.PtInRect(point) != FALSE);
	m_draw_data.skin_btn.hover = (m_draw_data.skin_btn.rect.PtInRect(point) != FALSE);
	m_draw_data.translate_btn.hover = (m_draw_data.translate_btn.rect.PtInRect(point) != FALSE);

	//显示歌词翻译的鼠标提示
	static bool last_translate_hover{ false };
	AddMouseToolTip(m_draw_data.translate_btn, _T("显示歌词翻译"), &last_translate_hover);

	//显示音量的鼠标提示
	static bool last_volume_hover{ false };
	AddMouseToolTip(m_draw_data.volume_btn, _T("鼠标滚轮调整音量"), &last_volume_hover);

	static bool last_skin_hover{ false };
	AddMouseToolTip(m_draw_data.skin_btn, _T("切换界面"), &last_skin_hover);

	//显示专辑封面的提示
	if (theApp.m_nc_setting_data.show_cover_tip && theApp.m_app_setting_data.show_album_cover)
	{
		CRect cover_rect{ m_draw_data.cover_rect };
		cover_rect.MoveToXY(m_draw_data.draw_rect.left + m_draw_data.cover_rect.left, m_draw_data.draw_rect.top + m_draw_data.cover_rect.top);
		bool show_cover_tip{ cover_rect.PtInRect(point) != FALSE };
		static bool last_show_cover_tip{ false };
		if (!last_show_cover_tip && show_cover_tip)
		{
			CString info;
			if (theApp.m_player.AlbumCoverExist())
			{
				info = _T("专辑封面: ");
				//CFilePathHelper cover_path(theApp.m_player.GetAlbumCoverPath());
				//if (cover_path.GetFileNameWithoutExtension() == ALBUM_COVER_NAME)
				if (theApp.m_player.IsInnerCover())
				{
					info += _T("内嵌图片\r\n图片格式: ");
					switch (theApp.m_player.GetAlbumCoverType())
					{
					case 0: info += _T("jpg"); break;
					case 1: info += _T("png"); break;
					case 2: info += _T("gif"); break;
					}
				}
				else
				{
					info += _T("外部图片\r\n");
					info += theApp.m_player.GetAlbumCoverPath().c_str();
				}
			}
			m_tool_tip->AddTool(theApp.m_pMainWnd, info);
			m_tool_tip->SetMaxTipWidth(theApp.DPI(400));
			m_tool_tip->Pop();
		}
		if (last_show_cover_tip && !show_cover_tip)
		{
			m_tool_tip->AddTool(theApp.m_pMainWnd, _T(""));
			m_tool_tip->Pop();
		}
		last_show_cover_tip = show_cover_tip;
	}

}

void CPlayerUI::LButtonUp(CPoint point)
{
	if (m_draw_data.repetemode_rect.PtInRect(point))	//点击了“循环模式”时，设置循环模式
	{
		theApp.m_player.SetRepeatMode();
	}

	if (!m_draw_data.show_volume_adj)		//如果设有显示音量调整按钮，则点击音量区域就显示音量调整按钮
		m_draw_data.show_volume_adj = (m_draw_data.volume_btn.rect.PtInRect(point) != FALSE);
	else		//如果已经显示了音量调整按钮，则点击音量调整时保持音量调整按钮的显示
		m_draw_data.show_volume_adj = (m_draw_data.volume_up_rect.PtInRect(point) || m_draw_data.volume_down_rect.PtInRect(point));

	if (m_draw_data.show_volume_adj && m_draw_data.volume_up_rect.PtInRect(point))	//点击音量调整按钮中的音量加时音量增加
	{
		theApp.m_player.MusicControl(Command::VOLUME_UP, theApp.m_nc_setting_data.volum_step);
	}
	if (m_draw_data.show_volume_adj && m_draw_data.volume_down_rect.PtInRect(point))	//点击音量调整按钮中的音量减时音量减小
	{
		theApp.m_player.MusicControl(Command::VOLUME_DOWN, theApp.m_nc_setting_data.volum_step);
	}

	if (m_draw_data.translate_btn.rect.PtInRect(point) && m_draw_data.translate_btn.enable)	//点击了“歌词翻译”时，开启或关闭歌词翻译
	{
		m_ui_data.show_translate = !m_ui_data.show_translate;
	}

	if (m_draw_data.skin_btn.rect.PtInRect(point))
	{
		theApp.m_pMainWnd->SendMessage(WM_COMMAND, ID_SWITCH_UI);
	}

}

void CPlayerUI::OnSizeRedraw(int cx, int cy, bool narrow_mode)
{
	m_draw_data.show_volume_adj = false;
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

CRect CPlayerUI::GetThumbnailClipArea(bool narrow_mode)
{
	CRect info_rect;
	if (!narrow_mode)
		info_rect = CRect{ CPoint{ m_pLayout->margin, m_pLayout->control_bar_height + m_pLayout->margin + theApp.DPI(20) }, CSize{ m_ui_data.client_width / 2 - 2 * m_pLayout->margin, m_pLayout->info_height2 - 3 * m_pLayout->margin } };
	else
		info_rect = CRect{ CPoint{ m_pLayout->margin, m_pLayout->control_bar_height + m_pLayout->progress_bar_height + theApp.DPI(20) }, CSize{ m_ui_data.client_width - 2 * m_pLayout->margin, m_pLayout->info_height - 2 * m_pLayout->margin } };

	return info_rect;
}

