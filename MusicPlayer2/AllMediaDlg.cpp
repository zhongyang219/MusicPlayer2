// AllMediaDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "AllMediaDlg.h"
#include "MusicPlayerCmdHelper.h"
#include "PropertyDlg.h"


// CAllMediaDlg 对话框

IMPLEMENT_DYNAMIC(CAllMediaDlg, CTabDlg)

CAllMediaDlg::CAllMediaDlg(CWnd* pParent /*=nullptr*/)
	: CTabDlg(IDD_ALL_MEDIA_DIALOG, pParent)
{

}

CAllMediaDlg::~CAllMediaDlg()
{
}

void CAllMediaDlg::GetSongsSelected(std::vector<wstring>& song_list) const
{
	std::vector<SongInfo> song_info_list;
	GetSongsSelected(song_info_list);
	for (const auto& song : song_info_list)
		song_list.push_back(song.file_path);
}

void CAllMediaDlg::GetSongsSelected(std::vector<SongInfo>& song_list) const
{
	song_list.clear();
	for (int index : m_selected_items)
	{
		if(index < 0 || index >= m_song_list_ctrl.GetItemCount())
			continue;
		wstring file_path = m_song_list_ctrl.GetItemText(index, COL_PATH);
		song_list.push_back(theApp.GetSongInfo(file_path));
	}
}

void CAllMediaDlg::GetCurrentSongList(std::vector<SongInfo>& song_list) const
{
	song_list.clear();
	for (int index{}; index < m_song_list_ctrl.GetItemCount(); index++)
	{
		wstring file_path = m_song_list_ctrl.GetItemText(index, COL_PATH).GetString();
		song_list.push_back(theApp.GetSongInfo(file_path));
	}
}

void CAllMediaDlg::OnTabEntered()
{
	SetButtonsEnable(m_song_list_ctrl.GetCurSel() >= 0);
	if (!m_initialized)
	{
		CWaitCursor wait_cursor;
		InitListData();
		ShowSongList();
		m_initialized = true;
	}
}

void CAllMediaDlg::InitListData()
{
	m_list_data.clear();
	for (const auto& item : theApp.m_song_data)
	{
		CListCtrlEx::RowData row_data;
		//row_data[COL_INDEX] = std::to_wstring(index);
		row_data[COL_TITLE] = item.second.GetTitle();
		row_data[COL_ARTIST] = item.second.GetArtist();
		row_data[COL_ALBUM] = item.second.GetAlbum();
		std::wstring track_str;
		if (item.second.track != 0)
			track_str = std::to_wstring(item.second.track);
		row_data[COL_TRACK] = track_str;
		row_data[COL_GENRE] = item.second.GetGenre();
		row_data[COL_YEAR] = item.second.GetYear();
		row_data[COL_PATH] = item.first;
		m_list_data.push_back(std::move(row_data));
	}
	std::sort(m_list_data.begin(), m_list_data.end(), [](const CListCtrlEx::RowData& a, const CListCtrlEx::RowData& b)
	{
		return a.at(COL_TITLE) < b.at(COL_TITLE);		//默认按标题排序
	});
	UpdateListIndex();
}

void CAllMediaDlg::UpdateListIndex()
{
	int index{ 1 };
	for (auto& item : m_list_data)
	{
		item[COL_INDEX] = std::to_wstring(index);
		index++;
	}
}

void CAllMediaDlg::ShowSongList()
{
	CWaitCursor wait_cursor;
	if (m_searched)
		m_song_list_ctrl.SetListData(&m_list_data_searched);
	else
		m_song_list_ctrl.SetListData(&m_list_data);
}

void CAllMediaDlg::QuickSearch(const wstring& key_word)
{
	m_list_data_searched.clear();
	for (const auto& item : m_list_data)
	{
		for (int i{ COL_INDEX + 1 }; i < COL_PATH; i++)
		{
			if (CCommon::StringFindNoCase(item.at(i), key_word) != wstring::npos)
			{
				m_list_data_searched.push_back(item);
				break;
			}
		}
	}
}

void CAllMediaDlg::SongListClicked(int index)
{
	m_selected_item = index;
	m_song_list_ctrl.GetItemSelected(m_selected_items);
	SetButtonsEnable(/*(index >=0 && index < m_song_list_ctrl.GetItemCount()) ||*/ !m_selected_items.empty());
}

void CAllMediaDlg::SetButtonsEnable(bool enable)
{
	CWnd* pParent = GetParentWindow();
	::SendMessage(pParent->GetSafeHwnd(), WM_PLAY_SELECTED_BTN_ENABLE, WPARAM(enable), 0);
}

