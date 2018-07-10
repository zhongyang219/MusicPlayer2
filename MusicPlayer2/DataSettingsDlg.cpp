// DataSettingsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "DataSettingsDlg.h"
#include "afxdialogex.h"


// CDataSettingsDlg 对话框

IMPLEMENT_DYNAMIC(CDataSettingsDlg, CDialogEx)

CDataSettingsDlg::CDataSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DATA_SETTINGS_DIALOG, pParent)
{

}

CDataSettingsDlg::~CDataSettingsDlg()
{
}

void CDataSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDataSettingsDlg, CDialogEx)
	ON_BN_CLICKED(IDC_CLEAN_DATA_FILE_BUTTON, &CDataSettingsDlg::OnBnClickedCleanDataFileButton)
	ON_BN_CLICKED(IDC_ID3V2_FIRST_CHECK, &CDataSettingsDlg::OnBnClickedId3v2FirstCheck)
	ON_BN_CLICKED(IDC_COVER_AUTO_DOWNLOAD_CHECK, &CDataSettingsDlg::OnBnClickedCoverAutoDownloadCheck)
	ON_BN_CLICKED(IDC_LYRIC_AUTO_DOWNLOAD_CHECK, &CDataSettingsDlg::OnBnClickedLyricAutoDownloadCheck)
	ON_BN_CLICKED(IDC_CHECK_UPDATE_CHECK, &CDataSettingsDlg::OnBnClickedCheckUpdateCheck)
	ON_BN_CLICKED(IDC_BROWSE_BUTTON, &CDataSettingsDlg::OnBnClickedBrowseButton)
	ON_BN_CLICKED(IDC_MIDI_USE_INNER_LYRIC_CHECK, &CDataSettingsDlg::OnBnClickedMidiUseInnerLyricCheck)
END_MESSAGE_MAP()


// CDataSettingsDlg 消息处理程序


BOOL CDataSettingsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetBackgroundColor(RGB(255, 255, 255));

	m_data_size = CCommon::GetFileSize(theApp.m_song_data_path);
	ShowDataSizeInfo();

	((CButton*)GetDlgItem(IDC_ID3V2_FIRST_CHECK))->SetCheck(m_data.id3v2_first);
	((CButton*)GetDlgItem(IDC_COVER_AUTO_DOWNLOAD_CHECK))->SetCheck(m_data.auto_download_album_cover);
	((CButton*)GetDlgItem(IDC_LYRIC_AUTO_DOWNLOAD_CHECK))->SetCheck(m_data.auto_download_lyric);
	((CButton*)GetDlgItem(IDC_CHECK_UPDATE_CHECK))->SetCheck(m_data.check_update_when_start);
	SetDlgItemText(IDC_SF2_PATH_EDIT, m_data.sf2_path.c_str());
	((CButton*)GetDlgItem(IDC_MIDI_USE_INNER_LYRIC_CHECK))->SetCheck(m_data.midi_use_inner_lyric);

	m_toolTip.Create(this);
	m_toolTip.SetMaxTipWidth(DPI(300));
	m_toolTip.AddTool(GetDlgItem(IDC_CLEAN_DATA_FILE_BUTTON), _T("说明：程序目录下的 song_data.dat 文件保存了所有加载过的歌曲信息，用于加快播放列表的载入速度，如果该文件过大，可以通过此按钮清理它。"));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CDataSettingsDlg::ShowDataSizeInfo()
{
	CString info;
	if (m_data_size < 1024)
		info.Format(_T("当前数据文件大小：%d个字节"), m_data_size);
	else if (m_data_size < 1024 * 1024)
		info.Format(_T("当前数据文件大小：%.2fKB (%d个字节)"), static_cast<float>(m_data_size) / 1024.0f, m_data_size);
	else
		info.Format(_T("当前数据文件大小：%.2fMB (%d个字节)"), static_cast<float>(m_data_size) / 1024.0f / 1024.0f, m_data_size);		//注：此处曾经由于“%.2fMB”漏掉了“f”导致出现了一打开这个对话框程序就停止工作的严重问题。
	SetDlgItemText(IDC_SIZE_STATIC, info);
}



