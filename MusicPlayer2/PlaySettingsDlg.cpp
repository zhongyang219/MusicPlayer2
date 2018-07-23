// PlaySettingsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "PlaySettingsDlg.h"
#include "afxdialogex.h"


// CPlaySettingsDlg 对话框

IMPLEMENT_DYNAMIC(CPlaySettingsDlg, CDialogEx)

CPlaySettingsDlg::CPlaySettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PLAY_SETTING_DIALOG, pParent)
{

}

CPlaySettingsDlg::~CPlaySettingsDlg()
{
}

void CPlaySettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STOP_WHEN_ERROR, m_stop_when_error_check);
	DDX_Control(pDX, IDC_KARAOKE_DISP, m_karaoke_disp_check);
	DDX_Control(pDX, IDC_SHOW_TASKBAR_PROGRESS, m_show_taskbar_progress_check);
	DDX_Control(pDX, IDC_LYRIC_FUZZY_MATCH, m_lyric_fuzzy_match_check);
	DDX_Control(pDX, IDC_SHOW_LYRIC_IN_CORTANA, m_show_lyric_in_cortana_check);
	DDX_Control(pDX, IDC_LYRIC_DOUBLE_LINE_CHECK, m_lyric_double_line_chk);
	DDX_Control(pDX, IDC_SHOW_ALBUM_COVER_IN_CORTANA, m_show_album_cover_in_cortana_check);
	DDX_Control(pDX, IDC_CORTANA_ICON_DEAT_CHECK, m_cortana_icon_beat_check);
	DDX_Control(pDX, IDC_CORTANA_COLOR_COMBO, m_cortana_color_combo);
}


BEGIN_MESSAGE_MAP(CPlaySettingsDlg, CDialogEx)
	ON_BN_CLICKED(IDC_STOP_WHEN_ERROR, &CPlaySettingsDlg::OnBnClickedStopWhenError)
	ON_BN_CLICKED(IDC_KARAOKE_DISP, &CPlaySettingsDlg::OnBnClickedKaraokeDisp)
	ON_BN_CLICKED(IDC_EXPLORE_LYRIC_BUTTON, &CPlaySettingsDlg::OnBnClickedExploreLyricButton)
	ON_BN_CLICKED(IDC_SHOW_TASKBAR_PROGRESS, &CPlaySettingsDlg::OnBnClickedShowTaskbarProgress)
	ON_BN_CLICKED(IDC_LYRIC_FUZZY_MATCH, &CPlaySettingsDlg::OnBnClickedLyricFuzzyMatch)
	ON_BN_CLICKED(IDC_SHOW_LYRIC_IN_CORTANA, &CPlaySettingsDlg::OnBnClickedShowLyricInCortana)
	//ON_BN_CLICKED(IDC_SAVE_IN_OFFSET_TAG, &CPlaySettingsDlg::OnBnClickedSaveInOffsetTag)
	//ON_BN_CLICKED(IDC_SAVE_IN_TIME_TAG, &CPlaySettingsDlg::OnBnClickedSaveInTimeTag)
	ON_BN_CLICKED(IDC_LYRIC_DOUBLE_LINE_CHECK, &CPlaySettingsDlg::OnBnClickedLyricDoubleLineCheck)
	ON_CBN_SELCHANGE(IDC_CORTANA_COLOR_COMBO, &CPlaySettingsDlg::OnCbnSelchangeCortanaColorCombo)
	ON_BN_CLICKED(IDC_SHOW_ALBUM_COVER_IN_CORTANA, &CPlaySettingsDlg::OnBnClickedShowAlbumCoverInCortana)
	ON_BN_CLICKED(IDC_CORTANA_ICON_DEAT_CHECK, &CPlaySettingsDlg::OnBnClickedCortanaIconDeatCheck)
END_MESSAGE_MAP()


// CPlaySettingsDlg 消息处理程序


