// MiniModeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "MiniModeDlg.h"
#include "afxdialogex.h"


// CMiniModeDlg 对话框

IMPLEMENT_DYNAMIC(CMiniModeDlg, CDialogEx)

CMiniModeDlg::CMiniModeDlg(int& item_selected, vector<int>& items_selected, CMenu& popup_menu, CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MINI_DIALOG, pParent), m_item_selected{ item_selected }, m_items_selected{ items_selected }, m_popup_menu{ popup_menu }
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
	DDX_Control(pDX, IDC_LIST2, m_playlist_ctrl);
}

void CMiniModeDlg::SaveConfig() const
{
	CCommon::WritePrivateProfileIntW(L"mini_mode", L"position_x", m_position_x, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"mini_mode", L"position_y", m_position_y, theApp.m_config_path.c_str());
}

void CMiniModeDlg::LoadConfig()
{
	m_position_x = GetPrivateProfileInt(_T("mini_mode"), _T("position_x"), -1, theApp.m_config_path.c_str());
	m_position_y = GetPrivateProfileInt(_T("mini_mode"), _T("position_y"), -1, theApp.m_config_path.c_str());
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
	m_ui_data.m_song_tip_info = _T("");
	m_ui_data.m_song_tip_info += _T("当前播放：");
	m_ui_data.m_song_tip_info += theApp.m_player.GetFileName().c_str();
	m_ui_data.m_song_tip_info += _T("\r\n");
	m_ui_data.m_song_tip_info += _T("标题：");
	m_ui_data.m_song_tip_info += theApp.m_player.GetPlayList()[theApp.m_player.GetIndex()].title.c_str();
	m_ui_data.m_song_tip_info += _T("\r\n");
	m_ui_data.m_song_tip_info += _T("艺术家：");
	m_ui_data.m_song_tip_info += theApp.m_player.GetPlayList()[theApp.m_player.GetIndex()].artist.c_str();
	m_ui_data.m_song_tip_info += _T("\r\n");
	m_ui_data.m_song_tip_info += _T("唱片集：");
	m_ui_data.m_song_tip_info += theApp.m_player.GetPlayList()[theApp.m_player.GetIndex()].album.c_str();
	m_ui_data.m_song_tip_info += _T("\r\n");
}


BEGIN_MESSAGE_MAP(CMiniModeDlg, CDialogEx)
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_MOVE()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_MINI_MODE_EXIT, &CMiniModeDlg::OnMiniModeExit)
	ON_WM_INITMENU()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDBLCLK()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST2, &CMiniModeDlg::OnNMDblclkList2)
	ON_NOTIFY(NM_RCLICK, IDC_LIST2, &CMiniModeDlg::OnNMRClickList2)
	ON_WM_PAINT()
	//ON_STN_CLICKED(IDC_MINI_PROGRESS_STATIC, &CMiniModeDlg::OnStnClickedMiniProgressStatic)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_SHOW_PLAY_LIST, &CMiniModeDlg::OnShowPlayList)
	ON_WM_MOUSELEAVE()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()


// CMiniModeDlg 消息处理程序


void CMiniModeDlg::ShowPlaylist()
{
	m_playlist_ctrl.ShowPlaylist(*m_ui_data.pDisplayFormat);
	SetPlayListColor();
}

void CMiniModeDlg::SetPlayListColor()
{
	m_playlist_ctrl.SetHightItem(theApp.m_player.GetIndex());
	m_playlist_ctrl.SetColor(theApp.m_app_setting_data.theme_color);
	m_playlist_ctrl.EnsureVisible(theApp.m_player.GetIndex(), FALSE);
}

void CMiniModeDlg::SetDefaultBackGround(CImage * pImage)
{
	m_ui_data.pDefaultBackground = pImage;
}

void CMiniModeDlg::SetDisplayFormat(DisplayFormat * pDisplayFormat)
{
	m_ui_data.pDisplayFormat = pDisplayFormat;
}

