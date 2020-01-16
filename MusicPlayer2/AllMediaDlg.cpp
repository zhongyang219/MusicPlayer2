// AllMediaDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "AllMediaDlg.h"


// CAllMediaDlg 对话框

IMPLEMENT_DYNAMIC(CAllMediaDlg, CTabDlg)

CAllMediaDlg::CAllMediaDlg(CWnd* pParent /*=nullptr*/)
	: CTabDlg(IDD_ALL_MEDIA_DIALOG, pParent)
{

}

CAllMediaDlg::~CAllMediaDlg()
{
}

void CAllMediaDlg::OnTabEntered()
{
	//SetButtonsEnable();
	if (!m_initialized)
	{
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
		return a.at(COL_PATH) < b.at(COL_PATH);
	});
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
	m_song_list_ctrl.SetListData(m_list_data);
}

void CAllMediaDlg::DoDataExchange(CDataExchange* pDX)
{
	CTabDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SONG_LIST, m_song_list_ctrl);
	DDX_Control(pDX, IDC_MFCEDITBROWSE1, m_search_edit);
}


BEGIN_MESSAGE_MAP(CAllMediaDlg, CTabDlg)
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

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