BOOL CPlaySettingsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetBackgroundColor(RGB(255, 255, 255));

	//初始化各控件的状态
	m_stop_when_error_check.SetCheck(m_data.stop_when_error);
	m_karaoke_disp_check.SetCheck(m_data.lyric_karaoke_disp);
	m_show_taskbar_progress_check.SetCheck(m_data.show_taskbar_progress);
	m_lyric_fuzzy_match_check.SetCheck(m_data.lyric_fuzzy_match);
	m_lyric_double_line_chk.SetCheck(m_data.cortana_lyric_double_line);
	m_show_album_cover_in_cortana_check.SetCheck(m_data.cortana_show_album_cover);
	m_cortana_icon_beat_check.SetCheck(m_data.cortana_icon_beat);
	//m_cortana_icon_beat_check.EnableWindow(!m_data.cortana_show_album_cover);
	if (theApp.m_win_version.IsWindows10OrLater())
	{
		m_show_lyric_in_cortana_check.SetCheck(m_data.show_lyric_in_cortana);
	}
	else
	{
		m_show_lyric_in_cortana_check.EnableWindow(FALSE);		//Win10以下系统禁用此复选按钮
		m_data.show_lyric_in_cortana = false;
	}
#ifdef COMPILE_IN_WIN_XP
	m_show_taskbar_progress_check.SetCheck(FALSE);
	m_show_taskbar_progress_check.EnableWindow(FALSE);
#endif
	//if(m_data.save_lyric_in_offset)
	//	((CButton*)GetDlgItem(IDC_SAVE_IN_OFFSET_TAG))->SetCheck(TRUE);
	//else
	//	((CButton*)GetDlgItem(IDC_SAVE_IN_TIME_TAG))->SetCheck(TRUE);

	SetCortanaControlEnable(m_data.show_lyric_in_cortana);

	SetDlgItemText(IDC_LYRIC_PATH_EDIT, m_data.lyric_path.c_str());

	m_tool_tip.Create(this);
	m_tool_tip.SetMaxTipWidth(300);
	m_tool_tip.AddTool(&m_lyric_fuzzy_match_check, _T("如果去掉此复选框的勾选，则只会匹配和歌曲文件名完全一样的歌词文件；\r\n如果选中此复选框，当找不到文件名完全一样的歌词文件时，会匹配文件名中包含艺术家和歌曲标题的歌词文件。\r\n（可能需要重新启动程序才能生效。）"));
	//m_tool_tip.AddTool(GetDlgItem(IDC_SAVE_IN_OFFSET_TAG), _T("将歌词偏移保存到offset标签中，选择此项会使得修改的时间偏移很容易恢复，但是并非所有的播放器都支持offset标签。"));
	//m_tool_tip.AddTool(GetDlgItem(IDC_SAVE_IN_TIME_TAG), _T("将歌词偏移保存到每个时间标签中，选择此项会使得修改的时间偏移不那么容易恢复，但是对其他播放器的兼容性很好。"));
	m_tool_tip.AddTool(GetDlgItem(IDC_LYRIC_PATH_EDIT), _T("说明：如果歌曲所在目录下找不到匹配的歌词文件，就会在此文件夹下寻找歌词文件。"));
	m_tool_tip.AddTool(GetDlgItem(IDC_SHOW_LYRIC_IN_CORTANA), _T("勾选项后，可以在 Cortana 搜索框中显示歌词、歌曲名称和专辑封面。开启此功能后可能需要重新启动软件才能生效。"));

	m_cortana_color_combo.AddString(_T("跟随系统"));
	m_cortana_color_combo.AddString(_T("黑色"));
	m_cortana_color_combo.AddString(_T("白色"));
	m_cortana_color_combo.SetCurSel(m_data.cortana_color);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CPlaySettingsDlg::SetCortanaControlEnable(bool enable)
{
	m_lyric_double_line_chk.EnableWindow(enable);
	m_show_album_cover_in_cortana_check.EnableWindow(enable);
	m_cortana_color_combo.EnableWindow(enable);
	//if(m_data.cortana_show_album_cover)
	//	m_cortana_icon_beat_check.EnableWindow(FALSE);
	//else
	m_cortana_icon_beat_check.EnableWindow(enable);
}


