// CPlaySettingsDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "PlaySettingsDlg.h"
#include "afxdialogex.h"


// CPlaySettingsDlg 对话框

IMPLEMENT_DYNAMIC(CPlaySettingsDlg, CTabDlg)

CPlaySettingsDlg::CPlaySettingsDlg(CWnd* pParent /*=nullptr*/)
	: CTabDlg(IDD_PLAY_SETTING_DIALOG, pParent)
{

}

CPlaySettingsDlg::~CPlaySettingsDlg()
{
}

void CPlaySettingsDlg::DoDataExchange(CDataExchange* pDX)
{
    CTabDlg::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STOP_WHEN_ERROR, m_stop_when_error_check);
    DDX_Control(pDX, IDC_SHOW_TASKBAR_PROGRESS, m_show_taskbar_progress_check);
    DDX_Control(pDX, IDC_OUTPUT_DEVICE_COMBO, m_output_device_combo);
    DDX_Control(pDX, IDC_OUTPUT_DEVICE_INFO_LIST, m_device_info_list);
    DDX_Control(pDX, IDC_AUTO_PLAY_WHEN_START_CHECK, m_auto_play_when_start_chk);
    DDX_Control(pDX, IDC_SHOW_PLAY_STATE_ICON_CHECK, m_show_play_state_icon_chk);
    DDX_Control(pDX, IDC_SOUND_FADE_CHECK, m_sound_fade_chk);
    DDX_Control(pDX, IDC_CONTINUE_WHEN_SWITCH_PLAYLIST_CHECK, m_continue_when_switch_playlist_check);
    DDX_Control(pDX, IDC_BASS_RADIO, m_bass_radio);
    DDX_Control(pDX, IDC_MCI_RADIO, m_mci_radio);
}

void CPlaySettingsDlg::ShowDeviceInfo()
{
    if (theApp.m_output_devices.empty())
        return;

	DeviceInfo& device{ theApp.m_output_devices[m_data.device_selected] };
	m_device_info_list.SetItemText(0, 1, device.name.c_str());
	m_device_info_list.SetItemText(1, 1, device.driver.c_str());
	m_data.output_device = device.name;
	DWORD device_type;
	device_type = device.flags & BASS_DEVICE_TYPE_MASK;
	CString type_info;
	switch (device_type)
	{
	case BASS_DEVICE_TYPE_DIGITAL:
		type_info = CCommon::LoadText(IDS_DEVICE_TYPE_DIGITAL);
		break;
	case BASS_DEVICE_TYPE_DISPLAYPORT:
		type_info = CCommon::LoadText(IDS_DEVICE_TYPE_DISPLAYPORT);
		break;
	case BASS_DEVICE_TYPE_HANDSET:
		type_info = CCommon::LoadText(IDS_DEVICE_TYPE_HANDSET);
		break;
	case BASS_DEVICE_TYPE_HDMI:
		type_info = CCommon::LoadText(IDS_DEVICE_TYPE_HDMI);
		break;
	case BASS_DEVICE_TYPE_HEADPHONES:
		type_info = CCommon::LoadText(IDS_DEVICE_TYPE_HEADPHONES);
		break;
	case BASS_DEVICE_TYPE_HEADSET:
		type_info = CCommon::LoadText(IDS_DEVICE_TYPE_HEADSET);
		break;
	case BASS_DEVICE_TYPE_LINE:
		type_info = CCommon::LoadText(IDS_DEVICE_TYPE_LINE);
		break;
	case BASS_DEVICE_TYPE_MICROPHONE:
		type_info = CCommon::LoadText(IDS_DEVICE_TYPE_MICROPHONE);
		break;
	case BASS_DEVICE_TYPE_NETWORK:
		type_info = CCommon::LoadText(IDS_DEVICE_TYPE_NETWORK);
		break;
	case BASS_DEVICE_TYPE_SPDIF:
		type_info = CCommon::LoadText(IDS_DEVICE_TYPE_SPDIF);
		break;
	case BASS_DEVICE_TYPE_SPEAKERS:
		type_info = CCommon::LoadText(IDS_DEVICE_TYPE_SPEAKERS);
		break;
	}
	m_device_info_list.SetItemText(2, 1, type_info);

}

void CPlaySettingsDlg::EnableControl()
{
    bool enable = !CPlayer::GetInstance().IsMciCore();
    m_sound_fade_chk.EnableWindow(enable);
    m_device_info_list.EnableWindow(enable);
    m_output_device_combo.EnableWindow(enable);
}


BEGIN_MESSAGE_MAP(CPlaySettingsDlg, CTabDlg)
	ON_BN_CLICKED(IDC_STOP_WHEN_ERROR, &CPlaySettingsDlg::OnBnClickedStopWhenError)
	ON_BN_CLICKED(IDC_SHOW_TASKBAR_PROGRESS, &CPlaySettingsDlg::OnBnClickedShowTaskbarProgress)
	ON_CBN_SELCHANGE(IDC_OUTPUT_DEVICE_COMBO, &CPlaySettingsDlg::OnCbnSelchangeOutputDeviceCombo)
	ON_BN_CLICKED(IDC_AUTO_PLAY_WHEN_START_CHECK, &CPlaySettingsDlg::OnBnClickedAutoPlayWhenStartCheck)
	ON_BN_CLICKED(IDC_SHOW_PLAY_STATE_ICON_CHECK, &CPlaySettingsDlg::OnBnClickedShowPlayStateIconCheck)
    ON_BN_CLICKED(IDC_SOUND_FADE_CHECK, &CPlaySettingsDlg::OnBnClickedSoundFadeCheck)
    ON_BN_CLICKED(IDC_CONTINUE_WHEN_SWITCH_PLAYLIST_CHECK, &CPlaySettingsDlg::OnBnClickedContinueWhenSwitchPlaylistCheck)
    ON_BN_CLICKED(IDC_BASS_RADIO, &CPlaySettingsDlg::OnBnClickedBassRadio)
    ON_BN_CLICKED(IDC_MCI_RADIO, &CPlaySettingsDlg::OnBnClickedMciRadio)
