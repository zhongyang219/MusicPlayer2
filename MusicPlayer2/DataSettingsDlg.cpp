// DataSettingsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "DataSettingsDlg.h"
#include "afxdialogex.h"
#include "UpdateHelper.h"


// CDataSettingsDlg 对话框

IMPLEMENT_DYNAMIC(CDataSettingsDlg, CTabDlg)

CDataSettingsDlg::CDataSettingsDlg(CWnd* pParent /*=NULL*/)
	: CTabDlg(IDD_DATA_SETTINGS_DIALOG, pParent)
{

}

CDataSettingsDlg::~CDataSettingsDlg()
{
}

bool CDataSettingsDlg::IsAutoRunModified() const
{
    return m_auto_run_modified;
}

void CDataSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CTabDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SF2_PATH_EDIT, m_sf2_path_edit);
	DDX_Control(pDX, IDC_COMBO1, m_language_combo);
}


BEGIN_MESSAGE_MAP(CDataSettingsDlg, CTabDlg)
	//ON_BN_CLICKED(IDC_ID3V2_FIRST_CHECK, &CDataSettingsDlg::OnBnClickedId3v2FirstCheck)
	ON_BN_CLICKED(IDC_COVER_AUTO_DOWNLOAD_CHECK, &CDataSettingsDlg::OnBnClickedCoverAutoDownloadCheck)
	ON_BN_CLICKED(IDC_LYRIC_AUTO_DOWNLOAD_CHECK, &CDataSettingsDlg::OnBnClickedLyricAutoDownloadCheck)
	ON_BN_CLICKED(IDC_CHECK_UPDATE_CHECK, &CDataSettingsDlg::OnBnClickedCheckUpdateCheck)
	//ON_BN_CLICKED(IDC_BROWSE_BUTTON, &CDataSettingsDlg::OnBnClickedBrowseButton)
	ON_BN_CLICKED(IDC_MIDI_USE_INNER_LYRIC_CHECK, &CDataSettingsDlg::OnBnClickedMidiUseInnerLyricCheck)
	ON_BN_CLICKED(IDC_DOWNLOAD_WHEN_TAG_FULL_CHECK, &CDataSettingsDlg::OnBnClickedDownloadWhenTagFullCheck)
	ON_EN_CHANGE(IDC_SF2_PATH_EDIT, &CDataSettingsDlg::OnEnChangeSf2PathEdit)
    ON_MESSAGE(WM_EDIT_BROWSE_CHANGED, &CDataSettingsDlg::OnEditBrowseChanged)
    ON_BN_CLICKED(IDC_AUTO_RUN_CHECK, &CDataSettingsDlg::OnBnClickedAutoRunCheck)
    ON_BN_CLICKED(IDC_GITHUB_RADIO, &CDataSettingsDlg::OnBnClickedGithubRadio)
    ON_BN_CLICKED(IDC_GITEE_RADIO, &CDataSettingsDlg::OnBnClickedGiteeRadio)
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

    m_auto_run = theApp.GetAutoRun();
    CheckDlgButton(IDC_AUTO_RUN_CHECK, m_auto_run);
    if (m_data.update_source == static_cast<int>(CUpdateHelper::UpdateSource::GitHubSource))
        CheckDlgButton(IDC_GITHUB_RADIO, TRUE);
    else
        CheckDlgButton(IDC_GITEE_RADIO, TRUE);

	//((CButton*)GetDlgItem(IDC_ID3V2_FIRST_CHECK))->SetCheck(m_data.id3v2_first);
	((CButton*)GetDlgItem(IDC_COVER_AUTO_DOWNLOAD_CHECK))->SetCheck(m_data.auto_download_album_cover);
	((CButton*)GetDlgItem(IDC_LYRIC_AUTO_DOWNLOAD_CHECK))->SetCheck(m_data.auto_download_lyric);
	((CButton*)GetDlgItem(IDC_DOWNLOAD_WHEN_TAG_FULL_CHECK))->SetCheck(m_data.auto_download_only_tag_full);
	((CButton*)GetDlgItem(IDC_CHECK_UPDATE_CHECK))->SetCheck(m_data.check_update_when_start);
    m_sf2_path_edit.SetWindowText(m_data.sf2_path.c_str());
    CString szFilter = CCommon::LoadText(IDS_SOUND_FONT_FILTER);
    m_sf2_path_edit.EnableFileBrowseButton(_T("SF2"), szFilter);
	((CButton*)GetDlgItem(IDC_MIDI_USE_INNER_LYRIC_CHECK))->SetCheck(m_data.midi_use_inner_lyric);
	if (m_data.minimize_to_notify_icon)
		((CButton*)GetDlgItem(IDC_MINIMIZE_TO_NOTIFY_RADIO))->SetCheck(TRUE);
	else
		((CButton*)GetDlgItem(IDC_EXIT_PROGRAM_RADIO))->SetCheck(TRUE);

    if(m_data.save_lyric_to_song_folder)
        ((CButton*)GetDlgItem(IDC_SAVE_TO_SONG_FOLDER))->SetCheck(TRUE);
    else
        ((CButton*)GetDlgItem(IDC_SAVE_TO_LYRIC_FOLDER))->SetCheck(TRUE);

    //判断歌词文件夹是否存在
    bool lyric_path_exist = CCommon::FolderExist(theApp.m_lyric_setting_data.lyric_path);
    if (!lyric_path_exist)		//如果歌词文件不存在，则禁用“保存到歌词文件夹”单选按钮，并强制选中“保存到歌曲所在目录”
    {
        ((CButton*)GetDlgItem(IDC_SAVE_TO_LYRIC_FOLDER))->EnableWindow(FALSE);
        ((CButton*)GetDlgItem(IDC_SAVE_TO_LYRIC_FOLDER))->SetCheck(FALSE);
        ((CButton*)GetDlgItem(IDC_SAVE_TO_SONG_FOLDER))->SetCheck(TRUE);
        m_data.save_lyric_to_song_folder = true;
    }

	m_toolTip.Create(this);
	m_toolTip.SetMaxTipWidth(theApp.DPI(300));
	m_toolTip.AddTool(GetDlgItem(IDC_DOWNLOAD_WHEN_TAG_FULL_CHECK), CCommon::LoadText(IDS_AUTO_DOWNLOAD_LYRIC_TIP_INFO));
	//m_toolTip.AddTool(GetDlgItem(IDC_SF2_PATH_EDIT), _T("需要额外的音色库才能播放 MIDI 音乐。"));
	m_toolTip.AddTool(GetDlgItem(IDC_MIDI_USE_INNER_LYRIC_CHECK), CCommon::LoadText(IDS_MIDI_INNER_LYRIC_TIP_INFO));

	m_toolTip.SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

    EnableControl();

    //设置控件不响应鼠标滚轮消息
    m_language_combo.SetMouseWheelEnable(false);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CDataSettingsDlg::EnableControl()
{
    bool enable = !CPlayer::GetInstance().IsMciCore();
    m_sf2_path_edit.EnableWindow(enable && theApp.m_format_convert_dialog_exit);		//正在进行格式转换时不允许更改音色库
    CWnd* pWnd = GetDlgItem(IDC_BROWSE_BUTTON);
    if(pWnd!=nullptr)
        pWnd->EnableWindow(enable && theApp.m_format_convert_dialog_exit);
    pWnd = GetDlgItem(IDC_MIDI_USE_INNER_LYRIC_CHECK);
    if (pWnd != nullptr)
        pWnd->EnableWindow(enable);

    ((CButton*)GetDlgItem(IDC_SAVE_TO_SONG_FOLDER))->EnableWindow(m_data.auto_download_lyric);
    ((CButton*)GetDlgItem(IDC_SAVE_TO_LYRIC_FOLDER))->EnableWindow(m_data.auto_download_lyric && CCommon::FolderExist(theApp.m_lyric_setting_data.lyric_path));

}