void CPlaySettingsDlg::OnBnClickedStopWhenError()
{
	// TODO: 在此添加控件通知处理程序代码
	m_data.stop_when_error = (m_stop_when_error_check.GetCheck() != 0);
}


void CPlaySettingsDlg::OnBnClickedKaraokeDisp()
{
	// TODO: 在此添加控件通知处理程序代码
	m_data.lyric_karaoke_disp = (m_karaoke_disp_check.GetCheck() != 0);
}


void CPlaySettingsDlg::OnBnClickedExploreLyricButton()
{
	// TODO: 在此添加控件通知处理程序代码
#ifdef COMPILE_IN_WIN_XP
	CFolderBrowserDlg folderPickerDlg(this->GetSafeHwnd());
	folderPickerDlg.SetInfo(_T("请选择歌词文件夹。"));
#else
	CFolderPickerDialog folderPickerDlg(m_data.lyric_path.c_str());
	folderPickerDlg.m_ofn.lpstrTitle = _T("选择歌词文件夹");		//设置对话框标题
#endif // COMPILE_IN_WIN_XP
	if (folderPickerDlg.DoModal() == IDOK)
	{
		m_data.lyric_path = folderPickerDlg.GetPathName();
		if (m_data.lyric_path.back() != L'\\') m_data.lyric_path.push_back(L'\\');	//确保路径末尾有反斜杠
		SetDlgItemText(IDC_LYRIC_PATH_EDIT, m_data.lyric_path.c_str());
	}
}


void CPlaySettingsDlg::OnBnClickedShowTaskbarProgress()
{
	// TODO: 在此添加控件通知处理程序代码
	m_data.show_taskbar_progress = (m_show_taskbar_progress_check.GetCheck() != 0);
}


void CPlaySettingsDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialogEx::OnCancel();
}


void CPlaySettingsDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialogEx::OnOK();
}


void CPlaySettingsDlg::OnBnClickedLyricFuzzyMatch()
{
	// TODO: 在此添加控件通知处理程序代码
	m_data.lyric_fuzzy_match = (m_lyric_fuzzy_match_check.GetCheck() != 0);
}


BOOL CPlaySettingsDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_MOUSEMOVE)
		m_tool_tip.RelayEvent(pMsg);

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CPlaySettingsDlg::OnBnClickedShowLyricInCortana()
{
	// TODO: 在此添加控件通知处理程序代码
	m_data.show_lyric_in_cortana = (m_show_lyric_in_cortana_check.GetCheck() != 0);
	SetCortanaControlEnable(m_data.show_lyric_in_cortana);
}


//void CPlaySettingsDlg::OnBnClickedSaveInOffsetTag()
//{
//	// TODO: 在此添加控件通知处理程序代码
//	m_data.save_lyric_in_offset = true;
//}


//void CPlaySettingsDlg::OnBnClickedSaveInTimeTag()
//{
//	// TODO: 在此添加控件通知处理程序代码
//	m_data.save_lyric_in_offset = false;
//}


void CPlaySettingsDlg::OnBnClickedLyricDoubleLineCheck()
{
	// TODO: 在此添加控件通知处理程序代码
	m_data.cortana_lyric_double_line = (m_lyric_double_line_chk.GetCheck() != 0);
}


void CPlaySettingsDlg::OnCbnSelchangeCortanaColorCombo()
{
	// TODO: 在此添加控件通知处理程序代码
	m_data.cortana_color = m_cortana_color_combo.GetCurSel();
}


void CPlaySettingsDlg::OnBnClickedShowAlbumCoverInCortana()
{
	// TODO: 在此添加控件通知处理程序代码
	m_data.cortana_show_album_cover = (m_show_album_cover_in_cortana_check.GetCheck() != 0);
	//m_cortana_icon_beat_check.EnableWindow(!m_data.cortana_show_album_cover);
}


void CPlaySettingsDlg::OnBnClickedCortanaIconDeatCheck()
{
	// TODO: 在此添加控件通知处理程序代码
	m_data.cortana_icon_beat = (m_cortana_icon_beat_check.GetCheck() != 0);
}