END_MESSAGE_MAP()


// CPlaySettingsDlg 消息处理程序


BOOL CPlaySettingsDlg::OnInitDialog()
{
	CTabDlg::OnInitDialog();

	// TODO:  在此添加额外的初始化

    m_toolTip.Create(this);
    m_toolTip.SetMaxTipWidth(theApp.DPI(300));
    m_toolTip.AddTool(GetDlgItem(IDC_MCI_RADIO), CCommon::LoadText(IDS_MCI_KERNAL_TIP));
    m_toolTip.SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

	//初始化各控件的状态
	m_stop_when_error_check.SetCheck(m_data.stop_when_error);
	m_auto_play_when_start_chk.SetCheck(m_data.auto_play_when_start);
    m_continue_when_switch_playlist_check.SetCheck(m_data.continue_when_switch_playlist);
	m_show_taskbar_progress_check.SetCheck(m_data.show_taskbar_progress);
	m_show_play_state_icon_chk.SetCheck(m_data.show_playstate_icon);
    m_sound_fade_chk.SetCheck(m_data.fade_effect);
	
	if (!CWinVersionHelper::IsWindows7OrLater() || theApp.GetITaskbarList3() == nullptr)
	{
		m_show_taskbar_progress_check.SetCheck(FALSE);
		m_show_play_state_icon_chk.SetCheck(FALSE);
		m_show_taskbar_progress_check.EnableWindow(FALSE);
		m_show_play_state_icon_chk.EnableWindow(FALSE);
	}

    if (m_data.use_mci)
        m_mci_radio.SetCheck(TRUE);
    else
        m_bass_radio.SetCheck(TRUE);

	for (const auto& device : theApp.m_output_devices)
	{
		m_output_device_combo.AddString(device.name.c_str());
	}
	m_output_device_combo.SetCurSel(m_data.device_selected);

	//m_device_info_list.SetColor(theApp.m_app_setting_data.theme_color);
	CRect rect;
	m_device_info_list.GetClientRect(rect);
	int width0, width1;
	width0 = theApp.DPI(70);
	width1 = rect.Width() - width0 - theApp.DPI(20);

    m_device_info_list.SetExtendedStyle(m_device_info_list.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_LABELTIP);
	m_device_info_list.InsertColumn(0, CCommon::LoadText(IDS_ITEM), LVCFMT_LEFT, width0);		//插入第1列
	m_device_info_list.InsertColumn(1, CCommon::LoadText(IDS_VLAUE), LVCFMT_LEFT, width1);		//插入第2列
	m_device_info_list.InsertItem(0, CCommon::LoadText(IDS_NAME));
	m_device_info_list.InsertItem(1, CCommon::LoadText(IDS_DRIVER));
	m_device_info_list.InsertItem(2, CCommon::LoadText(IDS_DEVICE_TYPE));
	ShowDeviceInfo();

    EnableControl();

    //设置控件不响应鼠标滚轮消息
    m_output_device_combo.SetMouseWheelEnable(false);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CPlaySettingsDlg::OnBnClickedStopWhenError()
{
	// TODO: 在此添加控件通知处理程序代码
	m_data.stop_when_error = (m_stop_when_error_check.GetCheck() != 0);
}


void CPlaySettingsDlg::OnBnClickedShowTaskbarProgress()
{
	// TODO: 在此添加控件通知处理程序代码
	m_data.show_taskbar_progress = (m_show_taskbar_progress_check.GetCheck() != 0);
}


void CPlaySettingsDlg::OnCbnSelchangeOutputDeviceCombo()
{
	// TODO: 在此添加控件通知处理程序代码
	m_data.device_selected = m_output_device_combo.GetCurSel();
	ShowDeviceInfo();
}


void CPlaySettingsDlg::OnBnClickedAutoPlayWhenStartCheck()
{
	// TODO: 在此添加控件通知处理程序代码
	m_data.auto_play_when_start = (m_auto_play_when_start_chk.GetCheck() != 0);
}


void CPlaySettingsDlg::OnBnClickedShowPlayStateIconCheck()
{
	// TODO: 在此添加控件通知处理程序代码
	m_data.show_playstate_icon = (m_show_play_state_icon_chk.GetCheck() != 0);
}


void CPlaySettingsDlg::OnBnClickedSoundFadeCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.fade_effect = (m_sound_fade_chk.GetCheck() != 0);
}


void CPlaySettingsDlg::OnBnClickedContinueWhenSwitchPlaylistCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.continue_when_switch_playlist = (m_continue_when_switch_playlist_check.GetCheck() != 0);
}


void CPlaySettingsDlg::OnBnClickedBassRadio()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.use_mci = false;
}


void CPlaySettingsDlg::OnBnClickedMciRadio()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.use_mci = true;
}


BOOL CPlaySettingsDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if (pMsg->message == WM_MOUSEMOVE)
        m_toolTip.RelayEvent(pMsg);

    return CTabDlg::PreTranslateMessage(pMsg);
}