bool CAllMediaDlg::_OnAddToNewPlaylist(std::wstring& playlist_path)
{
	auto getSongList = [&](std::vector<SongInfo>& song_list)
	{
		GetSongsSelected(song_list);
	};
	CMusicPlayerCmdHelper cmd_helper(this);
	return cmd_helper.OnAddToNewPlaylist(getSongList, playlist_path);
}

UINT CAllMediaDlg::ViewOnlineThreadFunc(LPVOID lpParam)
{
	CAllMediaDlg* pThis = (CAllMediaDlg*)(lpParam);
	if (pThis == nullptr)
		return 0;
	CCommon::SetThreadLanguage(theApp.m_general_setting_data.language);
	//此命令用于跳转到歌曲对应的网易云音乐的在线页面
	if (pThis->m_selected_item >= 0)
	{
		wstring file_path = pThis->m_song_list_ctrl.GetItemText(pThis->m_selected_item, COL_PATH).GetString();
		if (CCommon::FileExist(file_path))
		{
			SongInfo song{ theApp.GetSongInfo(file_path) };
			CMusicPlayerCmdHelper cmd_helper(pThis);
			cmd_helper.VeiwOnline(song);
		}
	}
	return 0;
}

void CAllMediaDlg::DoDataExchange(CDataExchange* pDX)
{
	CTabDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SONG_LIST, m_song_list_ctrl);
	DDX_Control(pDX, IDC_SEARCH_EDIT, m_search_edit);
}


BEGIN_MESSAGE_MAP(CAllMediaDlg, CTabDlg)
	ON_NOTIFY(HDN_ITEMCLICK, 0, &CAllMediaDlg::OnHdnItemclickSongList)
	ON_EN_CHANGE(IDC_SEARCH_EDIT, &CAllMediaDlg::OnEnChangeSearchEdit)
	ON_NOTIFY(NM_CLICK, IDC_SONG_LIST, &CAllMediaDlg::OnNMClickSongList)
	ON_NOTIFY(NM_RCLICK, IDC_SONG_LIST, &CAllMediaDlg::OnNMRClickSongList)
	ON_NOTIFY(NM_DBLCLK, IDC_SONG_LIST, &CAllMediaDlg::OnNMDblclkSongList)
	ON_MESSAGE(WM_SEARCH_EDIT_BTN_CLICKED, &CAllMediaDlg::OnSearchEditBtnClicked)
	ON_WM_INITMENU()
	ON_COMMAND(ID_PLAY_ITEM, &CAllMediaDlg::OnPlayItem)
	ON_COMMAND(ID_PLAY_ITEM_IN_FOLDER_MODE, &CAllMediaDlg::OnPlayItemInFolderMode)
	ON_COMMAND(ID_ADD_TO_NEW_PLAYLIST, &CAllMediaDlg::OnAddToNewPlaylist)
	ON_COMMAND(ID_ADD_TO_NEW_PALYLIST_AND_PLAY, &CAllMediaDlg::OnAddToNewPalylistAndPlay)
	ON_COMMAND(ID_EXPLORE_ONLINE, &CAllMediaDlg::OnExploreOnline)
	ON_COMMAND(ID_EXPLORE_TRACK, &CAllMediaDlg::OnExploreTrack)
	ON_COMMAND(ID_FORMAT_CONVERT, &CAllMediaDlg::OnFormatConvert)
	ON_COMMAND(ID_DELETE_FROM_DISK, &CAllMediaDlg::OnDeleteFromDisk)
	ON_COMMAND(ID_ITEM_PROPERTY, &CAllMediaDlg::OnItemProperty)
	ON_COMMAND(ID_COPY_TEXT, &CAllMediaDlg::OnCopyText)
END_MESSAGE_MAP()


// CAllMediaDlg 消息处理程序


