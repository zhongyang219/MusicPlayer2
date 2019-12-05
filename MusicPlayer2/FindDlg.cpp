// FindDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "FindDlg.h"
#include "afxdialogex.h"


// CFindDlg 对话框

IMPLEMENT_DYNAMIC(CFindDlg, CDialog)

CFindDlg::CFindDlg(const vector<SongInfo>& playlist, CWnd* pParent /*=NULL*/)
	: CDialog(IDD_FIND_DIALOG, pParent), m_playlist{ playlist }
{

}

CFindDlg::~CFindDlg()
{
}

void CFindDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FIND_LIST, m_find_result_list);
	DDX_Control(pDX, IDC_FIND_FILE_CHECK, m_find_file_check);
	DDX_Control(pDX, IDC_FIND_TITLE_CHECK, m_find_title_check);
	DDX_Control(pDX, IDC_FIND_ARTIST_CHECK, m_find_artist_check);
	DDX_Control(pDX, IDC_FIND_ALBUM_CHECK, m_find_album_check);
}

void CFindDlg::ShowFindResult()
{
	m_find_result_list.DeleteAllItems();
	CString str;
	if (m_find_current_playlist)
	{
		for (size_t i{}; i<m_find_result.size(); i++)
		{
			str.Format(_T("%u"), m_find_result[i] + 1);
			m_find_result_list.InsertItem(i, str);
			m_find_result_list.SetItemText(i, 1, m_playlist[m_find_result[i]].GetFileName().c_str());
			m_find_result_list.SetItemText(i, 2, m_playlist[m_find_result[i]].title.c_str());
			m_find_result_list.SetItemText(i, 3, m_playlist[m_find_result[i]].artist.c_str());
			m_find_result_list.SetItemText(i, 4, m_playlist[m_find_result[i]].album.c_str());
			m_find_result_list.SetItemText(i, 5, m_playlist[m_find_result[i]].file_path.c_str());
		}
	}
	else
	{
		for (size_t i{}; i < m_all_find_result.size(); i++)
		{
			str.Format(_T("%u"), i + 1);
			m_find_result_list.InsertItem(i, str);
			int index = m_all_find_result[i].rfind(L'\\');
			wstring file_name = m_all_find_result[i].substr(index + 1);
			m_find_result_list.SetItemText(i, 1, file_name.c_str());
			m_find_result_list.SetItemText(i, 2, theApp.m_song_data[m_all_find_result[i]].title.c_str());
			m_find_result_list.SetItemText(i, 3, theApp.m_song_data[m_all_find_result[i]].artist.c_str());
			m_find_result_list.SetItemText(i, 4, theApp.m_song_data[m_all_find_result[i]].album.c_str());
			m_find_result_list.SetItemText(i, 5, m_all_find_result[i].c_str());
		}
	}
}

void CFindDlg::ShowFindInfo()
{
	CString str;
	int result_mun;
	if (m_find_current_playlist)
		result_mun = m_find_result.size();
	else
		result_mun = m_all_find_result.size();
	str = CCommon::LoadTextFormat(IDS_FIND_DLG_INFO, { m_key_word, result_mun });
	SetDlgItemText(IDC_FIND_INFO_STATIC, str);
}

void CFindDlg::ClearFindResult()
{
	m_find_result.clear();
}

void CFindDlg::SaveConfig()
{
	m_find_option_data = 0;
	if (m_find_file)
		m_find_option_data |= 0x01;
	if (m_find_title)
		m_find_option_data |= 0x02;
	if (m_find_artist)
		m_find_option_data |= 0x04;
	if (m_find_album)
		m_find_option_data |= 0x08;
	if(m_find_current_playlist)
		m_find_option_data |= 0x10;

	CIniHelper ini(theApp.m_config_path);
	ini.WriteInt(L"config", L"find_option_data", m_find_option_data);
	ini.Save();
}

void CFindDlg::LoadConfig()
{
	CIniHelper ini(theApp.m_config_path);
	m_find_option_data = ini.GetInt(L"config", L"find_option_data", 0xff);

	m_find_file = (m_find_option_data % 2 != 0);
	m_find_title = ((m_find_option_data >> 1) % 2 != 0);
	m_find_artist = ((m_find_option_data >> 2) % 2 != 0);
	m_find_album = ((m_find_option_data >> 3) % 2 != 0);
	m_find_current_playlist = ((m_find_option_data >> 4) % 2 != 0);
}

int CFindDlg::GetSelectedTrack() const
{
	if (m_item_selected >= 0 && m_item_selected < static_cast<int>(m_find_result.size()))
		return m_find_result[m_item_selected];
	else
		return -1;
}

bool CFindDlg::GetFindCurrentPlaylist() const
{
	return m_find_current_playlist;
}