//void CDataSettingsDlg::OnBnClickedId3v2FirstCheck()
//{
//	// TODO: 在此添加控件通知处理程序代码
//	m_data.id3v2_first = (((CButton*)GetDlgItem(IDC_ID3V2_FIRST_CHECK))->GetCheck() != 0);
//}


void CDataSettingsDlg::OnBnClickedCoverAutoDownloadCheck()
{
	// TODO: 在此添加控件通知处理程序代码
	m_data.auto_download_album_cover = (((CButton*)GetDlgItem(IDC_COVER_AUTO_DOWNLOAD_CHECK))->GetCheck() != 0);
}


void CDataSettingsDlg::OnBnClickedLyricAutoDownloadCheck()
{
	// TODO: 在此添加控件通知处理程序代码
	m_data.auto_download_lyric = (((CButton*)GetDlgItem(IDC_LYRIC_AUTO_DOWNLOAD_CHECK))->GetCheck() != 0);
    EnableControl();
}


void CDataSettingsDlg::OnBnClickedCheckUpdateCheck()
{
	// TODO: 在此添加控件通知处理程序代码
	m_data.check_update_when_start = (((CButton*)GetDlgItem(IDC_CHECK_UPDATE_CHECK))->GetCheck() != 0);
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
    m_data.save_lyric_to_song_folder = (((CButton*)GetDlgItem(IDC_SAVE_TO_SONG_FOLDER))->GetCheck() != 0);

	//获取语言的设置
	m_data.language = static_cast<Language>(m_language_combo.GetCurSel());
	if (m_data.language != theApp.m_general_setting_data.language)
	{
		MessageBox(CCommon::LoadText(IDS_LANGUAGE_CHANGE_INFO), NULL, MB_ICONINFORMATION | MB_OK);
	}

	CTabDlg::OnOK();
}


afx_msg LRESULT CDataSettingsDlg::OnEditBrowseChanged(WPARAM wParam, LPARAM lParam)
{
    CString str;
    m_sf2_path_edit.GetWindowText(str);
    m_data.sf2_path = str;
    return 0;
}


void CDataSettingsDlg::OnBnClickedAutoRunCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_auto_run = (IsDlgButtonChecked(IDC_AUTO_RUN_CHECK) != 0);
    m_auto_run_modified = true;
}


void CDataSettingsDlg::OnBnClickedGithubRadio()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.update_source = static_cast<int>(CUpdateHelper::UpdateSource::GitHubSource);
}


void CDataSettingsDlg::OnBnClickedGiteeRadio()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.update_source = static_cast<int>(CUpdateHelper::UpdateSource::GiteeSource);
}
