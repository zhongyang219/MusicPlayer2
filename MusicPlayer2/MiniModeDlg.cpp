// MiniModeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "MiniModeDlg.h"
#include "afxdialogex.h"


// CMiniModeDlg 对话框

IMPLEMENT_DYNAMIC(CMiniModeDlg, CDialogEx)

CMiniModeDlg::CMiniModeDlg(int& item_selected, CMenu& popup_menu, CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MINI_DIALOG, pParent), m_item_selected{ item_selected }, m_popup_menu{ popup_menu }
{
	m_screen_width = GetSystemMetrics(SM_CXFULLSCREEN);
	m_screen_height = GetSystemMetrics(SM_CYFULLSCREEN) + 23;
}

CMiniModeDlg::~CMiniModeDlg()
{
}

void CMiniModeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LYRIC_STATIC, m_lyric_static);
	DDX_Control(pDX, IDC_TIME_STATIC1, m_time_static);
	DDX_Control(pDX, ID_M_PREVIOUS, m_previous_button);
	DDX_Control(pDX, ID_M_PLAY_PAUSE, m_play_pause_button);
	DDX_Control(pDX, ID_M_NEXT, m_next_button);
	DDX_Control(pDX, IDC_SPECTRAL_STATIC, m_spectral_static);
	DDX_Control(pDX, IDC_LIST2, m_playlist_ctrl);
	DDX_Control(pDX, IDC_SHOW_LIST_BUTTON, m_show_list_button);
	DDX_Control(pDX, IDOK, m_return_button);
	DDX_Control(pDX, ID_MINI_MODE_EXIT, m_exit_button);
	DDX_Control(pDX, IDC_MINI_PROGRESS_STATIC, m_progress_bar);
}

void CMiniModeDlg::SaveConfig() const
{
	CCommon::WritePrivateProfileIntW(L"mini_mode", L"position_x", m_position_x, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"mini_mode", L"position_y", m_position_y, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"mini_mode", L"text_color", m_theme_color.original_color, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"mini_mode", L"dark_mode", m_dark_mode, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"mini_mode", L"follow_main_color", m_follow_main_color, theApp.m_config_path.c_str());
}

void CMiniModeDlg::LoadConfig()
{
	m_position_x = GetPrivateProfileInt(_T("mini_mode"), _T("position_x"), -1, theApp.m_config_path.c_str());
	m_position_y = GetPrivateProfileInt(_T("mini_mode"), _T("position_y"), -1, theApp.m_config_path.c_str());
	m_theme_color.original_color = GetPrivateProfileInt(_T("mini_mode"), _T("text_color"), 13526784, theApp.m_config_path.c_str());
	m_dark_mode = (GetPrivateProfileInt(_T("mini_mode"), _T("dark_mode"), 0, theApp.m_config_path.c_str()) != 0);
	m_follow_main_color = (GetPrivateProfileInt(_T("mini_mode"), _T("follow_main_color"), 1, theApp.m_config_path.c_str()) != 0);
}

void CMiniModeDlg::CheckWindowPos()
{
	CRect rect;
	GetWindowRect(rect);
	if (rect.left < 0)
	{
		rect.MoveToX(0);
		MoveWindow(rect);
	}
	if (rect.top < 0)
	{
		rect.MoveToY(0);
		MoveWindow(rect);
	}
	if (rect.right > m_screen_width)
	{
		rect.MoveToX(m_screen_width - rect.Width());
		MoveWindow(rect);
	}
	if (rect.bottom > m_screen_height)
	{
		rect.MoveToY(m_screen_height - rect.Height());
		MoveWindow(rect);
	}
}

void CMiniModeDlg::UpdateSongTipInfo()
{
	m_song_tip_info = _T("");
	m_song_tip_info += _T("当前播放：");
	m_song_tip_info += theApp.m_player.GetFileName().c_str();
	m_song_tip_info += _T("\r\n");
	m_song_tip_info += _T("标题：");
	m_song_tip_info += theApp.m_player.GetPlayList()[theApp.m_player.GetIndex()].title.c_str();
	m_song_tip_info += _T("\r\n");
	m_song_tip_info += _T("艺术家：");
	m_song_tip_info += theApp.m_player.GetPlayList()[theApp.m_player.GetIndex()].artist.c_str();
	m_song_tip_info += _T("\r\n");
	m_song_tip_info += _T("唱片集：");
	m_song_tip_info += theApp.m_player.GetPlayList()[theApp.m_player.GetIndex()].album.c_str();
	m_song_tip_info += _T("\r\n");
}

