// AllMediaDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "AllMediaDlg.h"
#include "MusicPlayerCmdHelper.h"
#include "PropertyDlg.h"


// CAllMediaDlg 对话框

IMPLEMENT_DYNAMIC(CAllMediaDlg, CMediaLibTabDlg)

CAllMediaDlg::CAllMediaDlg(CAllMediaDlg::DialogType type, CWnd* pParent /*=nullptr*/)
	: CMediaLibTabDlg(IDD_ALL_MEDIA_DIALOG, pParent), m_type{ type }
{

}

CAllMediaDlg::~CAllMediaDlg()
{
}

void CAllMediaDlg::RefreshData()
{
	InitListData();
	ShowSongList();
	m_initialized = true;
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

		if (m_type == DT_RECENT_MEDIA)		//如果显示最近播放曲目，则跳过没有播放过的曲目
		{
			if(item.second.last_played_time == 0)
				continue;
		}

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
		if(item.second.last_played_time != 0)
		{
			CTime played_time(item.second.last_played_time);
			wchar_t buff[64];
			swprintf_s(buff, L"%d/%.2d/%.2d %.2d:%.2d:%.2d", played_time.GetYear(), played_time.GetMonth(), played_time.GetDay(),
				played_time.GetHour(), played_time.GetMinute(), played_time.GetSecond());
			row_data[COL_LAST_PLAYED_TIME] = buff;
		}
		m_list_data.push_back(std::move(row_data));
	}
	std::sort(m_list_data.begin(), m_list_data.end(), [&](const CListCtrlEx::RowData& a, const CListCtrlEx::RowData& b)
	{
		int sort_col{ m_type == DT_RECENT_MEDIA ? COL_LAST_PLAYED_TIME : COL_TITLE };  //排序的列，显示所有曲目时默认按标题排序，显示最近曲目时默认按最近播放时间排序
		wstring str_a, str_b;
		if (a.find(sort_col) != a.end())
			str_a = a.at(sort_col);
		if (b.find(sort_col) != b.end())
			str_b = b.at(sort_col);

		if (m_type == DT_RECENT_MEDIA)
			return str_a > str_b;
		else
			return str_a < str_b;
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

const CListCtrlEx & CAllMediaDlg::GetSongListCtrl() const
{
	return m_song_list_ctrl;
}

int CAllMediaDlg::GetItemSelected() const
{
	return m_selected_item;
}

const vector<int>& CAllMediaDlg::GetItemsSelected() const
{
	return m_selected_items;
}

void CAllMediaDlg::AfterDeleteFromDisk(const std::vector<SongInfo>& files)
{
	//删除成功，则刷新列表
	auto isRemoved = [&](const CListCtrlEx::RowData& data)
	{
		for (const auto& item : files)
		{
			if (item.file_path == data.at(GetPathColIndex()))
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

int CAllMediaDlg::GetPathColIndex() const
{
	return COL_PATH;
}

wstring CAllMediaDlg::GetSelectedString() const
{
	return wstring(m_selected_string);
}

void CAllMediaDlg::DoDataExchange(CDataExchange* pDX)
{
	CMediaLibTabDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SONG_LIST, m_song_list_ctrl);
	DDX_Control(pDX, IDC_SEARCH_EDIT, m_search_edit);
}


void CAllMediaDlg::OnOK()
{
	//这里重写了基类CMediaLibTabDlg的OnOK函数
	//在“所有曲目”和“最近播放”标签中双击一项，会在“默认”播放列表中打开选中的曲目

	std::vector<wstring> files;
	GetSongsSelected(files);
	if (!files.empty())
	{
		if (files.size() == 1)
		{
			CPlayer::GetInstance().OpenFiles(files);
		}
		else
			CPlayer::GetInstance().OpenFilesInTempPlaylist(files);

		CTabDlg::OnOK();
		CWnd* pParent = GetParentWindow();
		if (pParent != nullptr)
			::SendMessage(pParent->GetSafeHwnd(), WM_COMMAND, IDOK, 0);
	}
}

BEGIN_MESSAGE_MAP(CAllMediaDlg, CMediaLibTabDlg)
	ON_NOTIFY(HDN_ITEMCLICK, 0, &CAllMediaDlg::OnHdnItemclickSongList)
	ON_EN_CHANGE(IDC_SEARCH_EDIT, &CAllMediaDlg::OnEnChangeSearchEdit)
	ON_NOTIFY(NM_CLICK, IDC_SONG_LIST, &CAllMediaDlg::OnNMClickSongList)
	ON_NOTIFY(NM_RCLICK, IDC_SONG_LIST, &CAllMediaDlg::OnNMRClickSongList)
	ON_NOTIFY(NM_DBLCLK, IDC_SONG_LIST, &CAllMediaDlg::OnNMDblclkSongList)
	ON_MESSAGE(WM_SEARCH_EDIT_BTN_CLICKED, &CAllMediaDlg::OnSearchEditBtnClicked)
	ON_WM_INITMENU()
END_MESSAGE_MAP()


// CAllMediaDlg 消息处理程序


BOOL CAllMediaDlg::OnInitDialog()
{
	CMediaLibTabDlg::OnInitDialog();

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
	m_song_list_ctrl.InsertColumn(8, CCommon::LoadText(IDS_LAST_PLAYED_TIME), LVCFMT_LEFT, theApp.DPI(140));
	m_song_list_ctrl.SetCtrlAEnable(true);

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
				wstring str_a, str_b;
				if (a.find(phdr->iItem) != a.end())
					str_a = a.at(phdr->iItem);
				if (b.find(phdr->iItem) != b.end())
					str_b = b.at(phdr->iItem);

				if (phdr->iItem == COL_TRACK)		//如果是对“音轨号”排序，则需要将字符串转换成数字
				{
					int index_a, index_b;
					index_a = _ttoi(str_a.c_str());
					index_b = _ttoi(str_b.c_str());
					if (ascending)
						return index_a < index_b;
					else
						return index_a > index_b;
				}
				else
				{
					if (ascending)
						return str_a < str_b;
					else
						return str_a > str_b;
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
	// 发送此通知，除非重写 CMediaLibTabDlg::OnInitDialog()
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

