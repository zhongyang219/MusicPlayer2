#include "stdafx.h"
#include "CPlayerUI.h"
#include "MusicPlayer2.h"


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
}

void CPlayerUI::DrawInfo(bool narrow_mode, bool reset)
{
	//设置颜色
	if (theApp.m_app_setting_data.dark_mode)
	{
		m_colors.color_text = ColorTable::WHITE;
		m_colors.color_text_lable = theApp.m_app_setting_data.theme_color.light2;
		m_colors.color_text_2 = theApp.m_app_setting_data.theme_color.light1;
		m_colors.color_text_heighlight = theApp.m_app_setting_data.theme_color.light2;
		m_colors.color_back = GRAY(96);
		m_colors.color_lyric_back = theApp.m_app_setting_data.theme_color.dark3;
		m_colors.color_control_bar_back = theApp.m_app_setting_data.theme_color.dark2;
		m_colors.color_spectrum = theApp.m_app_setting_data.theme_color.light2;
		m_colors.color_spectrum_cover = theApp.m_app_setting_data.theme_color.original_color;
		m_colors.color_spectrum_back = theApp.m_app_setting_data.theme_color.dark1;
		m_colors.color_button_back = theApp.m_app_setting_data.theme_color.dark3;

		m_colors.background_transparency = theApp.m_app_setting_data.background_transparency;
	}
	else
	{
		m_colors.color_text = theApp.m_app_setting_data.theme_color.dark2;
		m_colors.color_text_lable = theApp.m_app_setting_data.theme_color.original_color;
		m_colors.color_text_2 = theApp.m_app_setting_data.theme_color.light1;
		m_colors.color_text_heighlight = theApp.m_app_setting_data.theme_color.dark1;
		m_colors.color_back = ColorTable::WHITE;
		m_colors.color_lyric_back = theApp.m_app_setting_data.theme_color.light3;
		m_colors.color_control_bar_back = theApp.m_app_setting_data.theme_color.light3;
		m_colors.color_spectrum = theApp.m_app_setting_data.theme_color.original_color;
		m_colors.color_spectrum_cover = theApp.m_app_setting_data.theme_color.original_color;
		m_colors.color_spectrum_back = theApp.m_app_setting_data.theme_color.light3;
		m_colors.color_button_back = theApp.m_app_setting_data.theme_color.light2;

		m_colors.background_transparency = theApp.m_app_setting_data.background_transparency;
	}


	//设置信息区域的矩形
	CRect info_rect;
	if (!narrow_mode)
		info_rect = CRect{ CPoint{m_ui_data.margin, m_ui_data.control_bar_height + m_ui_data.margin}, CSize{m_ui_data.client_width / 2 - 2 * m_ui_data.margin, m_ui_data.info_height2 - 3 * m_ui_data.margin } };
	else
		info_rect = CRect{ CPoint{ m_ui_data.margin, m_ui_data.control_bar_height + m_ui_data.progress_bar_height}, CSize{ m_ui_data.client_width - 2 * m_ui_data.margin, m_ui_data.info_height - 2 * m_ui_data.margin } };

	//设置缓冲的DC
	CDC MemDC;
	CBitmap MemBitmap;
	MemDC.CreateCompatibleDC(NULL);
	m_ui_data.draw_rect = info_rect;		//绘图区域
	if (!narrow_mode)
		m_ui_data.draw_rect.bottom = m_ui_data.client_height - m_ui_data.margin;
	CRect draw_rect{ m_ui_data.draw_rect };
	draw_rect.MoveToXY(0, 0);
	MemBitmap.CreateCompatibleBitmap(m_pDC, m_ui_data.draw_rect.Width(), m_ui_data.draw_rect.Height());
	CBitmap *pOldBit = MemDC.SelectObject(&MemBitmap);
	m_draw.SetDC(&MemDC);	//将m_draw中的绘图DC设置为缓冲的DC
	if (theApp.m_app_setting_data.album_cover_as_background)
	{
		if (theApp.m_player.AlbumCoverExist())
		{
			CImage& back_image{ theApp.m_app_setting_data.background_gauss_blur ? theApp.m_player.GetAlbumCoverBlur() : theApp.m_player.GetAlbumCover() };
			m_draw.DrawBitmap(back_image, CPoint(0, 0), m_ui_data.draw_rect.Size(), CDrawCommon::StretchMode::FILL);
		}
		else
		{
			//MemDC.FillSolidRect(0, 0, m_ui_data.draw_rect.Width(), m_ui_data.draw_rect.Height(), GetSysColor(COLOR_BTNFACE));	//给缓冲DC的绘图区域填充对话框的背景颜色
			m_draw.DrawBitmap(m_ui_data.default_background, CPoint(0, 0), m_ui_data.draw_rect.Size(), CDrawCommon::StretchMode::FILL);
		}
	}
	else
	{
		m_draw.FillRect(draw_rect, m_colors.color_back);
	}

	//由于设置了缓冲绘图区域，m_ui_data.draw_rect的左上角点变成了绘图的原点
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
	//	gap_rect.bottom = gap_rect.top + 2*m_ui_data.margin;
	//	//CDrawCommon::SetDrawArea(&MemDC, gap_rect);
	//	//MemDC.FillSolidRect(gap_rect, GetSysColor(COLOR_BTNFACE));
	//	m_draw.FillAlphaRect(gap_rect, color_back, ALPHA_CHG(m_colors.background_transparency));
	//}

	CPoint text_start{ info_rect.left + m_ui_data.spectral_size.cx + 2 * m_ui_data.margin, info_rect.top + m_ui_data.margin };		//文本的起始坐标
	int text_height{ theApp.DPI(18) };		//文本的高度

	//显示歌曲信息
	m_draw.SetFont(theApp.m_pMainWnd->GetFont());
	//m_draw.SetBackColor(color_back);
	CRect tmp{ text_start, CSize{1,text_height} };
	wchar_t buff[64];
	if (theApp.m_player.m_loading)
	{
		tmp.right = info_rect.right - m_ui_data.margin;
		static CDrawCommon::ScrollInfo scroll_info0;
		CString info;
		info.Format(_T("找到%d首歌曲，正在读取音频文件信息，已完成%d%%，请稍候……"), theApp.m_player.GetSongNum(), theApp.m_player.m_thread_info.process_percent);
		m_draw.DrawScrollText(tmp, info, m_colors.color_text, theApp.DPI(1.5), false, scroll_info0, reset);
	}
	else
	{
		//显示正在播放的文件名
		tmp.MoveToX(text_start.x + theApp.DPI(82));
		tmp.right = info_rect.right - m_ui_data.margin;
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
	tmp.right = info_rect.right - m_ui_data.margin;
	static CDrawCommon::ScrollInfo scroll_info2;
	m_draw.DrawScrollText2(tmp, lable1_content.c_str(), m_colors.color_text, theApp.DPI(1), false, scroll_info2, reset);
	//显示艺术家
	tmp.MoveToXY(text_start.x, text_start.y + 2 * text_height);
	tmp.right = tmp.left + theApp.DPI(52);
	m_draw.DrawWindowText(tmp, lable2_str.c_str(), m_colors.color_text_lable);
	tmp.MoveToX(tmp.left + theApp.DPI(52));
	tmp.right = info_rect.right - m_ui_data.margin;
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
	tmp.right = info_rect.right - m_ui_data.margin;
	static CDrawCommon::ScrollInfo scroll_info4;
	m_draw.DrawScrollText2(tmp, lable3_content.c_str(), m_colors.color_text, theApp.DPI(1), false, scroll_info4, reset);
	//显示文件格式和比特率
	tmp.MoveToXY(text_start.x, text_start.y + 4 * text_height);
	tmp.right = tmp.left + theApp.DPI(52);
	m_draw.DrawWindowText(tmp, lable4_str.c_str(), m_colors.color_text_lable);
	tmp.MoveToX(tmp.left + theApp.DPI(52));
	tmp.right = info_rect.right - m_ui_data.margin;
	static CDrawCommon::ScrollInfo scroll_info5;
	m_draw.DrawScrollText2(tmp, lable4_content.c_str(), m_colors.color_text, theApp.DPI(1), false, scroll_info5, reset);

	//显示频谱分析
	CRect spectral_rect{ CPoint{info_rect.left + m_ui_data.margin, info_rect.top + m_ui_data.margin}, m_ui_data.spectral_size };
	//绘制背景
	if (draw_background)
		m_draw.FillAlphaRect(spectral_rect, m_colors.color_spectrum_back, ALPHA_CHG(m_colors.background_transparency) * 2 / 3);
	else
		m_draw.FillRect(spectral_rect, m_colors.color_spectrum_back);
	if (theApp.m_app_setting_data.show_album_cover)
	{
		//绘制专辑封面
		m_ui_data.cover_rect = spectral_rect;
		m_ui_data.cover_rect.DeflateRect(m_ui_data.margin / 2, m_ui_data.margin / 2);
		m_draw.DrawBitmap(theApp.m_player.GetAlbumCover(), m_ui_data.cover_rect.TopLeft(), m_ui_data.cover_rect.Size(), theApp.m_app_setting_data.album_cover_fit);
	}
	if (theApp.m_app_setting_data.show_spectrum)
	{
		const int ROWS = 31;		//要显示的频谱柱形的数量
		int gap_width{ theApp.DPIRound(1, 0.4) };		//频谱柱形间隙宽度
		CRect rects[ROWS];
		int width = (spectral_rect.Width() - (ROWS - 1)*gap_width) / (ROWS - 1);
		rects[0] = spectral_rect;
		rects[0].DeflateRect(m_ui_data.margin / 2, m_ui_data.margin / 2);
		rects[0].right = rects[0].left + width;
		for (int i{ 1 }; i < ROWS; i++)
		{
			rects[i] = rects[0];
			rects[i].left += (i * (width + gap_width));
			rects[i].right += (i * (width + gap_width));
		}
		for (int i{}; i < ROWS; i++)
		{
			CRect rect_tmp{ rects[i] };
			int spetral_height = static_cast<int>(theApp.m_player.GetSpectralData()[i] * rects[0].Height() / 30 * theApp.m_app_setting_data.sprctrum_height / 100);
			int peak_height = static_cast<int>(theApp.m_player.GetSpectralPeakData()[i] * rects[0].Height() / 30 * theApp.m_app_setting_data.sprctrum_height / 100);
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
	point.y += 2 * m_ui_data.margin;
	CRect other_info_rect{ point, CSize(info_rect.Width() - 2 * m_ui_data.margin,theApp.DPI(24)) };
	if (draw_background)
		m_draw.FillAlphaRect(other_info_rect, m_colors.color_control_bar_back, ALPHA_CHG(m_colors.background_transparency));
	else
		m_draw.FillRect(other_info_rect, m_colors.color_control_bar_back);
	//显示文字信息
	//m_draw.SetBackColor(theApp.m_app_setting_data.theme_color.light3);
	//显示循环模式
	tmp = other_info_rect;
	tmp.left += m_ui_data.margin;
	tmp.right = tmp.left + theApp.DPI(112);
	m_ui_data.repetemode_rect = tmp;
	m_ui_data.repetemode_rect.DeflateRect(0, theApp.DPI(4));
	CString repeat_mode_str{ _T("循环模式：") };
	switch (theApp.m_player.GetRepeatMode())
	{
	case RepeatMode::RM_PLAY_ORDER: repeat_mode_str += _T("顺序播放"); break;
	case RepeatMode::RM_LOOP_PLAYLIST: repeat_mode_str += _T("列表循环"); break;
	case RepeatMode::RM_LOOP_TRACK: repeat_mode_str += _T("单曲循环"); break;
	case RepeatMode::RM_PLAY_SHUFFLE: repeat_mode_str += _T("随机播放"); break;
	}
	if (m_ui_data.repetemode_hover)		//鼠标指向“循环模式”时，以另外一种颜色显示
		m_draw.DrawWindowText(m_ui_data.repetemode_rect, repeat_mode_str, m_colors.color_text_heighlight);
	else
		m_draw.DrawWindowText(m_ui_data.repetemode_rect, repeat_mode_str, m_colors.color_text);
	m_ui_data.repetemode_rect.MoveToXY(CPoint{ m_ui_data.repetemode_rect.left + m_ui_data.draw_rect.left, m_ui_data.repetemode_rect.top + m_ui_data.draw_rect.top });	//将矩形坐标变换为以客户区左上角为原点
	//显示音量
	tmp.MoveToX(info_rect.right - theApp.DPI(124));
	tmp.right = info_rect.right - theApp.DPI(49);
	m_ui_data.volume_btn.rect = tmp;
	m_ui_data.volume_btn.rect.DeflateRect(0, theApp.DPI(4));
	m_ui_data.volume_btn.rect.right -= theApp.DPI(12);
	swprintf_s(buff, L"音量：%d%%", theApp.m_player.GetVolume());
	if (m_ui_data.volume_btn.hover)		//鼠标指向音量区域时，以另外一种颜色显示
		m_draw.DrawWindowText(tmp, buff, m_colors.color_text_heighlight);
	else
		m_draw.DrawWindowText(tmp, buff, m_colors.color_text);
	//设置音量调整按钮的位置
	m_ui_data.volume_down_rect = m_ui_data.volume_btn.rect;
	m_ui_data.volume_down_rect.bottom += theApp.DPI(4);
	m_ui_data.volume_down_rect.MoveToY(m_ui_data.volume_btn.rect.bottom);
	m_ui_data.volume_down_rect.right = m_ui_data.volume_btn.rect.left + m_ui_data.volume_btn.rect.Width() / 2;
	m_ui_data.volume_up_rect = m_ui_data.volume_down_rect;
	m_ui_data.volume_up_rect.MoveToX(m_ui_data.volume_down_rect.right);
	m_ui_data.volume_btn.rect.MoveToXY(CPoint{ m_ui_data.volume_btn.rect.left + m_ui_data.draw_rect.left, m_ui_data.volume_btn.rect.top + m_ui_data.draw_rect.top });	//将矩形坐标变换为以客户区左上角为原点
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
	tmp.MoveToX(tmp.right);
	tmp.right = other_info_rect.right;
	m_draw.DrawWindowText(tmp, _T("<<<<"), m_colors.color_text, m_colors.color_text_2, progress, false);

	//显示歌词
	m_draw.SetFont(&m_ui_data.lyric_font);
	CRect lyric_rect;
	if (narrow_mode)
	{
		lyric_rect = other_info_rect;
		lyric_rect.MoveToY(other_info_rect.bottom + m_ui_data.margin);
		DrawLyricsSingleLine(lyric_rect);
	}
	else
	{
		//if (theApp.m_player.IsPlaying() || reset)
		//{
		lyric_rect = info_rect;
		lyric_rect.MoveToY(info_rect.bottom/* + 2*m_ui_data.margin*/);
		lyric_rect.bottom = m_ui_data.draw_rect.Height()/* - m_ui_data.margin*/;
		DrawLyricsMulityLine(lyric_rect, &MemDC);
		//}
	}

	//绘制音量调按钮，因为必须在上层，所以必须在歌词绘制完成后绘制
	if (m_ui_data.show_volume_adj)
	{
		//m_draw.SetBackColor(theApp.m_app_setting_data.theme_color.light1);
		if (draw_background)
		{
			m_draw.FillAlphaRect(m_ui_data.volume_down_rect, m_colors.color_text_2, ALPHA_CHG(m_colors.background_transparency));
			m_draw.FillAlphaRect(m_ui_data.volume_up_rect, m_colors.color_text_2, ALPHA_CHG(m_colors.background_transparency));
		}
		else
		{
			m_draw.FillRect(m_ui_data.volume_down_rect, m_colors.color_text_2);
			m_draw.FillRect(m_ui_data.volume_up_rect, m_colors.color_text_2);
		}
		m_draw.DrawWindowText(m_ui_data.volume_down_rect, L"-", ColorTable::WHITE, Alignment::CENTER);
		m_draw.DrawWindowText(m_ui_data.volume_up_rect, L"+", ColorTable::WHITE, Alignment::CENTER);
	}
	m_ui_data.volume_down_rect.MoveToXY(CPoint{ m_ui_data.volume_down_rect.left + m_ui_data.draw_rect.left, m_ui_data.volume_down_rect.top + m_ui_data.draw_rect.top });	//将矩形坐标变换为以客户区左上角为原点
	m_ui_data.volume_up_rect.MoveToXY(CPoint{ m_ui_data.volume_up_rect.left + m_ui_data.draw_rect.left, m_ui_data.volume_up_rect.top + m_ui_data.draw_rect.top });	//将矩形坐标变换为以客户区左上角为原点

	//将缓冲区DC中的图像拷贝到屏幕中显示
	m_pDC->BitBlt(m_ui_data.draw_rect.left, m_ui_data.draw_rect.top, m_ui_data.draw_rect.Width(), m_ui_data.draw_rect.Height(), &MemDC, 0, 0, SRCCOPY);
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
	tmp.left += 2 * m_ui_data.margin;
	tmp.bottom = tmp.top + theApp.DPI(28);
	m_draw.SetFont(theApp.m_pMainWnd->GetFont());
	m_draw.DrawWindowText(tmp, _T("歌词秀："), m_colors.color_text);
	//显示翻译按钮
	CRect translate_rect{ tmp };
	translate_rect.DeflateRect(theApp.DPI(4), theApp.DPI(4));
	translate_rect.right = lyric_rect.right - 2 * m_ui_data.margin;
	translate_rect.left = translate_rect.right - translate_rect.Height();
	m_ui_data.translate_btn.rect = translate_rect;
	m_ui_data.translate_btn.rect.MoveToXY(CPoint{ translate_rect.left + m_ui_data.draw_rect.left, translate_rect.top + m_ui_data.draw_rect.top });	//将矩形坐标变换为以客户区左上角为原点
	m_ui_data.translate_btn.enable = theApp.m_player.m_Lyrics.IsTranslated() && !midi_lyric;
	if (m_ui_data.translate_btn.enable)
	{
		BYTE alpha;
		if (draw_background)
			alpha = ALPHA_CHG(m_colors.background_transparency);
		else
			alpha = 255;
		if (m_ui_data.translate_btn.hover)
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
	lyric_area.DeflateRect(2 * m_ui_data.margin, 2 * m_ui_data.margin);
	lyric_area.top += theApp.DPI(20);
	if (theApp.m_app_setting_data.lyric_background)
	{
		if (draw_background)
			m_draw.FillAlphaRect(lyric_area, m_colors.color_lyric_back, ALPHA_CHG(m_colors.background_transparency) * 3 / 5);
		else
			m_draw.FillRect(lyric_area, m_colors.color_lyric_back);
	}
	//设置歌词文字区域
	lyric_area.DeflateRect(2 * m_ui_data.margin, 2 * m_ui_data.margin);
	CDrawCommon::SetDrawArea(pDC, lyric_area);
	//计算文本高度
	m_pDC->SelectObject(&m_ui_data.lyric_font);
	int text_height = m_pDC->GetTextExtent(L"文").cy;	//根据当前的字体设置计算文本的高度
	int lyric_height = text_height + theApp.m_app_setting_data.lyric_line_space;			//文本高度加上行间距
	int lyric_height2 = lyric_height * 2 + theApp.m_app_setting_data.lyric_line_space;		//包含翻译的歌词高度
	//绘制歌词文本
	if (midi_lyric)
	{
		wstring current_lyric{ theApp.m_player.GetMidiLyric() };
		m_draw.DrawWindowText(lyric_area, current_lyric.c_str(), m_colors.color_text, Alignment::CENTER, false, true);
	}
	else if (theApp.m_player.m_Lyrics.IsEmpty())
	{
		m_draw.DrawWindowText(lyric_area, _T("当前歌曲没有歌词"), m_colors.color_text_2, Alignment::CENTER);
	}
	else
	{
		//CRect arect{ lyric_area };		//一行歌词的矩形区域
		//arect.bottom = arect.top + lyric_height;
		//vector<CRect> rects(theApp.m_player.m_Lyrics.GetLyricCount() + 1, arect);
		//为每一句歌词创建一个矩形，保存在容器里
		vector<CRect> rects;
		int lyric_count = theApp.m_player.m_Lyrics.GetLyricCount() + 1;		//获取歌词数量（由于第一行歌词需要显示标题，所以这里要+1）
		for (int i{}; i < lyric_count; i++)
		{
			CRect arect{ lyric_area };
			if (!theApp.m_player.m_Lyrics.GetLyric(i).translate.empty() && m_ui_data.show_translate)
				arect.bottom = arect.top + lyric_height2;
			else
				arect.bottom = arect.top + lyric_height;
			rects.push_back(arect);
		}
		int center_pos = (lyric_area.top + lyric_area.bottom) / 2;		//歌词区域的中心y坐标
		Time time{ theApp.m_player.GetCurrentPosition() };		//当前播放时间
		int lyric_index = theApp.m_player.m_Lyrics.GetLyricIndex(time) + 1;		//当前歌词的序号（歌词的第一句GetLyricIndex返回的是0，由于显示时第一句歌词要显示标题，所以这里要+1）
		int progress = theApp.m_player.m_Lyrics.GetLyricProgress(time);		//当前歌词进度（范围为0~1000）
		int y_progress;			//当前歌词在y轴上的进度
		if (!theApp.m_player.m_Lyrics.GetLyric(lyric_index).translate.empty() && m_ui_data.show_translate)
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
			if (!theApp.m_player.m_Lyrics.GetLyric(i).translate.empty() && m_ui_data.show_translate)
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
				if (!theApp.m_player.m_Lyrics.GetLyric(i).translate.empty() && m_ui_data.show_translate)
				{
					rect_text.MoveToY(rect_text.top + theApp.m_app_setting_data.lyric_line_space);
					rect_text.bottom = rect_text.top + text_height;
					rect_translate = rect_text;
					rect_translate.MoveToY(rect_text.bottom + theApp.m_app_setting_data.lyric_line_space);
				}

				if (i == lyric_index)		//绘制正在播放的歌词
				{
					//绘制歌词文本
					m_draw.SetFont(&m_ui_data.lyric_font);
					if (theApp.m_lyric_setting_data.lyric_karaoke_disp)
						m_draw.DrawWindowText(rect_text, theApp.m_player.m_Lyrics.GetLyric(i).text.c_str(), m_colors.color_text, m_colors.color_text_2, progress, true, true);
					else
						m_draw.DrawWindowText(rect_text, theApp.m_player.m_Lyrics.GetLyric(i).text.c_str(), m_colors.color_text, m_colors.color_text, progress, true, true);
					//绘制翻译文本
					if (!theApp.m_player.m_Lyrics.GetLyric(i).translate.empty() && m_ui_data.show_translate)
					{
						m_draw.SetFont(&m_ui_data.lyric_translate_font);
						m_draw.DrawWindowText(rect_translate, theApp.m_player.m_Lyrics.GetLyric(i).translate.c_str(), m_colors.color_text, m_colors.color_text, progress, true, true);
					}
				}
				else		//绘制非正在播放的歌词
				{
					//绘制歌词文本
					m_draw.SetFont(&m_ui_data.lyric_font);
					m_draw.DrawWindowText(rect_text, theApp.m_player.m_Lyrics.GetLyric(i).text.c_str(), m_colors.color_text_2, Alignment::CENTER, true);
					//绘制翻译文本
					if (!theApp.m_player.m_Lyrics.GetLyric(i).translate.empty() && m_ui_data.show_translate)
					{
						m_draw.SetFont(&m_ui_data.lyric_translate_font);
						m_draw.DrawWindowText(rect_translate, theApp.m_player.m_Lyrics.GetLyric(i).translate.c_str(), m_colors.color_text_2, Alignment::CENTER, true);
					}
				}
			}
		}
	}
}