void CMiniModeDlg::DrawSpectral()
{
	if (m_spectrum_pDC == nullptr)
	{
		m_spectrum_pDC = m_spectral_static.GetDC();
	}
	//pDC->SetBkMode(TRANSPARENT);
	CBrush BGBrush, *pOldBrush;
	BGBrush.CreateSolidBrush(m_theme_color.original_color);
	pOldBrush = m_spectrum_pDC->SelectObject(&BGBrush);
	DrawThemeParentBackground(m_hWnd, m_spectrum_pDC->GetSafeHdc(), &m_spectral_client_rect);	//重绘控件区域以解决文字重叠的问题
	//将CPlayer类里获得到的频谱数据变换成ROW列的数据
	float spectral_data[ROW];
	memset(spectral_data, 0, sizeof(spectral_data));
	for (int i{}; i < 16; i++)
	{
		spectral_data[i / (16 / ROW)] += theApp.m_player.GetSpectralData()[i];
	}
	for (int i{}; i < ROW; i++)
	{
		CRect rect_tmp{ m_spectral_rect[i] };
		int spetral_height = static_cast<int>(spectral_data[i] * m_spectral_client_rect.Height() / 120 * theApp.m_app_setting_data.sprctrum_height / 100);
		if (spetral_height <= 0 || theApp.m_player.IsError()) spetral_height = 1;		//频谱高度最少为1个像素，如果播放出错，也不显示频谱
		rect_tmp.top = rect_tmp.bottom - spetral_height;
		if (rect_tmp.top < 0) rect_tmp.top = 0;
		m_spectrum_pDC->FillRect(&rect_tmp, &BGBrush);
	}
	m_spectrum_pDC->SelectObject(pOldBrush);
	BGBrush.DeleteObject();

}

void CMiniModeDlg::DrawBorder()
{
	//CDC* pDC = GetDC();
	//int height = (m_show_playlist ? m_rect.Height() : m_rect_s.Height());
	//CPen aPen;
	//aPen.CreatePen(PS_SOLID, 2 * theApp.m_dpi / 96 , m_item_text_color);
	//CPen* pOldPen{ pDC->SelectObject(&aPen) };
	//pDC->MoveTo(0, 0);
	//pDC->LineTo(m_rect.Width(), 0);
	//pDC->LineTo(m_rect.Width(), height);
	//pDC->LineTo(0, height);
	//pDC->LineTo(0, 0);
	//pDC->SelectObject(pOldPen);

}


BEGIN_MESSAGE_MAP(CMiniModeDlg, CDialogEx)
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
//	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_MOVE()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_MINI_MODE_EXIT, &CMiniModeDlg::OnMiniModeExit)
	//ON_COMMAND(ID_SONG_INFO, &CMiniModeDlg::OnSongInfo)
	ON_WM_INITMENU()
	ON_COMMAND(ID_SET_TEXT_COLOR, &CMiniModeDlg::OnSetTextColor)
	ON_BN_CLICKED(ID_M_PLAY_PAUSE, &CMiniModeDlg::OnBnClickedMPlayPause)
	ON_BN_CLICKED(ID_M_PREVIOUS, &CMiniModeDlg::OnBnClickedMPrevious)
	ON_BN_CLICKED(ID_M_NEXT, &CMiniModeDlg::OnBnClickedMNext)
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDBLCLK()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST2, &CMiniModeDlg::OnNMDblclkList2)
	ON_NOTIFY(NM_RCLICK, IDC_LIST2, &CMiniModeDlg::OnNMRClickList2)
	ON_BN_CLICKED(IDC_SHOW_LIST_BUTTON, &CMiniModeDlg::OnBnClickedShowListButton)
	ON_WM_PAINT()
	ON_COMMAND(ID_DARK_MODE, &CMiniModeDlg::OnDarkMode)
	ON_COMMAND(ID_FOLLOW_MAIN_COLOR, &CMiniModeDlg::OnFollowMainColor)
	ON_STN_CLICKED(IDC_MINI_PROGRESS_STATIC, &CMiniModeDlg::OnStnClickedMiniProgressStatic)
