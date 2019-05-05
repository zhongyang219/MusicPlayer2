// CFloatPlaylistDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "CFloatPlaylistDlg.h"
#include "afxdialogex.h"


// CFloatPlaylistDlg 对话框

IMPLEMENT_DYNAMIC(CFloatPlaylistDlg, CDialog)

CFloatPlaylistDlg::CFloatPlaylistDlg(int& item_selected, vector<int>& items_selected, CMenu& popup_menu, CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_MUSICPLAYER2_DIALOG, pParent), m_item_selected{ item_selected }, m_items_selected{ items_selected }, m_popup_menu(popup_menu)
{

}

CFloatPlaylistDlg::~CFloatPlaylistDlg()
{
}

void CFloatPlaylistDlg::RefreshData()
{
	//刷新播放列表数据
	m_playlist_ctrl.ShowPlaylist(theApp.m_ui_data.display_format);
	m_playlist_ctrl.SetHightItem(CPlayer::GetInstance().GetIndex());
	m_playlist_ctrl.EnsureVisible(CPlayer::GetInstance().GetIndex(), FALSE);

	m_path_edit.SetWindowText(CPlayer::GetInstance().GetCurrentDir().c_str());
}

void CFloatPlaylistDlg::ReSizeControl(int cx, int cy)
{
	//设置“当前路径”static控件大小
	CRect rect_static;
	m_path_static.GetWindowRect(rect_static);
	rect_static.bottom = rect_static.top + m_layout.search_edit_height;
	rect_static.MoveToXY(m_layout.margin, m_layout.margin);
	m_path_static.MoveWindow(rect_static);

	//设置“选择文件夹”的大小和位置
	CRect rect_select_folder{ rect_static };
	rect_select_folder.right = cx - m_layout.margin;
	rect_select_folder.left = rect_select_folder.right - m_layout.select_folder_width;
	m_set_path_button.MoveWindow(rect_select_folder);

	//设置“当前路径”edit控件大小和位置
	CRect rect_edit{ rect_select_folder };
	rect_edit.left = rect_static.right + m_layout.margin;
	rect_edit.right = rect_select_folder.left - m_layout.margin;
	m_path_edit.MoveWindow(rect_edit);

	//设置清除搜索按钮的大小和位置
	CRect rect_clear{};
	rect_clear.top = 2 * m_layout.margin + m_layout.search_edit_height;
	rect_clear.right = cx - m_layout.margin;
	rect_clear.left = rect_clear.right - m_layout.search_edit_height;
	rect_clear.bottom = rect_clear.top + m_layout.search_edit_height;
	m_clear_search_button.MoveWindow(rect_clear);

	//
	CRect rect_search{ rect_clear };
	rect_search.left = m_layout.margin;
	rect_search.right = rect_clear.left - m_layout.margin;
	m_search_edit.MoveWindow(rect_search);

	//
	CRect rect_playlist;
	rect_playlist.top = rect_search.bottom + m_layout.margin;
	rect_playlist.left = m_layout.margin;
	rect_playlist.right = cx - m_layout.margin;
	rect_playlist.bottom = cy - m_layout.margin;
	m_playlist_ctrl.MoveWindow(rect_playlist);
	m_playlist_ctrl.AdjustColumnWidth();

}

bool CFloatPlaylistDlg::Initilized() const
{
	return m_playlist_ctrl.GetSafeHwnd()!=NULL && m_path_static.GetSafeHwnd() != NULL && m_path_edit.GetSafeHwnd() != NULL
		&& m_set_path_button.GetSafeHwnd() != NULL && m_search_edit.GetSafeHwnd() != NULL && m_clear_search_button.GetSafeHwnd() != NULL;
}

void CFloatPlaylistDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PLAYLIST_LIST, m_playlist_ctrl);
	DDX_Control(pDX, IDC_PATH_STATIC, m_path_static);
	DDX_Control(pDX, IDC_PATH_EDIT, m_path_edit);
	DDX_Control(pDX, ID_SET_PATH, m_set_path_button);
	DDX_Control(pDX, IDC_SEARCH_EDIT, m_search_edit);
	DDX_Control(pDX, IDC_CLEAR_SEARCH_BUTTON, m_clear_search_button);
}