BOOL CAllMediaDlg::OnInitDialog()
{
	CTabDlg::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CCommon::SetDialogFont(this, theApp.m_pMainWnd->GetFont());     //由于此对话框资源由不同语言共用，所以这里要设置一下字体

	//初始化歌曲列表
	m_song_list_ctrl.SetExtendedStyle(m_song_list_ctrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_LABELTIP);
	m_song_list_ctrl.InsertColumn(0, CCommon::LoadText(IDS_NUMBER), LVCFMT_LEFT, theApp.DPI(40));
	m_song_list_ctrl.InsertColumn(1, CCommon::LoadText(IDS_TITLE), LVCFMT_LEFT, theApp.DPI(150));
	m_song_list_ctrl.InsertColumn(2, CCommon::LoadText(IDS_ARTIST), LVCFMT_LEFT, theApp.DPI(100));
	m_song_list_ctrl.InsertColumn(3, CCommon::LoadText(IDS_ALBUM), LVCFMT_LEFT, theApp.DPI(150));
	m_song_list_ctrl.InsertColumn(4, CCommon::LoadText(IDS_TRACK_NUM), LVCFMT_LEFT, theApp.DPI(60));
	m_song_list_ctrl.InsertColumn(5, CCommon::LoadText(IDS_GENRE), LVCFMT_LEFT, theApp.DPI(100));
	m_song_list_ctrl.InsertColumn(6, CCommon::LoadText(IDS_YEAR), LVCFMT_LEFT, theApp.DPI(60));
	m_song_list_ctrl.InsertColumn(7, CCommon::LoadText(IDS_FILE_PATH), LVCFMT_LEFT, theApp.DPI(600));

	m_search_edit.SetCueBanner(CCommon::LoadText(IDS_SEARCH_HERE), TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CAllMediaDlg::OnHdnItemclickSongList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	if (phdr->hdr.hwndFrom == m_song_list_ctrl.GetHeaderCtrl()->GetSafeHwnd())
	{
		static bool ascending = false;
		ascending = !ascending;

		static int last_item = -1;
		if (last_item != phdr->iItem)
		{
			last_item = phdr->iItem;
			ascending = true;
		}

		//对列表排序
		auto& list_data{ m_searched ? m_list_data_searched : m_list_data };
		if (phdr->iItem > COL_INDEX && phdr->iItem < COL_MAX)
		{
			std::sort(list_data.begin(), list_data.end(), [&](const CListCtrlEx::RowData& a, const CListCtrlEx::RowData& b)
			{ 
				if (phdr->iItem == COL_TRACK)		//如果是对“音轨号”排序，则需要将字符串转换成数字
				{
					int index_a, index_b;
					index_a = _ttoi(a.at(phdr->iItem).c_str());
					index_b = _ttoi(b.at(phdr->iItem).c_str());
					if (ascending)
						return index_a < index_b;
					else
						return index_a > index_b;
				}
				else
				{
					if (ascending)
						return a.at(phdr->iItem) < b.at(phdr->iItem);
					else
						return a.at(phdr->iItem) > b.at(phdr->iItem);
				}
			});
			if (!m_searched)
				UpdateListIndex();
			ShowSongList();
		}
	}

	*pResult = 0;
}


void CAllMediaDlg::OnEnChangeSearchEdit()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CTabDlg::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString str;
	m_search_edit.GetWindowText(str);
	QuickSearch(wstring(str));
	m_searched = !str.IsEmpty();
	ShowSongList();
}


void CAllMediaDlg::OnNMClickSongList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	SongListClicked(pNMItemActivate->iItem);
	*pResult = 0;
}


void CAllMediaDlg::OnNMRClickSongList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	SongListClicked(pNMItemActivate->iItem);
	m_selected_string = m_song_list_ctrl.GetItemText(pNMItemActivate->iItem, pNMItemActivate->iSubItem);

	if (pNMItemActivate->iItem >= 0)
	{
		//弹出右键菜单
		CMenu* pMenu = theApp.m_menu_set.m_media_lib_popup_menu.GetSubMenu(1);
		ASSERT(pMenu != nullptr);
		if (pMenu != nullptr)
		{
			m_song_list_ctrl.ShowPopupMenu(pMenu, pNMItemActivate->iItem, this);
		}
	}
	*pResult = 0;
}


void CAllMediaDlg::OnNMDblclkSongList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
	*pResult = 0;
}


void CAllMediaDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	std::vector<wstring> files;
	GetSongsSelected(files);
	if (!files.empty())
	{
		if (files.size() == 1)
			CPlayer::GetInstance().OpenFiles(files);
		else
			CPlayer::GetInstance().OpenFilesInTempPlaylist(files);

		CTabDlg::OnOK();
		CWnd* pParent = GetParentWindow();
		if (pParent != nullptr)
			::SendMessage(pParent->GetSafeHwnd(), WM_COMMAND, IDOK, 0);
	}
}


afx_msg LRESULT CAllMediaDlg::OnSearchEditBtnClicked(WPARAM wParam, LPARAM lParam)
{
	//点击搜索框中的叉按钮时清除搜索结果
	if (m_searched)
	{
		//清除搜索结果
		m_searched = false;
		m_search_edit.SetWindowText(_T(""));
		ShowSongList();
	}
	return 0;
}


void CAllMediaDlg::OnInitMenu(CMenu* pMenu)
{
	CTabDlg::OnInitMenu(pMenu);

	// TODO: 在此处添加消息处理程序代码
	pMenu->SetDefaultItem(ID_PLAY_ITEM);
}


void CAllMediaDlg::OnPlayItem()
{
	// TODO: 在此添加命令处理程序代码
	OnOK();
}