END_MESSAGE_MAP()


// CMiniModeDlg 消息处理程序


void CMiniModeDlg::UpdatePlayPauseButton()
{
	if (theApp.m_player.IsPlaying() && !theApp.m_player.IsError())
	{
		m_play_pause_button.SetImage(m_hPauseIcon_s, FALSE);
		m_play_pause_button.SetWindowText(_T(""));		//尽管按钮上只显示图标而不显示文本，但是这里调用SetWindowText函数可以解决更新按钮图标后不刷新的问题
		m_Mytip.UpdateTipText(_T("暂停"), &m_play_pause_button);
	}
	else
	{
		m_play_pause_button.SetImage(m_hPlayIcon_s, FALSE);
		m_play_pause_button.SetWindowText(_T(""));
		m_Mytip.UpdateTipText(_T("播放"), &m_play_pause_button);
	}
}

void CMiniModeDlg::ShowPlaylist()
{
	m_playlist_ctrl.ShowPlaylist(m_display_format);
	SetPlayListColor();
}

void CMiniModeDlg::SetPlayListColor()
{
	//m_playlist_ctrl.ResetAllItemColor();
	//m_playlist_ctrl.SetItemColor(theApp.m_player.GetIndex(), m_item_text_color, m_item_back_color);
	m_playlist_ctrl.SetHightItem(theApp.m_player.GetIndex());
	m_playlist_ctrl.SetColor(m_theme_color);
	m_playlist_ctrl.EnsureVisible(theApp.m_player.GetIndex(), FALSE);
}

