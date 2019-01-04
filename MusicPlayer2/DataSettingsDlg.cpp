// DataSettingsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "DataSettingsDlg.h"
#include "afxdialogex.h"


// CDataSettingsDlg 对话框

IMPLEMENT_DYNAMIC(CDataSettingsDlg, CTabDlg)

CDataSettingsDlg::CDataSettingsDlg(CWnd* pParent /*=NULL*/)
	: CTabDlg(IDD_DATA_SETTINGS_DIALOG, pParent)
{

}

CDataSettingsDlg::~CDataSettingsDlg()
{
}

void CDataSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CTabDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SF2_PATH_EDIT, m_sf2_path_edit);
	DDX_Control(pDX, IDC_COMBO1, m_language_combo);
}


BEGIN_MESSAGE_MAP(CDataSettingsDlg, CTabDlg)
	ON_BN_CLICKED(IDC_CLEAN_DATA_FILE_BUTTON, &CDataSettingsDlg::OnBnClickedCleanDataFileButton)
	ON_BN_CLICKED(IDC_ID3V2_FIRST_CHECK, &CDataSettingsDlg::OnBnClickedId3v2FirstCheck)
	ON_BN_CLICKED(IDC_COVER_AUTO_DOWNLOAD_CHECK, &CDataSettingsDlg::OnBnClickedCoverAutoDownloadCheck)
	ON_BN_CLICKED(IDC_LYRIC_AUTO_DOWNLOAD_CHECK, &CDataSettingsDlg::OnBnClickedLyricAutoDownloadCheck)
	ON_BN_CLICKED(IDC_CHECK_UPDATE_CHECK, &CDataSettingsDlg::OnBnClickedCheckUpdateCheck)
	ON_BN_CLICKED(IDC_BROWSE_BUTTON, &CDataSettingsDlg::OnBnClickedBrowseButton)
	ON_BN_CLICKED(IDC_MIDI_USE_INNER_LYRIC_CHECK, &CDataSettingsDlg::OnBnClickedMidiUseInnerLyricCheck)
	ON_BN_CLICKED(IDC_DOWNLOAD_WHEN_TAG_FULL_CHECK, &CDataSettingsDlg::OnBnClickedDownloadWhenTagFullCheck)
	ON_EN_CHANGE(IDC_SF2_PATH_EDIT, &CDataSettingsDlg::OnEnChangeSf2PathEdit)
END_MESSAGE_MAP()


// CDataSettingsDlg 消息处理程序