wstring CFindDlg::GetSelectedSongPath() const
{
	if (m_item_selected >= 0 && m_item_selected < static_cast<int>(m_all_find_result.size()))
		return m_all_find_result[m_item_selected];
	else
		return wstring();
}


BEGIN_MESSAGE_MAP(CFindDlg, CDialog)
	ON_EN_CHANGE(IDC_FIND_EDIT, &CFindDlg::OnEnChangeFindEdit)
	ON_NOTIFY(NM_CLICK, IDC_FIND_LIST, &CFindDlg::OnNMClickFindList)
	ON_BN_CLICKED(IDC_FIND_BUTTON, &CFindDlg::OnBnClickedFindButton)
	ON_NOTIFY(NM_DBLCLK, IDC_FIND_LIST, &CFindDlg::OnNMDblclkFindList)
	ON_BN_CLICKED(IDC_FIND_FILE_CHECK, &CFindDlg::OnBnClickedFindFileCheck)
	ON_BN_CLICKED(IDC_FIND_TITLE_CHECK, &CFindDlg::OnBnClickedFindTitleCheck)
	ON_BN_CLICKED(IDC_FIND_ARTIST_CHECK, &CFindDlg::OnBnClickedFindArtistCheck)
	ON_BN_CLICKED(IDC_FIND_ALBUM_CHECK, &CFindDlg::OnBnClickedFindAlbumCheck)
	ON_BN_CLICKED(IDC_FIND_CURRENT_PLAYLIST_RADIO, &CFindDlg::OnBnClickedFindCurrentPlaylistRadio)
	ON_BN_CLICKED(IDC_FIND_ALL_PLAYLIST_RADIO, &CFindDlg::OnBnClickedFindAllPlaylistRadio)
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_COMMAND(ID_FD_PLAY, &CFindDlg::OnFdPlay)
	ON_COMMAND(ID_FD_OPEN_FILE_LOCATION, &CFindDlg::OnFdOpenFileLocation)
	ON_NOTIFY(NM_RCLICK, IDC_FIND_LIST, &CFindDlg::OnNMRClickFindList)
	ON_COMMAND(ID_FD_COPY_TEXT, &CFindDlg::OnFdCopyText)
END_MESSAGE_MAP()


// CFindDlg 消息处理程序

void CFindDlg::OnEnChangeFindEdit()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString find_string;
	GetDlgItemText(IDC_FIND_EDIT, find_string);
	m_key_word = find_string;
}


void CFindDlg::OnNMClickFindList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	//NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	m_item_selected = pNMItemActivate->iItem;		//单击查找结果列表时保存选中的项目序号
	GetDlgItem(IDOK)->EnableWindow(m_item_selected != -1);
	*pResult = 0;
}


void CFindDlg::OnBnClickedFindButton()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_key_word.empty())
	{
		if (m_find_current_playlist)		//查找当前播放列表时，在m_playlist窗口中查找
		{
			m_find_result.clear();
			int index;
			bool find_flag;
			for (size_t i{ 0 }; i < m_playlist.size(); i++)
			{
				find_flag = false;
				if (m_find_file && !find_flag)
				{
					index = CCommon::StringFindNoCase(m_playlist[i].GetFileName(), m_key_word);
					if (index != string::npos) find_flag = true;
				}
				if (m_find_title && !find_flag)
				{
					index = CCommon::StringFindNoCase(m_playlist[i].title, m_key_word);
					if (index != string::npos) find_flag = true;
				}
				if (m_find_artist && !find_flag)
				{
					index = CCommon::StringFindNoCase(m_playlist[i].artist, m_key_word);
					if (index != string::npos) find_flag = true;
				}
				if (m_find_album && !find_flag)
				{
					index = CCommon::StringFindNoCase(m_playlist[i].album, m_key_word);
					if (index != string::npos) find_flag = true;
				}

				if (find_flag)
					m_find_result.push_back(i);		//如果找到了，保存播放列表中的曲目序号
			}
			ShowFindResult();
			if (!m_find_result.empty())
				SetDlgItemText(IDC_FIND_RESULT_STATIC, CCommon::LoadText(IDS_FIND_RESULT, _T(": ")));
			else
				SetDlgItemText(IDC_FIND_RESULT_STATIC, CCommon::LoadText(IDS_NO_RESULT));
		}
		else			//查找所有播放列表时，在theApp.m_song_data窗口中查找
		{
			m_all_find_result.clear();
			wstring a_result;
			int index;
			bool find_flag;
			for (const auto& item : theApp.m_song_data)
			{
				find_flag = false;
				if (m_find_file && !find_flag)
				{
                    index = CCommon::StringFindNoCase(item.second.GetFileName(), m_key_word);
                    if (index != string::npos) find_flag = true;
				}
				if (m_find_title && !find_flag)
				{
					index = CCommon::StringFindNoCase(item.second.title, m_key_word);
					if (index != string::npos) find_flag = true;
				}
				if (m_find_artist && !find_flag)
				{
                    index = CCommon::StringFindNoCase(item.second.artist, m_key_word);
					if (index != string::npos) find_flag = true;
				}
				if (m_find_album && !find_flag)
				{
					index = CCommon::StringFindNoCase(item.second.album, m_key_word);
					if (index != string::npos) find_flag = true;
				}
				if (find_flag)
					m_all_find_result.push_back(item.first);		//如果找到了，就保存歌曲的绝对路径
			}
			ShowFindResult();
			if (!m_all_find_result.empty())
				SetDlgItemText(IDC_FIND_RESULT_STATIC, CCommon::LoadText(IDS_FIND_RESULT, _T(": ")));
			else
				SetDlgItemText(IDC_FIND_RESULT_STATIC, CCommon::LoadText(IDS_NO_RESULT));
		}
		m_item_selected = -1;
		ShowFindInfo();
	}
}