BOOL CMiniModeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	LoadConfig();

	SetWindowText(_T("MiniDlg_ByH87M"));		//为对话框设置一个标题，用于通过FindWindow函数查找

	GetWindowRect(m_rect);
	CRect rect;
	m_lyric_static.GetWindowRect(rect);
	m_rect_s = m_rect;
	m_rect_s.bottom = rect.bottom + DPI(2);

	//获取专辑封面的矩形区域
	m_album_rect = m_rect_s;
	CRect previous_btn_tect;
	m_previous_button.GetWindowRect(previous_btn_tect);
	m_album_rect.right = previous_btn_tect.left;
	m_album_rect.MoveToXY(0, 0);
	m_album_rect.DeflateRect(DPI(2), DPI(2));


	//设置窗口背景图片
	//m_back_img = (HBITMAP)LoadImage(NULL, (CCommon::GetExePath() + L"minimode_background.bmp").c_str(), IMAGE_BITMAP, m_rect_s.Width(), m_rect_s.Height(), LR_LOADFROMFILE);
	//SetBackgroundImage(m_back_img);

	m_lyric_static.SetTextCenter(true);

	//载入按钮小图标（16*16）
	m_hPreviousIcon_s = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_PREVIOUS), IMAGE_ICON, DPI(16), DPI(16), LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);
	m_hNextIcon_s = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_NEXT1), IMAGE_ICON, DPI(16), DPI(16), LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);
	m_hPlayIcon_s = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_PLAY), IMAGE_ICON, DPI(16), DPI(16), LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);
	m_hPauseIcon_s = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_PAUSE), IMAGE_ICON, DPI(16), DPI(16), LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);

	//设置按钮图标
	m_previous_button.SetImage(m_hPreviousIcon_s);
	if (theApp.m_player.IsPlaying())
		m_play_pause_button.SetImage(m_hPauseIcon_s, FALSE);
	else
		m_play_pause_button.SetImage(m_hPlayIcon_s, FALSE);
	m_next_button.SetImage(m_hNextIcon_s);

	//设置按钮上的文本为空
	m_previous_button.SetWindowText(_T(""));
	m_play_pause_button.SetWindowText(_T(""));
	m_next_button.SetWindowText(_T(""));

	m_show_list_button.SetFont(GetFont());
	m_return_button.SetFont(GetFont());
	m_exit_button.SetFont(GetFont());

	//初始化鼠标提示
	m_Mytip.Create(this, TTS_ALWAYSTIP);
	m_Mytip.AddTool(&m_previous_button, _T("上一曲"));
	if (theApp.m_player.IsPlaying())
		m_Mytip.AddTool(&m_play_pause_button, _T("暂停"));
	else
		m_Mytip.AddTool(&m_play_pause_button, _T("播放"));
	m_Mytip.AddTool(&m_next_button, _T("下一曲"));
	m_Mytip.AddTool(GetDlgItem(IDOK), _T("返回正常模式"));
	m_Mytip.AddTool(GetDlgItem(ID_MINI_MODE_EXIT), _T("退出程序"));
	m_Mytip.AddTool(GetDlgItem(IDC_SHOW_LIST_BUTTON), _T("显示/隐藏播放列表"));
	m_Mytip.SetMaxTipWidth(800);

	UpdateSongTipInfo();
	m_Mytip.AddTool(this, m_song_tip_info);

	//初始化窗口位置
	if (m_position_x != -1 && m_position_y != -1)
		SetWindowPos(nullptr , m_position_x, m_position_y, m_rect_s.Width(), m_rect_s.Height(), SWP_NOZORDER);
	CheckWindowPos();

	//装载右键菜单
	m_menu.LoadMenu(IDR_MINI_MODE_MENU);
	//m_list_popup_menu.LoadMenu(IDR_POPUP_MENU);		//装载右键菜单

	//计算频谱分析中每个柱形的位置
	m_spectral_static.GetClientRect(&m_spectral_client_rect);
	int width{ m_spectral_client_rect.Width()/ROW };	//每个柱形的宽度
	m_spectral_rect[0] = m_spectral_client_rect;
	m_spectral_rect[0].right = m_spectral_rect[0].left + width - 1;
	for (int i{ 1 }; i < ROW; i++)
	{
		m_spectral_rect[i] = m_spectral_rect[0];
		m_spectral_rect[i].left += (i * width);
		m_spectral_rect[i].right += (i * width);
	}

	//获取频谱分析控件的CDC，用于绘制频谱柱形
	m_spectrum_pDC = m_spectral_static.GetDC();

	m_draw.Create(GetDC(), this);

	//为了确保每次打开迷你窗口时一定会显示当前歌词，在这里将此变量置为-1
	m_last_lyric_index = -1;

	m_last_index = -1;

	////显示播放歌曲名
	//m_lyric_static.SetWindowText(theApp.m_player.GetFileName().c_str());

	//初始化进度条控件
	m_time_static.GetWindowRect(rect);
	ScreenToClient(rect);
	rect.MoveToY(rect.bottom);
	rect.bottom = rect.top + DPI(8);
	m_progress_bar.MoveWindow(rect);
	m_progress_bar.GetToolTip()->SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);	//设置消息提示置顶
	m_progress_bar.SetProgressBarHeight(DPI(2));
	m_progress_bar.SetRange(1000);		//设置进度条的范围
	
	m_progress_bar.SetSongLength(theApp.m_player.GetSongLength());

	//设置定时器
	SetTimer(TIMER_ID_MINI, TIMER_ELAPSE_MINI, NULL);	//设置主定时器
	SetTimer(TIMER_ID_MINI2, TIMER_ELAPSE, NULL);		//设置用于频谱分析的定时器

	//显示播放列表
	ShowPlaylist();

	//设置主题色
	if (m_follow_main_color)
		m_theme_color = theApp.m_app_setting_data.theme_color;

	m_first_start = true;
	m_show_playlist = false;
	m_show_volume = false;
	m_first_show_time = true;

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CMiniModeDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));

	CDialogEx::OnLButtonDown(nFlags, point);
}


void CMiniModeDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == TIMER_ID_MINI)
	{
		//更新鼠标提示
		static int index{};
		static wstring song_name{};
		//如果当前播放的歌曲发生变化，就更新鼠标提示信息
		if (index != theApp.m_player.GetIndex() || song_name != theApp.m_player.GetFileName())
		{
			UpdateSongTipInfo();
			m_Mytip.UpdateTipText(m_song_tip_info, this);
			index = theApp.m_player.GetIndex();
			song_name = theApp.m_player.GetFileName();
		}
		
	}
	if (nIDEvent == TIMER_ID_MINI2)
	{
		if (m_first_start)
		{
			//窗口刚刚打开时执行
			ColorChanged();
			RePaint();
			m_first_start = false;
		}
		if (theApp.m_player.IsPlaying())
		{
			DrawSpectral();
		}
		ShowInfo(false);
	}
	if (nIDEvent == 11)
	{
		m_show_volume = false;
		KillTimer(11);
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CMiniModeDlg::ShowVolume(int volume)
{
	CString volume_str;
	volume_str.Format(_T("音量:%d%%"), volume);
	m_time_static.DrawWindowText(volume_str);
}

void CMiniModeDlg::ShowInfo(bool force_refresh)
{
	if (theApp.m_player.m_Lyrics.IsEmpty())	//没有歌词时显示播放的文件名
	{
		//正在播放的文件名以滚动的样式显示，函数每调用一次滚动DPI/72个像素（150%缩放时为2个像素）。如果参数要求强制刷新，则重置滚动位置
		m_lyric_static.DrawScrollText(CPlayListCtrl::GetDisplayStr(theApp.m_player.GetCurrentSongInfo(), m_display_format).c_str(), theApp.m_dpi / 72, force_refresh);
	}
	else		//显示歌词
	{
		if (theApp.m_play_setting_data.lyric_karaoke_disp)		//歌词以卡拉OK样式显示时
		{
			wstring current_lyric{ theApp.m_player.m_Lyrics.GetLyric(Time(theApp.m_player.GetCurrentPosition()), 0) };	//获取当歌词
			int progress{ theApp.m_player.m_Lyrics.GetLyricProgress(Time(theApp.m_player.GetCurrentPosition())) };		//获取当前歌词进度（范围为0~1000）
			if (current_lyric.empty())		//如果当前歌词为空白，就显示为省略号
				current_lyric = DEFAULT_LYRIC_TEXT;
			m_lyric_static.DrawWindowText(current_lyric.c_str(), progress);
		}
		else				//歌词不以卡拉OK样式显示时
		{
			int lyric_index = theApp.m_player.m_Lyrics.GetLyricIndex(Time(theApp.m_player.GetCurrentPosition()));
			//判断显示的歌词是否发生变化，如果发生了变化，或参数要求强制刷新，就更新歌词显示
			if (lyric_index != m_last_lyric_index || force_refresh)
			{
				wstring current_lyric{ theApp.m_player.m_Lyrics.GetLyric(Time(theApp.m_player.GetCurrentPosition()), 0) };
				if (current_lyric.empty())		//如果当前歌词为空白，就显示为省略号
					current_lyric = DEFAULT_LYRIC_TEXT;
				m_lyric_static.DrawWindowText(current_lyric.c_str());
				m_last_lyric_index = lyric_index;
			}
		}
	}
	//显示播放时间
	if (!m_show_volume)
	{
		static int last_second;
		int second = theApp.m_player.GetCurrentSecond();
		if (second != last_second || m_first_show_time)
		{
			if (theApp.m_player.IsError())		//出现错误时显示错误信息
			{
				m_time_static.DrawWindowText(_T("播放出错"));
			}
			else
			{
				m_time_static.DrawWindowText(theApp.m_player.GetTimeString().c_str());
			}
			last_second = second;
			m_first_show_time = false;
		}
	}

	//绘制专辑封面
	if (theApp.m_player.AlbumCoverExist())
		m_draw.DrawBitmap(theApp.m_player.GetAlbumCover(), m_album_rect.TopLeft(), m_album_rect.Size(), CDrawCommon::StretchMode::FILL);
	else
		m_draw.DrawBitmap(IDB_DEFAULT_COVER, m_album_rect.TopLeft(), m_album_rect.Size(), CDrawCommon::StretchMode::FILL);
}

void CMiniModeDlg::ColorChanged()
{
	//m_item_text_color = CColorConvert::ConvertToItemColor(theme_color);
	//m_item_back_color = CColorConvert::ConvertToBackColor(theme_color);
	//m_lyric_text_color = CColorConvert::ConvertToLightColor(theme_color);
	CColorConvert::ConvertColor(m_theme_color);
	if (!m_dark_mode)
	{
		//设置窗口文本颜色
		m_lyric_static.SetTextColor(m_theme_color.dark2);
		m_lyric_static.SetText2Color(m_theme_color.light1);
		m_time_static.SetTextColor(m_theme_color.dark2);
		//设置背景颜色
		SetBackgroundColor(m_theme_color.light3);
		//设置进度条颜色
		m_progress_bar.SetColor(m_theme_color.original_color);
		m_progress_bar.SetBackColor(RGB(255, 255, 255));
	}
	else
	{
		//设置窗口文本颜色
		m_lyric_static.SetTextColor(m_theme_color.light3);
		m_lyric_static.SetText2Color(m_theme_color.light1);
		m_time_static.SetTextColor(m_theme_color.light3);
		//设置背景颜色
		SetBackgroundColor(m_theme_color.dark2);
		//设置进度条颜色
		m_progress_bar.SetColor(RGB(255, 255, 255));
		m_progress_bar.SetBackColor(m_theme_color.dark1);
	}
	SetPlayListColor();
	//m_progress_bar.Invalidate();
}

void CMiniModeDlg::SetVolume(bool up)
{
	if (up)
		theApp.m_player.MusicControl(Command::VOLUME_UP);
	else
		theApp.m_player.MusicControl(Command::VOLUME_DOWN);
	ShowVolume(theApp.m_player.GetVolume());
	KillTimer(11);
	SetTimer(11, 1500, NULL);		//显示音量后设置一个1500毫秒的定时器（音量显示保持1.5秒）
	m_show_volume = true;
}

void CMiniModeDlg::RePaint()
{
	ShowInfo(true);
	DrawSpectral();
	DrawBorder();
}


BOOL CMiniModeDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		OnOK();		//按ESC键返回主窗口
		return TRUE;
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) return TRUE;

	//按上下方向键调整音量
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_UP)
	{
		SetVolume(true);
		return TRUE;
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_DOWN)
	{
		SetVolume(false);
		return TRUE;
	}

	//按Ctrl+X退出
	if (pMsg->message == WM_KEYDOWN && (GetKeyState(VK_CONTROL) & 0x80) && pMsg->wParam == 'X')
	{
		OnCancel();
		return TRUE;
	}
	//按Ctrl+M回到主窗口
	if (pMsg->message == WM_KEYDOWN && (GetKeyState(VK_CONTROL) & 0x80) && pMsg->wParam == 'M')
	{
		OnOK();
		return TRUE;
	}

	//将此窗口的其他键盘消息转发给主窗口
	if (pMsg->message == WM_KEYDOWN)
	{
		::PostMessage(theApp.m_pMainWnd->m_hWnd, WM_KEYDOWN, pMsg->wParam, pMsg->lParam);
		return TRUE;
	}

	if (pMsg->message == WM_MOUSEMOVE)
		m_Mytip.RelayEvent(pMsg);

	return CDialogEx::PreTranslateMessage(pMsg);
}