BOOL CDataSettingsDlg::OnInitDialog()
{
	CTabDlg::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//SetBackgroundColor(RGB(255, 255, 255));

	m_language_combo.AddString(CCommon::LoadText(IDS_FOLLOWING_SYSTEM));
	m_language_combo.AddString(_T("English"));
	m_language_combo.AddString(_T("简体中文"));
	m_language_combo.SetCurSel(static_cast<int>(m_data.language));

	m_data_size = CCommon::GetFileSize(theApp.m_song_data_path);
	ShowDataSizeInfo();

	((CButton*)GetDlgItem(IDC_ID3V2_FIRST_CHECK))->SetCheck(m_data.id3v2_first);
	((CButton*)GetDlgItem(IDC_COVER_AUTO_DOWNLOAD_CHECK))->SetCheck(m_data.auto_download_album_cover);
	((CButton*)GetDlgItem(IDC_LYRIC_AUTO_DOWNLOAD_CHECK))->SetCheck(m_data.auto_download_lyric);
	((CButton*)GetDlgItem(IDC_DOWNLOAD_WHEN_TAG_FULL_CHECK))->SetCheck(m_data.auto_download_only_tag_full);
	((CButton*)GetDlgItem(IDC_CHECK_UPDATE_CHECK))->SetCheck(m_data.check_update_when_start);
	SetDlgItemText(IDC_SF2_PATH_EDIT, m_data.sf2_path.c_str());
	((CButton*)GetDlgItem(IDC_MIDI_USE_INNER_LYRIC_CHECK))->SetCheck(m_data.midi_use_inner_lyric);
	if (m_data.minimize_to_notify_icon)
		((CButton*)GetDlgItem(IDC_MINIMIZE_TO_NOTIFY_RADIO))->SetCheck(TRUE);
	else
		((CButton*)GetDlgItem(IDC_EXIT_PROGRAM_RADIO))->SetCheck(TRUE);

	m_toolTip.Create(this);
	m_toolTip.SetMaxTipWidth(theApp.DPI(300));
	m_toolTip.AddTool(GetDlgItem(IDC_CLEAN_DATA_FILE_BUTTON), CCommon::LoadText(IDS_CLEAR_DATA_FILE_TIP_INFO));
	m_toolTip.AddTool(GetDlgItem(IDC_DOWNLOAD_WHEN_TAG_FULL_CHECK), CCommon::LoadText(IDS_AUTO_DOWNLOAD_LYRIC_TIP_INFO));
	//m_toolTip.AddTool(GetDlgItem(IDC_SF2_PATH_EDIT), _T("需要额外的音色库才能播放 MIDI 音乐。"));
	m_toolTip.AddTool(GetDlgItem(IDC_MIDI_USE_INNER_LYRIC_CHECK), CCommon::LoadText(IDS_MIDI_INNER_LYRIC_TIP_INFO));

	GetDlgItem(IDC_BROWSE_BUTTON)->EnableWindow(theApp.m_format_convert_dialog_exit);		//正在进行格式转换时不允许更改音色库
	m_sf2_path_edit.EnableWindow(theApp.m_format_convert_dialog_exit);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CDataSettingsDlg::ShowDataSizeInfo()
{
	CString info;
	if (m_data_size < 1024)
		info.Format(_T("%s: %d %s"), CCommon::LoadText(IDS_CURRENT_DATA_FILE_SIZE), m_data_size, CCommon::LoadText(IDS_BYTE));
	else if (m_data_size < 1024 * 1024)
		info.Format(_T("%s: %.2f KB (%d %s)"), CCommon::LoadText(IDS_CURRENT_DATA_FILE_SIZE), static_cast<float>(m_data_size) / 1024.0f, m_data_size, CCommon::LoadText(IDS_BYTE));
	else
		info.Format(_T("%s: %.2f MB (%d %s)"), CCommon::LoadText(IDS_CURRENT_DATA_FILE_SIZE), static_cast<float>(m_data_size) / 1024.0f / 1024.0f, m_data_size, CCommon::LoadText(IDS_BYTE));		//注：此处曾经由于“%.2fMB”漏掉了“f”导致出现了一打开这个对话框程序就停止工作的严重问题。
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
	info = CCommon::LoadTextFormat(IDS_CLEAR_COMPLETE_INFO, { clear_cnt, size_reduced });
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
	CString szFilter = CCommon::LoadText(IDS_SOUND_FONT_FILTER);
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

	return CTabDlg::PreTranslateMessage(pMsg);
}


void CDataSettingsDlg::OnBnClickedMidiUseInnerLyricCheck()
{
	// TODO: 在此添加控件通知处理程序代码
	m_data.midi_use_inner_lyric = (((CButton*)GetDlgItem(IDC_MIDI_USE_INNER_LYRIC_CHECK))->GetCheck() != 0);
}


void CDataSettingsDlg::OnBnClickedDownloadWhenTagFullCheck()
{
	// TODO: 在此添加控件通知处理程序代码
	m_data.auto_download_only_tag_full = (((CButton*)GetDlgItem(IDC_DOWNLOAD_WHEN_TAG_FULL_CHECK))->GetCheck() != 0);
}


void CDataSettingsDlg::OnEnChangeSf2PathEdit()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CTabDlg::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。
	if (m_sf2_path_edit.GetModify())
	{
		CString str;
		m_sf2_path_edit.GetWindowText(str);
		m_data.sf2_path = str;
	}

	// TODO:  在此添加控件通知处理程序代码
}


void CDataSettingsDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	m_data.minimize_to_notify_icon = (((CButton*)GetDlgItem(IDC_MINIMIZE_TO_NOTIFY_RADIO))->GetCheck() != 0);

	//获取语言的设置
	m_data.language = static_cast<Language>(m_language_combo.GetCurSel());
	if (m_data.language != theApp.m_general_setting_data.language)
	{
		MessageBox(CCommon::LoadText(IDS_LANGUAGE_CHANGE_INFO), NULL, MB_ICONINFORMATION | MB_OK);
	}

	CTabDlg::OnOK();
}