BOOL CFindDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);		// 设置小图标

	//设置主题颜色
	//m_find_result_list.SetColor(theApp.m_app_setting_data.theme_color);

	//设置查找选项复选按钮的状态
	m_find_file_check.SetCheck(m_find_file);
	m_find_title_check.SetCheck(m_find_title);
	m_find_artist_check.SetCheck(m_find_artist);
	m_find_album_check.SetCheck(m_find_album);

	if (m_find_current_playlist)
		((CButton*)GetDlgItem(IDC_FIND_CURRENT_PLAYLIST_RADIO))->SetCheck(TRUE);
	else
		((CButton*)GetDlgItem(IDC_FIND_ALL_PLAYLIST_RADIO))->SetCheck(TRUE);

	//初始化列表控件
	CRect rect;
	m_find_result_list.GetClientRect(rect);
    m_find_result_list.SetExtendedStyle(m_find_result_list.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_LABELTIP);
	int list_width{ rect.Width() - theApp.DPI(20) - 1 };		//列表控件宽度减去留给垂直滚动条的宽度余量
	int width0, width1, width2;
	width0 = theApp.DPI(40);
	width2 = (list_width - width0) / 5;
	width1 = list_width - width0 - width2 * 4;
	m_find_result_list.InsertColumn(0, CCommon::LoadText(IDS_NUMBER), LVCFMT_LEFT, width0);		//插入第0列
	m_find_result_list.InsertColumn(1, CCommon::LoadText(IDS_FILE_NAME), LVCFMT_LEFT, width1);		//插入第1列
	m_find_result_list.InsertColumn(2, CCommon::LoadText(IDS_TITLE), LVCFMT_LEFT, width2);		//插入第2列
	m_find_result_list.InsertColumn(3, CCommon::LoadText(IDS_ARTIST), LVCFMT_LEFT, width2);		//插入第3列
	m_find_result_list.InsertColumn(4, CCommon::LoadText(IDS_ALBUM), LVCFMT_LEFT, width2);		//插入第4列
	m_find_result_list.InsertColumn(5, CCommon::LoadText(IDS_FILE_PATH), LVCFMT_LEFT, width2);		//插入第5列

	ShowFindResult();	//显示（上一次的）查找结果
	ShowFindInfo();

	m_key_word.clear();

	GetDlgItem(IDC_FIND_EDIT)->SetFocus();		//将焦点设置到搜索框

	//设置列表控件的提示总是置顶，用于解决如果弹出此窗口的父窗口具有置顶属性时，提示信息在窗口下面的问题
	m_find_result_list.GetToolTips()->SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	GetWindowRect(rect);
	m_min_width = rect.Width();
	m_min_height = rect.Height();

	//初始化右键菜单
	if (m_menu.m_hMenu == NULL)
		m_menu.LoadMenu(IDR_FIND_POPUP_MENU);
	m_menu.GetSubMenu(0)->SetDefaultItem(ID_FD_PLAY);

	GetDlgItem(IDOK)->EnableWindow(FALSE);	//禁用“播放选中曲目”按钮，除非选中了一个项目

	return FALSE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CFindDlg::OnNMDblclkFindList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	m_item_selected = pNMItemActivate->iItem;
	GetDlgItem(IDOK)->EnableWindow(m_item_selected != -1);

	//双击列表项目后关闭对话框并播放选中项目
	OnFdPlay();
	*pResult = 0;
}


BOOL CFindDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYUP && pMsg->wParam == VK_RETURN)		//按回车键执行搜索操作
	{
		OnBnClickedFindButton();
		return TRUE;
	}
	if (pMsg->wParam == VK_RETURN)		//屏蔽按回车键退出
	{
		return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}