//void CMiniModeDlg::OnClose()
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	SaveConfig();
//	CDialogEx::OnClose();
//}


void CMiniModeDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	CRect rect;
	GetWindowRect(rect);
	m_position_x = rect.left;
	m_position_y = rect.top;
	SaveConfig();
	KillTimer(TIMER_ID_MINI);
	m_menu.DestroyMenu();
	ReleaseDC(m_spectrum_pDC);
}


void CMiniModeDlg::OnMove(int x, int y)
{
	CDialogEx::OnMove(x, y);

	// TODO: 在此处添加消息处理程序代码
	CheckWindowPos();
}


void CMiniModeDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//设置点击鼠标右键弹出菜单
	CMenu* pContextMenu = m_menu.GetSubMenu(0); //获取第一个弹出菜单，所以第一个菜单必须有子菜单 
	CPoint point1;	//定义一个用于确定光标位置的位置  
	GetCursorPos(&point1);	//获取当前光标的位置，以便使得菜单可以跟随光标  
	pContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, this); //在指定位置显示弹出菜单

	CDialogEx::OnRButtonUp(nFlags, point);
}


void CMiniModeDlg::OnMiniModeExit()
{
	// TODO: 在此添加命令处理程序代码
	//执行退出命令时让对话框返回IDCANCEL，主窗口检测到这个返回值时就退出程序。
	OnCancel();
}