void CDataSettingsDlg::OnBnClickedCleanDataFileButton()
{
	// TODO: 在此添加控件通知处理程序代码
	
	CWaitCursor wait_cursor;	//显示等待光标
	int clear_cnt{};		//统计删除的项目的数量
	//遍历映射容器，删除不必要的条目。
	for (auto iter{ theApp.m_song_data.begin() }; iter != theApp.m_song_data.end();)
	{
		//检查该条目对应的文件所在的路径是否在“最近播放路径”列表里
		bool path_exist{ false };	//如果iter指向的条目的文件路径在“最近播放路径”列表(theApp.m_player.GetRecentPath())里，则为true
		wstring item_path;
		size_t index = iter->first.rfind(L'\\');
		item_path = iter->first.substr(0, index + 1);		//获取iter指向项目的文件目录
		for (int i{}; i < theApp.m_player.GetRecentPath().size(); i++)
		{
			if (item_path == theApp.m_player.GetRecentPath()[i].path)
			{
				path_exist = true;
				break;
			}
		}
		//如果该条目对应的文件所在的路径不在“最近播放路径”列表里，或该条目对应的文件不存在，则删除该条目
		if (!path_exist || !CCommon::FileExist(iter->first))
		{
			iter = theApp.m_song_data.erase(iter);		//删除条目之后将迭代器指向被删除条目的前一个条目
			clear_cnt++;
		}
		else
		{
			iter++;
		}
	}
	theApp.SaveSongData();		//清理后将数据写入文件

	size_t data_size = CCommon::GetFileSize(theApp.m_song_data_path);	 //清理后数据文件的大小
	int size_reduced = m_data_size - data_size;		//清理后数据文件减少的字节数
	if (size_reduced < 0) size_reduced = 0;
	CString info;
	info.Format(_T("清理完成，已删除%d个项目，数据文件大小减小%d个字节。"), clear_cnt, size_reduced);
	MessageBox(info, NULL, MB_ICONINFORMATION);
	m_data_size = data_size;
	ShowDataSizeInfo();
}


void CDataSettingsDlg::OnBnClickedId3v2FirstCheck()
{
	// TODO: 在此添加控件通知处理程序代码
	m_data.id3v2_first = (((CButton*)GetDlgItem(IDC_ID3V2_FIRST_CHECK))->GetCheck() != 0);
}


void CDataSettingsDlg::OnBnClickedCoverAutoDownloadCheck()
{
	// TODO: 在此添加控件通知处理程序代码
	m_data.auto_download_album_cover = (((CButton*)GetDlgItem(IDC_COVER_AUTO_DOWNLOAD_CHECK))->GetCheck() != 0);
}


void CDataSettingsDlg::OnBnClickedLyricAutoDownloadCheck()
{
	// TODO: 在此添加控件通知处理程序代码
	m_data.auto_download_lyric = (((CButton*)GetDlgItem(IDC_LYRIC_AUTO_DOWNLOAD_CHECK))->GetCheck() != 0);
}


void CDataSettingsDlg::OnBnClickedCheckUpdateCheck()
{
	// TODO: 在此添加控件通知处理程序代码
	m_data.check_update_when_start = (((CButton*)GetDlgItem(IDC_CHECK_UPDATE_CHECK))->GetCheck() != 0);
}


void CDataSettingsDlg::OnBnClickedBrowseButton()
{
	// TODO: 在此添加控件通知处理程序代码
	//设置过滤器
	LPCTSTR szFilter = _T("音色库文件(*.SF2)|*.SF2|所有文件(*.*)|*.*||");
	//构造打开文件对话框
	CFileDialog fileDlg(TRUE, _T("SF2"), NULL, 0, szFilter, this);
	//显示打开文件对话框
	if (IDOK == fileDlg.DoModal())
	{
		m_data.sf2_path = fileDlg.GetPathName();	//获取打开的文件路径
		SetDlgItemText(IDC_SF2_PATH_EDIT, m_data.sf2_path.c_str());
	}
}


BOOL CDataSettingsDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_MOUSEMOVE)
		m_toolTip.RelayEvent(pMsg);

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CDataSettingsDlg::OnBnClickedMidiUseInnerLyricCheck()
{
	// TODO: 在此添加控件通知处理程序代码
	m_data.midi_use_inner_lyric = (((CButton*)GetDlgItem(IDC_MIDI_USE_INNER_LYRIC_CHECK))->GetCheck() != 0);
}