void CFindDlg::OnBnClickedFindFileCheck()
{
	// TODO: 在此添加控件通知处理程序代码
	m_find_file = (m_find_file_check.GetCheck() != 0);
}


void CFindDlg::OnBnClickedFindTitleCheck()
{
	// TODO: 在此添加控件通知处理程序代码
	m_find_title = (m_find_title_check.GetCheck() != 0);
}


void CFindDlg::OnBnClickedFindArtistCheck()
{
	// TODO: 在此添加控件通知处理程序代码
	m_find_artist = (m_find_artist_check.GetCheck() != 0);
}


void CFindDlg::OnBnClickedFindAlbumCheck()
{
	// TODO: 在此添加控件通知处理程序代码
	m_find_album = (m_find_album_check.GetCheck() != 0);
}



void CFindDlg::OnBnClickedFindCurrentPlaylistRadio()
{
	// TODO: 在此添加控件通知处理程序代码
	m_find_current_playlist = true;
}


void CFindDlg::OnBnClickedFindAllPlaylistRadio()
{
	// TODO: 在此添加控件通知处理程序代码
	m_find_current_playlist = false;
}


void CFindDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//限制窗口最小大小
	lpMMI->ptMinTrackSize.x = m_min_width;		//设置最小宽度
	lpMMI->ptMinTrackSize.y = m_min_height;		//设置最小高度

	CDialog::OnGetMinMaxInfo(lpMMI);
}


void CFindDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if (m_find_result_list.m_hWnd != NULL&&nType != SIZE_MINIMIZED)
	{
		//调整列表中项目的宽度
		CRect rect;
		m_find_result_list.GetWindowRect(rect);
		int list_width{ rect.Width() - theApp.DPI(20) - 1 };		//列表控件宽度减去留给垂直滚动条的宽度余量
		int width0, width1, width2;
		width0 = theApp.DPI(40);
		width2 = (list_width - width0) / 5;
		width1 = list_width - width0 - width2 * 4;
		m_find_result_list.SetColumnWidth(1, width1);
		m_find_result_list.SetColumnWidth(2, width2);
		m_find_result_list.SetColumnWidth(3, width2);
		m_find_result_list.SetColumnWidth(4, width2);
		m_find_result_list.SetColumnWidth(5, width2);
	}
}


void CFindDlg::OnFdPlay()
{
	// TODO: 在此添加命令处理程序代码
	if (m_find_current_playlist)
	{
		if (m_item_selected >= 0 && m_item_selected < static_cast<int>(m_find_result.size()))
			OnOK();
	}
	else
	{
		if (m_item_selected >= 0 && m_item_selected < static_cast<int>(m_all_find_result.size()))
			OnOK();
	}
}


void CFindDlg::OnFdOpenFileLocation()
{
	// TODO: 在此添加命令处理程序代码
	wstring file;
	if (m_find_current_playlist)
	{
		if (m_item_selected >= 0 && m_item_selected < static_cast<int>(m_find_result.size()))
			file = m_playlist[m_find_result[m_item_selected]].file_path;
		else
			return;
	}
	else
	{
		if (m_item_selected >= 0 && m_item_selected < static_cast<int>(m_all_find_result.size()))
			file = m_all_find_result[m_item_selected];
		else
			return;
	}
	CString str;
	str.Format(_T("/select,\"%s\""), file.c_str());
	ShellExecute(NULL, _T("open"), _T("explorer"), str, NULL, SW_SHOWNORMAL);
}


void CFindDlg::OnNMRClickFindList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	m_item_selected = pNMItemActivate->iItem;
	GetDlgItem(IDOK)->EnableWindow(m_item_selected != -1);

	if (m_find_current_playlist && (m_item_selected >= 0 && m_item_selected < static_cast<int>(m_find_result.size()))
		|| !m_find_current_playlist && (m_item_selected >= 0 && m_item_selected < static_cast<int>(m_all_find_result.size())))
	{
		//获取鼠标点击处的文本
		int sub_item;
		sub_item = pNMItemActivate->iSubItem;
		m_selected_string = m_find_result_list.GetItemText(m_item_selected, sub_item);
		//弹出右键菜单
		CMenu* pContextMenu = m_menu.GetSubMenu(0);	//获取第一个弹出菜单
		CPoint point1;	//定义一个用于确定光标位置的位置  
		GetCursorPos(&point1);	//获取当前光标的位置，以便使得菜单可以跟随光标
		pContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, this); //在指定位置显示弹出菜单
	}

	*pResult = 0;
}


void CFindDlg::OnFdCopyText()
{
	// TODO: 在此添加命令处理程序代码
	if (!CCommon::CopyStringToClipboard(wstring(m_selected_string)))
		MessageBox(CCommon::LoadText(IDS_COPY_CLIPBOARD_FAILED), NULL, MB_ICONWARNING);
}