//void CMiniModeDlg::OnSongInfo()
//{
//	// TODO: 在此添加命令处理程序代码
//	CPropertyDlg propertyDlg(theApp.m_player.GetPlayList());
//	propertyDlg.m_path = theApp.m_player.GetCurrentPath();
//	propertyDlg.m_index = theApp.m_player.GetIndex();
//	propertyDlg.m_song_num = theApp.m_player.GetSongNum();
//	propertyDlg.DoModal();
//}


void CMiniModeDlg::OnInitMenu(CMenu* pMenu)
{
	CDialogEx::OnInitMenu(pMenu);

	// TODO: 在此处添加消息处理程序代码

	//设置迷你模式窗口右键菜单
	//设置循环模式菜单的单选标记
	RepeatMode repeat_mode{ theApp.m_player.GetRepeatMode() };
	switch (repeat_mode)
	{
	case RM_PLAY_ORDER: pMenu->CheckMenuRadioItem(ID_PLAY_ORDER, ID_LOOP_TRACK, ID_PLAY_ORDER, MF_BYCOMMAND | MF_CHECKED); break;
	case RM_PLAY_SHUFFLE: pMenu->CheckMenuRadioItem(ID_PLAY_ORDER, ID_LOOP_TRACK, ID_PLAY_SHUFFLE, MF_BYCOMMAND | MF_CHECKED); break;
	case RM_LOOP_PLAYLIST: pMenu->CheckMenuRadioItem(ID_PLAY_ORDER, ID_LOOP_TRACK, ID_LOOP_PLAYLIST, MF_BYCOMMAND | MF_CHECKED); break;
	case RM_LOOP_TRACK: pMenu->CheckMenuRadioItem(ID_PLAY_ORDER, ID_LOOP_TRACK, ID_LOOP_TRACK, MF_BYCOMMAND | MF_CHECKED); break;
	default: break;
	}

	//设置复选菜单项的勾选
	pMenu->CheckMenuItem(ID_DARK_MODE, MF_BYCOMMAND | (m_dark_mode ? MF_CHECKED : MF_UNCHECKED));
	pMenu->CheckMenuItem(ID_FOLLOW_MAIN_COLOR, MF_BYCOMMAND | (m_follow_main_color ? MF_CHECKED : MF_UNCHECKED));

	//设置播放列表右键菜单
	//弹出播放列表右键菜单时，如果没有选中播放列表中的项目，则禁用右键菜单中“播放”、“从列表中删除”、“属性”、“从磁盘删除”项目。
	if (m_item_selected < 0 || m_item_selected >= theApp.m_player.GetSongNum())
	{
		pMenu->EnableMenuItem(ID_PLAY_ITEM, MF_BYCOMMAND | MF_GRAYED);
		pMenu->EnableMenuItem(ID_REMOVE_FROM_PLAYLIST, MF_BYCOMMAND | MF_GRAYED);
		pMenu->EnableMenuItem(ID_ITEM_PROPERTY, MF_BYCOMMAND | MF_GRAYED);
		pMenu->EnableMenuItem(ID_DELETE_FROM_DISK, MF_BYCOMMAND | MF_GRAYED);
	}
	else
	{
		pMenu->EnableMenuItem(ID_PLAY_ITEM, MF_BYCOMMAND | MF_ENABLED);
		pMenu->EnableMenuItem(ID_REMOVE_FROM_PLAYLIST, MF_BYCOMMAND | MF_ENABLED);
		pMenu->EnableMenuItem(ID_ITEM_PROPERTY, MF_BYCOMMAND | MF_ENABLED);
		pMenu->EnableMenuItem(ID_DELETE_FROM_DISK, MF_BYCOMMAND | MF_ENABLED);
	}
	//设置播放列表右键菜单的默认菜单项
	pMenu->SetDefaultItem(ID_PLAY_ITEM);

}