void CAllMediaDlg::OnPlayItemInFolderMode()
{
	// TODO: 在此添加命令处理程序代码
	if (m_selected_item >= 0)
	{
		std::wstring file_path = m_song_list_ctrl.GetItemText(m_selected_item, COL_PATH);
		CPlayer::GetInstance().OpenAFile(file_path, true);

		CTabDlg::OnOK();
		CWnd* pParent = GetParentWindow();
		if (pParent != nullptr)
			::SendMessage(pParent->GetSafeHwnd(), WM_COMMAND, IDOK, 0);
	}
}


void CAllMediaDlg::OnAddToNewPlaylist()
{
	// TODO: 在此添加命令处理程序代码
	wstring playlist_path;
	_OnAddToNewPlaylist(playlist_path);
}


void CAllMediaDlg::OnAddToNewPalylistAndPlay()
{
	// TODO: 在此添加命令处理程序代码
	wstring playlist_path;
	if (_OnAddToNewPlaylist(playlist_path))
	{
		CPlayer::GetInstance().SetPlaylist(playlist_path, 0, 0, false, true);
		CPlayer::GetInstance().SaveRecentPath();
		OnCancel();
	}
}


void CAllMediaDlg::OnExploreOnline()
{
	// TODO: 在此添加命令处理程序代码
	AfxBeginThread(ViewOnlineThreadFunc, (void*)this);
}


void CAllMediaDlg::OnExploreTrack()
{
	// TODO: 在此添加命令处理程序代码
	if (m_selected_item < 0)
		return;
	wstring file_path = m_song_list_ctrl.GetItemText(m_selected_item, COL_PATH).GetString();
	if (!file_path.empty())
	{
		CString str;
		str.Format(_T("/select,\"%s\""), file_path.c_str());
		ShellExecute(NULL, _T("open"), _T("explorer"), str, NULL, SW_SHOWNORMAL);
	}
}


void CAllMediaDlg::OnFormatConvert()
{
	// TODO: 在此添加命令处理程序代码
	std::vector<SongInfo> songs;
	GetSongsSelected(songs);
	CMusicPlayerCmdHelper cmd_helper(this);
	cmd_helper.FormatConvert(songs);
}


void CAllMediaDlg::OnDeleteFromDisk()
{
	// TODO: 在此添加命令处理程序代码
	vector<SongInfo> songs_selected;
	GetSongsSelected(songs_selected);
	CMusicPlayerCmdHelper helper;
	if (helper.DeleteSongsFromDisk(songs_selected))
	{
		//删除成功，则刷新列表
		auto isRemoved = [&](const CListCtrlEx::RowData& data)
		{
			for (const auto& item : songs_selected)
			{
				if (item.file_path == data.at(COL_PATH))
					return true;
			}
			return false;
		};
		auto iter_removed = std::remove_if(m_list_data.begin(), m_list_data.end(), isRemoved);
		m_list_data.erase(iter_removed, m_list_data.end());
		auto iter_removed1 = std::remove_if(m_list_data_searched.begin(), m_list_data_searched.end(), isRemoved);
		m_list_data_searched.erase(iter_removed1, m_list_data_searched.end());
		ShowSongList();
	}
}


void CAllMediaDlg::OnItemProperty()
{
	// TODO: 在此添加命令处理程序代码
	if (m_selected_item < 0)
		return;
	std::vector<SongInfo> songs;
	GetCurrentSongList(songs);
	CPropertyDlg propertyDlg(songs, this, true);
	propertyDlg.m_index = m_selected_item;
	propertyDlg.DoModal();
}


void CAllMediaDlg::OnCopyText()
{
	// TODO: 在此添加命令处理程序代码
	if (!CCommon::CopyStringToClipboard(wstring(m_selected_string)))
		MessageBox(CCommon::LoadText(IDS_COPY_CLIPBOARD_FAILED), NULL, MB_ICONWARNING);
}


BOOL CAllMediaDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	WORD command = LOWORD(wParam);
	auto getSelectedItems = [&](std::vector<SongInfo>& item_list)
	{
		GetSongsSelected(item_list);
	};
	//响应播放列表右键菜单中的“添加到播放列表”
	CMusicPlayerCmdHelper cmd_helper;
	cmd_helper.OnAddToPlaylistCommand(getSelectedItems, command);

	return CTabDlg::OnCommand(wParam, lParam);
}


void CAllMediaDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	CTabDlg::OnCancel();

	CWnd* pParent = GetParentWindow();
	if (pParent != nullptr)
		::SendMessage(pParent->GetSafeHwnd(), WM_COMMAND, IDCANCEL, 0);
}