BOOL CMiniModeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	LoadConfig();

	SetWindowText(_T("MiniDlg_ByH87M"));		//为对话框设置一个标题，用于通过FindWindow函数查找

	m_ui_data.Init();
	m_pDC = GetDC();
	m_ui.Init(m_pDC);

	//初始化鼠标提示
	m_Mytip.Create(this, TTS_ALWAYSTIP);
	m_Mytip.SetMaxTipWidth(800);

	UpdateSongTipInfo();
	//m_Mytip.AddTool(this, m_ui_data.m_song_tip_info);
	m_ui.SetToolTip(&m_Mytip);

	//初始化窗口位置
	if (m_position_x != -1 && m_position_y != -1)
		SetWindowPos(nullptr , m_position_x, m_position_y, m_ui_data.widnow_width, m_ui_data.window_height, SWP_NOZORDER);
	else
		SetWindowPos(nullptr , 0, 0, m_ui_data.widnow_width, m_ui_data.window_height, SWP_NOMOVE | SWP_NOZORDER);
	CheckWindowPos();

	//初始化播放列表控件的位置
	CRect playlist_rect;
	int margin = 0;
	playlist_rect.left = margin;
	playlist_rect.right = m_ui_data.widnow_width - margin;
	playlist_rect.top = m_ui_data.window_height + margin;
	playlist_rect.bottom = m_ui_data.window_height2 - margin;
	m_playlist_ctrl.MoveWindow(playlist_rect);
	m_playlist_ctrl.AdjustColumnWidth();

	//装载右键菜单
	m_menu.LoadMenu(IDR_MINI_MODE_MENU);

	//设置定时器
	SetTimer(TIMER_ID_MINI, TIMER_ELAPSE_MINI, NULL);	//设置主定时器
	SetTimer(TIMER_ID_MINI2, TIMER_ELAPSE, NULL);		//设置用于界面刷新的定时器

	//显示播放列表
	ShowPlaylist();

	m_show_playlist = false;
	m_ui_data.m_show_volume = false;

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CMiniModeDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(!m_ui.PointInControlArea(point))
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
			//m_Mytip.UpdateTipText(m_song_tip_info, this);
			index = theApp.m_player.GetIndex();
			song_name = theApp.m_player.GetFileName();
		}
		
	}
	if (nIDEvent == TIMER_ID_MINI2)
	{
		m_ui.DrawInfo(false);
	}
	if (nIDEvent == 11)
	{
		m_ui_data.m_show_volume = false;
		KillTimer(11);
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CMiniModeDlg::SetVolume(bool up)
{
	if (up)
		theApp.m_player.MusicControl(Command::VOLUME_UP);
	else
		theApp.m_player.MusicControl(Command::VOLUME_DOWN);
	//ShowVolume(theApp.m_player.GetVolume());
	KillTimer(11);
	SetTimer(11, 1500, NULL);		//显示音量后设置一个1500毫秒的定时器（音量显示保持1.5秒）
	m_ui_data.m_show_volume = true;
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
	{
		//POINT point = pMsg->pt;
		//ScreenToClient(&point);

		//if (!m_ui.PointInControlArea(point))
			m_Mytip.RelayEvent(pMsg);
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


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
	ReleaseDC(m_pDC);
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
	pMenu->CheckMenuItem(ID_SHOW_PLAY_LIST, MF_BYCOMMAND | (m_show_playlist ? MF_CHECKED : MF_UNCHECKED));

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

	pMenu->EnableMenuItem(ID_DOWNLOAD_ALBUM_COVER, MF_BYCOMMAND | (!theApp.m_player.IsInnerCover() ? MF_ENABLED : MF_GRAYED));
	bool midi_lyric{ theApp.m_player.IsMidi() && theApp.m_general_setting_data.midi_use_inner_lyric && !theApp.m_player.MidiNoLyric() };
	pMenu->EnableMenuItem(ID_DOWNLOAD_LYRIC, MF_BYCOMMAND | (!midi_lyric ? MF_ENABLED : MF_GRAYED));

	//设置播放列表右键菜单的默认菜单项
	pMenu->SetDefaultItem(ID_PLAY_ITEM);

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
	//if(!m_ui.PointInControlArea(point))
	//	OnOK();

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
	//RePaint();
	*pResult = 0;
}


void CMiniModeDlg::OnNMRClickList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	m_item_selected = pNMItemActivate->iItem;	//获取鼠标选中的项目
	m_playlist_ctrl.GetItemSelected(m_items_selected);

	CMenu* pContextMenu = m_popup_menu.GetSubMenu(0); //获取第一个弹出菜单
	CPoint point;			//定义一个用于确定光标位置的位置  
	GetCursorPos(&point);	//获取当前光标的位置，以便使得菜单可以跟随光标

	pContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this); //在指定位置显示弹出菜单
	*pResult = 0;
}


void CMiniModeDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CDialogEx::OnPaint()
	m_ui.DrawInfo();
}


//void CMiniModeDlg::OnStnClickedMiniProgressStatic()
//{
//	// TODO: 在此添加控件通知处理程序代码
//	//int pos = m_progress_bar.GetPos();
//	//int song_pos = static_cast<__int64>(pos) * theApp.m_player.GetSongLength() / 1000;
//	//theApp.m_player.SeekTo(song_pos);
//}


void CMiniModeDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_ui.MouseMove(point);

	CDialogEx::OnMouseMove(nFlags, point);
}


void CMiniModeDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_ui.LButtonUp(point);

	CDialogEx::OnLButtonUp(nFlags, point);
}


void CMiniModeDlg::OnShowPlayList()
{
	// TODO: 在此添加命令处理程序代码
	if (m_show_playlist)
	{
		SetWindowPos(nullptr, 0, 0, m_ui_data.widnow_width, m_ui_data.window_height, SWP_NOMOVE | SWP_NOZORDER);
		CheckWindowPos();
		m_show_playlist = false;
	}
	else
	{
		SetWindowPos(nullptr, 0, 0, m_ui_data.widnow_width, m_ui_data.window_height2, SWP_NOMOVE | SWP_NOZORDER);
		CheckWindowPos();
		m_show_playlist = true;
	}
}


void CMiniModeDlg::OnMouseLeave()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_ui.MouseLeave();

	CDialogEx::OnMouseLeave();
}


BOOL CMiniModeDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_ui.SetCursor())
		return TRUE;
	else	
		return CDialogEx::OnSetCursor(pWnd, nHitTest, message);
}