void CMiniModeDlg::OnSetTextColor()
{
	// TODO: 在此添加命令处理程序代码
	CColorDialog colorDlg(m_theme_color.original_color, 0, this);
	if (colorDlg.DoModal() == IDOK)
	{
		m_theme_color.original_color = colorDlg.GetColor();
		ColorChanged();
		RePaint();
		//m_lyric_static.SetTextColor(m_item_text_color);
		//m_time_static.SetTextColor(m_item_text_color);
		//DrawBorder();
		SaveConfig();
		m_follow_main_color = false;
	}
}


void CMiniModeDlg::OnBnClickedMPlayPause()
{
	// TODO: 在此添加控件通知处理程序代码
	SetFocus();
	SendMessage(WM_COMMAND, ID_PLAY_PAUSE);
}


void CMiniModeDlg::OnBnClickedMPrevious()
{
	// TODO: 在此添加控件通知处理程序代码
	SetFocus();
	SendMessage(WM_COMMAND, ID_PREVIOUS);
}


void CMiniModeDlg::OnBnClickedMNext()
{
	// TODO: 在此添加控件通知处理程序代码
	SetFocus();
	SendMessage(WM_COMMAND, ID_NEXT);
}


BOOL CMiniModeDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (zDelta > 0)
	{
		SetVolume(true);
	}
	if (zDelta < 0)
	{
		SetVolume(false);
	}

	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}


void CMiniModeDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	OnOK();

	CDialogEx::OnLButtonDblClk(nFlags, point);
}


void CMiniModeDlg::OnNMDblclkList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	int row = pNMItemActivate->iItem;
	theApp.m_player.PlayTrack(row);
	//SwitchTrack();
	SetPlayListColor();
	RePaint();
	UpdatePlayPauseButton();
	*pResult = 0;
}


void CMiniModeDlg::OnNMRClickList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	m_item_selected = pNMItemActivate->iItem;	//获取鼠标选中的项目

	CMenu* pContextMenu = m_popup_menu.GetSubMenu(0); //获取第一个弹出菜单
	CPoint point;			//定义一个用于确定光标位置的位置  
	GetCursorPos(&point);	//获取当前光标的位置，以便使得菜单可以跟随光标

	pContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this); //在指定位置显示弹出菜单
	*pResult = 0;
}


void CMiniModeDlg::OnBnClickedShowListButton()
{
	// TODO: 在此添加控件通知处理程序代码
	SetFocus();
	//切换显示播放列表
	if (m_show_playlist)
	{
		//MoveWindow(m_rect_s);
		SetWindowPos(nullptr, 0, 0, m_rect_s.Width(), m_rect_s.Height(), SWP_NOMOVE | SWP_NOZORDER);
		CheckWindowPos();
		m_show_playlist = false;
	}
	else
	{
		//MoveWindow(m_rect);
		SetWindowPos(nullptr, 0, 0, m_rect.Width(), m_rect.Height(), SWP_NOMOVE | SWP_NOZORDER);
		CheckWindowPos();
		m_show_playlist = true;
	}
	//SetBackgroundImage(m_back_img);
	//ShowInfo(true);
	RePaint();
	//DrawSpectral();
	//DrawBorder();
}


void CMiniModeDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CDialogEx::OnPaint()
}


void CMiniModeDlg::OnDarkMode()
{
	// TODO: 在此添加命令处理程序代码
	m_dark_mode = !m_dark_mode;
	ColorChanged();
	RePaint();
	//DrawBorder();
}


void CMiniModeDlg::OnFollowMainColor()
{
	// TODO: 在此添加命令处理程序代码
	m_follow_main_color = !m_follow_main_color;
	if (m_follow_main_color)
		m_theme_color = theApp.m_app_setting_data.theme_color;
	ColorChanged();
	RePaint();
}


void CMiniModeDlg::OnStnClickedMiniProgressStatic()
{
	// TODO: 在此添加控件通知处理程序代码
	int pos = m_progress_bar.GetPos();
	int song_pos = static_cast<__int64>(pos) * theApp.m_player.GetSongLength() / 1000;
	theApp.m_player.SeekTo(song_pos);
}