BEGIN_MESSAGE_MAP(CFloatPlaylistDlg, CDialog)
	ON_WM_SIZE()
	ON_NOTIFY(NM_RCLICK, IDC_PLAYLIST_LIST, &CFloatPlaylistDlg::OnNMRClickPlaylistList)
	ON_NOTIFY(NM_DBLCLK, IDC_PLAYLIST_LIST, &CFloatPlaylistDlg::OnNMDblclkPlaylistList)
	ON_EN_CHANGE(IDC_SEARCH_EDIT, &CFloatPlaylistDlg::OnEnChangeSearchEdit)
	ON_BN_CLICKED(IDC_CLEAR_SEARCH_BUTTON, &CFloatPlaylistDlg::OnBnClickedClearSearchButton)
END_MESSAGE_MAP()


// CFloatPlaylistDlg 消息处理程序


BOOL CFloatPlaylistDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	RefreshData();

	//设置窗口大小
	const CSize window_size{ theApp.DPI(320), theApp.DPI(530) };
	SetWindowPos(nullptr, 0, 0, window_size.cx, window_size.cy, SWP_NOMOVE | SWP_NOZORDER);

	CRect rect;
	GetClientRect(rect);
	ReSizeControl(rect.Width(), rect.Height());

	m_search_edit.SetCueBanner(CCommon::LoadText(IDS_SEARCH_HERE), TRUE);


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CFloatPlaylistDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (nType != SIZE_MINIMIZED && Initilized())
	{
		ReSizeControl(cx, cy);
	}
}

void CFloatPlaylistDlg::OnNMRClickPlaylistList(NMHDR * pNMHDR, LRESULT * pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	if (!m_searched)
	{
		m_item_selected = pNMItemActivate->iItem;	//获取鼠标选中的项目
		m_playlist_ctrl.GetItemSelected(m_items_selected);		//获取多个选中的项目
	}
	else
	{
		CString str;
		str = m_playlist_ctrl.GetItemText(pNMItemActivate->iItem, 0);
		m_item_selected = _ttoi(str) - 1;
		m_playlist_ctrl.GetItemSelectedSearched(m_items_selected);
	}

	CMenu* pContextMenu = m_popup_menu.GetSubMenu(0);
	CPoint point;			//定义一个用于确定光标位置的位置  
	GetCursorPos(&point);	//获取当前光标的位置，以便使得菜单可以跟随光标

	if (m_item_selected >= 0 && m_item_selected < CPlayer::GetInstance().GetSongNum())
	{
		CRect item_rect;
		m_playlist_ctrl.GetItemRect(pNMItemActivate->iItem, item_rect, LVIR_BOUNDS);		//获取选中项目的矩形区域（以播放列表控件左上角为原点）
		CRect playlist_rect;
		m_playlist_ctrl.GetWindowRect(playlist_rect);		//获取播放列表控件的矩形区域（以屏幕左上角为原点）
		point.y = playlist_rect.top + item_rect.bottom;	//设置鼠标要弹出的y坐标为选中项目的下边框位置，防止右键菜单挡住选中的项目
	}

	pContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this); //在指定位置显示弹出菜单

	*pResult = 0;
}

void CFloatPlaylistDlg::OnNMDblclkPlaylistList(NMHDR * pNMHDR, LRESULT * pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	if (!m_searched)	//如果播放列表不在搜索状态，则当前选中项的行号就是曲目的索引
	{
		if (pNMItemActivate->iItem < 0)
			return;
		CPlayer::GetInstance().PlayTrack(pNMItemActivate->iItem);
	}
	else		//如果播放列表处理选中状态，则曲目的索引是选中行第一列的数字-1
	{
		int song_index;
		CString str;
		str = m_playlist_ctrl.GetItemText(pNMItemActivate->iItem, 0);
		song_index = _ttoi(str) - 1;
		if (song_index < 0)
			return;
		CPlayer::GetInstance().PlayTrack(song_index);
	}

	*pResult = 0;
}

void CFloatPlaylistDlg::OnEnChangeSearchEdit()
{
	CString key_word;
	m_search_edit.GetWindowText(key_word);
	m_searched = (key_word.GetLength() != 0);
	m_playlist_ctrl.QuickSearch(wstring(key_word));
	m_playlist_ctrl.ShowPlaylist(theApp.m_ui_data.display_format, m_searched);
}

void CFloatPlaylistDlg::OnBnClickedClearSearchButton()
{
	if (m_searched)
	{
		//清除搜索结果
		m_searched = false;
		m_search_edit.SetWindowText(_T(""));
		m_playlist_ctrl.ShowPlaylist(theApp.m_ui_data.display_format, m_searched);
		m_playlist_ctrl.EnsureVisible(CPlayer::GetInstance().GetIndex(), FALSE);		//清除搜索结果后确保正在播放曲目可见
	}
}
