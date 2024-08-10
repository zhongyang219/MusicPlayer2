// CPlaySettingsDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "Player.h"
#include "PlaySettingsDlg.h"
#include "FfmpegCore.h"
#include "WinVersionHelper.h"
#include "FilterHelper.h"


// CPlaySettingsDlg 对话框

IMPLEMENT_DYNAMIC(CPlaySettingsDlg, CTabDlg)

CPlaySettingsDlg::CPlaySettingsDlg(CWnd* pParent /*=nullptr*/)
    : CTabDlg(IDD_PLAY_SETTING_DIALOG, pParent)
{

}

CPlaySettingsDlg::~CPlaySettingsDlg()
{
}

void CPlaySettingsDlg::ShowDeviceInfo()
{
    if (theApp.m_output_devices.empty())
        return;

    // 重新设置设备选择下拉框
    m_output_device_combo.ResetContent();
    for (const auto& device : theApp.m_output_devices)
    {
        m_output_device_combo.AddString(device.name.c_str());
    }
    m_output_device_combo.SetCurSel(m_data.device_selected);

    // 更新当前设备信息显示
    DeviceInfo& device{ theApp.m_output_devices[m_data.device_selected] };
    m_device_info_list.SetItemText(0, 1, device.name.c_str());
    m_device_info_list.SetItemText(1, 1, device.driver.c_str());
    m_data.output_device = device.name;
    DWORD device_type;
    device_type = device.flags & BASS_DEVICE_TYPE_MASK;
    wstring type_info;
    switch (device_type)
    {
    case BASS_DEVICE_TYPE_NETWORK: type_info = theApp.m_str_table.LoadText(L"TXT_OPT_PLAY_DEVICE_TYPE_NETWORK"); break;
    case BASS_DEVICE_TYPE_SPEAKERS: type_info = theApp.m_str_table.LoadText(L"TXT_OPT_PLAY_DEVICE_TYPE_SPEAKERS"); break;
    case BASS_DEVICE_TYPE_LINE: type_info = theApp.m_str_table.LoadText(L"TXT_OPT_PLAY_DEVICE_TYPE_LINE"); break;
    case BASS_DEVICE_TYPE_HEADPHONES: type_info = theApp.m_str_table.LoadText(L"TXT_OPT_PLAY_DEVICE_TYPE_HEADPHONES"); break;
    case BASS_DEVICE_TYPE_MICROPHONE: type_info = theApp.m_str_table.LoadText(L"TXT_OPT_PLAY_DEVICE_TYPE_MICROPHONE"); break;
    case BASS_DEVICE_TYPE_HEADSET: type_info = theApp.m_str_table.LoadText(L"TXT_OPT_PLAY_DEVICE_TYPE_HEADSET"); break;
    case BASS_DEVICE_TYPE_HANDSET: type_info = theApp.m_str_table.LoadText(L"TXT_OPT_PLAY_DEVICE_TYPE_HANDSET"); break;
    case BASS_DEVICE_TYPE_DIGITAL: type_info = theApp.m_str_table.LoadText(L"TXT_OPT_PLAY_DEVICE_TYPE_DIGITAL"); break;
    case BASS_DEVICE_TYPE_SPDIF: type_info = theApp.m_str_table.LoadText(L"TXT_OPT_PLAY_DEVICE_TYPE_SPDIF"); break;
    case BASS_DEVICE_TYPE_HDMI: type_info = theApp.m_str_table.LoadText(L"TXT_OPT_PLAY_DEVICE_TYPE_HDMI"); break;
    case BASS_DEVICE_TYPE_DISPLAYPORT: type_info = theApp.m_str_table.LoadText(L"TXT_OPT_PLAY_DEVICE_TYPE_DISPLAYPORT"); break;
    default: break;
    }
    m_device_info_list.SetItemText(2, 1, type_info.c_str());
}

void CPlaySettingsDlg::EnableControl()
{
    bool enable = !CPlayer::GetInstance().IsMciCore();
    bool ffmpeg_enable = CPlayer::GetInstance().IsFfmpegCore();
    EnableDlgCtrl(IDC_SOUND_FADE_CHECK, CPlayer::GetInstance().IsBassCore());
    m_device_info_list.EnableWindow(enable);
    m_output_device_combo.EnableWindow(enable);
    m_ffmpeg_cache_length.EnableWindow(ffmpeg_enable);
    m_ffmpeg_max_retry_count.EnableWindow(ffmpeg_enable);
    m_ffmpeg_url_retry_interval.EnableWindow(ffmpeg_enable);
    bool max_wait_time_support{ false }, wasapi_support{ false };
    if (ffmpeg_enable) {
        auto core = static_cast<CFfmpegCore*>(CPlayer::GetInstance().GetPlayerCore());
        wasapi_support = core->IsWASAPISupported();
        auto version = core->GetVersion();
        max_wait_time_support = version > FFMPEG_CORE_VERSION(1, 0, 0, 0);
    }
    m_ffmpeg_max_wait_time.EnableWindow(max_wait_time_support);
    EnableDlgCtrl(IDC_FFMPEG_ENABLE_WASAPI, wasapi_support);
    EnableDlgCtrl(IDC_FFMPEG_ENABLE_WASAPI_EXCLUSIVE, wasapi_support && m_data.ffmpeg_core_enable_WASAPI);

    bool midi_enable = !theApp.m_play_setting_data.use_ffmpeg && !theApp.m_play_setting_data.use_mci;
    m_sf2_path_edit.EnableWindow(midi_enable && theApp.m_format_convert_dialog_exit);		//正在进行格式转换时不允许更改音色库
    EnableDlgCtrl(IDC_MIDI_USE_INNER_LYRIC_CHECK, midi_enable);
}

void CPlaySettingsDlg::GetDataFromUi()
{
    m_data.stop_when_error = (IsDlgButtonChecked(IDC_STOP_WHEN_ERROR) != FALSE);
    m_data.auto_play_when_start = (IsDlgButtonChecked(IDC_AUTO_PLAY_WHEN_START_CHECK) != FALSE);
    m_data.show_taskbar_progress = (IsDlgButtonChecked(IDC_SHOW_TASKBAR_PROGRESS) != FALSE);
    m_data.show_playstate_icon = (IsDlgButtonChecked(IDC_SHOW_PLAY_STATE_ICON_CHECK) != FALSE);
    m_data.fade_effect = (IsDlgButtonChecked(IDC_SOUND_FADE_CHECK) != FALSE);
    m_data.continue_when_switch_playlist = (IsDlgButtonChecked(IDC_CONTINUE_WHEN_SWITCH_PLAYLIST_CHECK) != FALSE);
    m_data.use_media_trans_control = (IsDlgButtonChecked(IDC_USE_MEDIA_TRANS_CONTORL_CHECK) != FALSE);

    m_data.use_mci = (IsDlgButtonChecked(IDC_MCI_RADIO) != FALSE);
    m_data.use_ffmpeg = (IsDlgButtonChecked(IDC_FFMPEG_RADIO) != FALSE);

    m_data.ffmpeg_core_cache_length = m_ffmpeg_cache_length.GetValue();
    m_data.ffmpeg_core_max_retry_count = m_ffmpeg_max_retry_count.GetValue();
    m_data.ffmpeg_core_url_retry_interval = m_ffmpeg_url_retry_interval.GetValue();
    m_data.ffmpeg_core_max_wait_time = m_ffmpeg_max_wait_time.GetValue();
    m_data.ffmpeg_core_enable_WASAPI = (IsDlgButtonChecked(IDC_FFMPEG_ENABLE_WASAPI) != FALSE);
    m_data.ffmpeg_core_enable_WASAPI_exclusive_mode = (IsDlgButtonChecked(IDC_FFMPEG_ENABLE_WASAPI_EXCLUSIVE) != FALSE);
}

void CPlaySettingsDlg::ApplyDataToUi()
{
    ShowDeviceInfo();
    EnableControl();
}

bool CPlaySettingsDlg::InitializeControls()
{
    wstring temp;
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_PLAY_PLAY_OPT");
    SetDlgItemTextW(IDC_TXT_OPT_PLAY_PLAY_OPT_STATIC, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_PLAY_STOP_WHEN_ERROR");
    SetDlgItemTextW(IDC_STOP_WHEN_ERROR, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_PLAY_AUTO_PLAY_WHEN_START");
    SetDlgItemTextW(IDC_AUTO_PLAY_WHEN_START_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_PLAY_SHOW_TASKBAR_PROGRESS");
    SetDlgItemTextW(IDC_SHOW_TASKBAR_PROGRESS, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_PLAY_SHOW_TASKBAR_PLAY_STATE_ICON");
    SetDlgItemTextW(IDC_SHOW_PLAY_STATE_ICON_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_PLAY_SOUND_FADE");
    SetDlgItemTextW(IDC_SOUND_FADE_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_PLAY_CONTINUE_WHEN_SWITCH_PLAYLIST");
    SetDlgItemTextW(IDC_CONTINUE_WHEN_SWITCH_PLAYLIST_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_PLAY_USE_MEDIA_TRANS_CONTROL");
    SetDlgItemTextW(IDC_USE_MEDIA_TRANS_CONTORL_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_PLAY_CORE");
    SetDlgItemTextW(IDC_TXT_OPT_PLAY_CORE_STATIC, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_PLAY_CORE_BASS");
    SetDlgItemTextW(IDC_BASS_RADIO, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_PLAY_CORE_MCI");
    SetDlgItemTextW(IDC_MCI_RADIO, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_PLAY_CORE_FFMPEG");
    SetDlgItemTextW(IDC_FFMPEG_RADIO, temp.c_str());
    temp = L"<a>" + theApp.m_str_table.LoadText(L"TXT_OPT_PLAY_CORE_FFMPEG_DL_INFO") + L"</a>";
    SetDlgItemTextW(IDC_FFMPEG_DOWN_SYSLINK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_PLAY_DEVICE");
    SetDlgItemTextW(IDC_TXT_OPT_PLAY_DEVICE_STATIC, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_PLAY_DEVICE_SEL");
    SetDlgItemTextW(IDC_TXT_OPT_PLAY_DEVICE_SEL_STATIC, temp.c_str());
    // IDC_OUTPUT_DEVICE_COMBO
    // IDC_OUTPUT_DEVICE_INFO_LIST
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_PLAY_FFMPEG_CORE_SETTING");
    SetDlgItemTextW(IDC_TXT_OPT_PLAY_FFMPEG_CORE_SETTING_STATIC, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_PLAY_FFMPEG_CORE_CACHE_LENGTH");
    SetDlgItemTextW(IDC_TXT_OPT_PLAY_FFMPEG_CORE_CACHE_LENGTH_STATIC, temp.c_str());
    // IDC_FFMPEG_CACHE_LENGTH
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_PLAY_FFMPEG_CORE_MAX_RETRY_COUNT");
    SetDlgItemTextW(IDC_TXT_OPT_PLAY_FFMPEG_CORE_MAX_RETRY_COUNT_STATIC, temp.c_str());
    // IDC_FFMPEG_MAX_RETRY_COUNT
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_PLAY_FFMPEG_CORE_URL_RETRY_INTERVAL");
    SetDlgItemTextW(IDC_TXT_OPT_PLAY_FFMPEG_CORE_URL_RETRY_INTERVAL_STATIC, temp.c_str());
    // IDC_FFMPEG_URL_RETRY_INTERVAL
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_PLAY_FFMPEG_CORE_MAX_WAIT_TIME");
    SetDlgItemTextW(IDC_TXT_OPT_PLAY_FFMPEG_CORE_MAX_WAIT_TIME_STATIC, temp.c_str());
    // IDC_FFMPEG_MAX_WAIT_TIME
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_PLAY_FFMPEG_CORE_ENABLE_WASAPI");
    SetDlgItemTextW(IDC_FFMPEG_ENABLE_WASAPI, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_PLAY_FFMPEG_CORE_ENABLE_WASAPI_EXCLUSIVE");
    SetDlgItemTextW(IDC_FFMPEG_ENABLE_WASAPI_EXCLUSIVE, temp.c_str());

    temp = theApp.m_str_table.LoadText(L"TXT_OPT_DATA_MIDI_SETTING");
    SetDlgItemTextW(IDC_TXT_OPT_DATA_MIDI_SETTING_STATIC, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_DATA_MIDI_INNER_LYRIC_FIRST");
    SetDlgItemTextW(IDC_MIDI_USE_INNER_LYRIC_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_DATA_MIDI_SF2_PATH");
    SetDlgItemTextW(IDC_TXT_OPT_DATA_MIDI_SF2_PATH_STATIC, temp.c_str());
    // IDC_SF2_PATH_EDIT

    RepositionTextBasedControls({
        { CtrlTextInfo::L1, IDC_FFMPEG_RADIO, CtrlTextInfo::W16 },
        { CtrlTextInfo::C0, IDC_FFMPEG_DOWN_SYSLINK }
        });
    RepositionTextBasedControls({
        { CtrlTextInfo::L1, IDC_TXT_OPT_PLAY_DEVICE_SEL_STATIC },
        { CtrlTextInfo::C0, IDC_OUTPUT_DEVICE_COMBO }
        });
    RepositionTextBasedControls({
        { CtrlTextInfo::L1, IDC_TXT_OPT_DATA_MIDI_SF2_PATH_STATIC },
        { CtrlTextInfo::C0, IDC_SF2_PATH_EDIT }
        });

    return true;
}

void CPlaySettingsDlg::DoDataExchange(CDataExchange* pDX)
{
    CTabDlg::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_OUTPUT_DEVICE_COMBO, m_output_device_combo);
    DDX_Control(pDX, IDC_OUTPUT_DEVICE_INFO_LIST, m_device_info_list);
    DDX_Control(pDX, IDC_FFMPEG_CACHE_LENGTH, m_ffmpeg_cache_length);
    DDX_Control(pDX, IDC_FFMPEG_MAX_RETRY_COUNT, m_ffmpeg_max_retry_count);
    DDX_Control(pDX, IDC_FFMPEG_URL_RETRY_INTERVAL, m_ffmpeg_url_retry_interval);
    DDX_Control(pDX, IDC_FFMPEG_MAX_WAIT_TIME, m_ffmpeg_max_wait_time);
    DDX_Control(pDX, IDC_SF2_PATH_EDIT, m_sf2_path_edit);
}


BEGIN_MESSAGE_MAP(CPlaySettingsDlg, CTabDlg)
    ON_CBN_SELCHANGE(IDC_OUTPUT_DEVICE_COMBO, &CPlaySettingsDlg::OnCbnSelchangeOutputDeviceCombo)
    ON_NOTIFY(NM_CLICK, IDC_FFMPEG_DOWN_SYSLINK, &CPlaySettingsDlg::OnNMClickFfmpegDownSyslink)
    ON_BN_CLICKED(IDC_FFMPEG_ENABLE_WASAPI, &CPlaySettingsDlg::OnBnClickedFfmpegEnableWasapi)
    ON_BN_CLICKED(IDC_MIDI_USE_INNER_LYRIC_CHECK, &CPlaySettingsDlg::OnBnClickedMidiUseInnerLyricCheck)
    ON_EN_CHANGE(IDC_SF2_PATH_EDIT, &CPlaySettingsDlg::OnEnChangeSf2PathEdit)
    ON_MESSAGE(WM_EDIT_BROWSE_CHANGED, &CPlaySettingsDlg::OnEditBrowseChanged)
END_MESSAGE_MAP()


// CPlaySettingsDlg 消息处理程序


BOOL CPlaySettingsDlg::OnInitDialog()
{
    CTabDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

    m_toolTip.Create(this);
    m_toolTip.SetMaxTipWidth(theApp.DPI(300));
    m_toolTip.AddTool(GetDlgItem(IDC_MCI_RADIO), theApp.m_str_table.LoadText(L"TIP_OPT_PLAY_CORE_MCI").c_str());
    m_toolTip.AddTool(GetDlgItem(IDC_FFMPEG_RADIO), theApp.m_str_table.LoadText(L"TIP_OPT_PLAY_CORE_FFMPEG").c_str());
    m_toolTip.AddTool(GetDlgItem(IDC_CONTINUE_WHEN_SWITCH_PLAYLIST_CHECK), theApp.m_str_table.LoadText(L"TIP_OPT_PLAY_CONTINUE_WHEN_SWITCH_PLAYLIST").c_str());
    m_toolTip.AddTool(GetDlgItem(IDC_MIDI_USE_INNER_LYRIC_CHECK), theApp.m_str_table.LoadText(L"TIP_OPT_DATA_MIDI_INNER_LYRIC_FIRST").c_str());
    m_toolTip.SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

    //初始化各控件的状态
    if (!CWinVersionHelper::IsWindows7OrLater() || theApp.GetITaskbarList3() == nullptr)
    {
        m_data.show_taskbar_progress = false;
        m_data.show_playstate_icon = false;
        EnableDlgCtrl(IDC_SHOW_TASKBAR_PROGRESS, false);
        EnableDlgCtrl(IDC_SHOW_PLAY_STATE_ICON_CHECK, false);
    }
    if (!CWinVersionHelper::IsWindows81OrLater())
    {
        m_data.use_media_trans_control = false;
        EnableDlgCtrl(IDC_USE_MEDIA_TRANS_CONTORL_CHECK, FALSE);
    }
    CheckDlgButton(IDC_STOP_WHEN_ERROR, m_data.stop_when_error);
    CheckDlgButton(IDC_AUTO_PLAY_WHEN_START_CHECK, m_data.auto_play_when_start);
    CheckDlgButton(IDC_SHOW_TASKBAR_PROGRESS, m_data.show_taskbar_progress);
    CheckDlgButton(IDC_SHOW_PLAY_STATE_ICON_CHECK, m_data.show_playstate_icon);
    CheckDlgButton(IDC_SOUND_FADE_CHECK, m_data.fade_effect);
    CheckDlgButton(IDC_CONTINUE_WHEN_SWITCH_PLAYLIST_CHECK, m_data.continue_when_switch_playlist);
    CheckDlgButton(IDC_USE_MEDIA_TRANS_CONTORL_CHECK, m_data.use_media_trans_control);

    bool enable_ffmpeg = false;
    if (CPlayer::GetInstance().IsFfmpegCore()) {
        enable_ffmpeg = true;
    } else {
        auto h = LoadLibraryW(L"ffmpeg_core.dll");
        if (h) {
            enable_ffmpeg = true;
            FreeLibrary(h);
        }
    }
    EnableDlgCtrl(IDC_FFMPEG_RADIO, enable_ffmpeg);
    ShowDlgCtrl(IDC_FFMPEG_DOWN_SYSLINK, !enable_ffmpeg);       //未检测到ffmpeg内核时，显示下载链接
    m_data.use_ffmpeg &= enable_ffmpeg;

    if (m_data.use_ffmpeg)
        CheckDlgButton(IDC_FFMPEG_RADIO, TRUE);
    else if (m_data.use_mci)
        CheckDlgButton(IDC_MCI_RADIO, TRUE);
    else
        CheckDlgButton(IDC_BASS_RADIO, TRUE);

    //m_device_info_list.SetColor(theApp.m_app_setting_data.theme_color);
    m_device_info_list.SetMouseWheelEnable(false);
    CRect rect;
    m_device_info_list.GetClientRect(rect);
    int width0, width1;
    width0 = theApp.DPI(70);
    width1 = rect.Width() - width0 - theApp.DPI(20);

    m_device_info_list.SetExtendedStyle(m_device_info_list.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_LABELTIP);
    m_device_info_list.InsertColumn(0, theApp.m_str_table.LoadText(L"TXT_ITEM").c_str(), LVCFMT_LEFT, width0);
    m_device_info_list.InsertColumn(1, theApp.m_str_table.LoadText(L"TXT_VALUE").c_str(), LVCFMT_LEFT, width1);
    m_device_info_list.InsertItem(0, theApp.m_str_table.LoadText(L"TXT_OPT_PLAY_DEVICE_NAME").c_str());
    m_device_info_list.InsertItem(1, theApp.m_str_table.LoadText(L"TXT_OPT_PLAY_DEVICE_DRIVER").c_str());
    m_device_info_list.InsertItem(2, theApp.m_str_table.LoadText(L"TXT_OPT_PLAY_DEVICE_TYPE").c_str());
    ShowDeviceInfo();

    EnableControl();

    m_ffmpeg_cache_length.SetRange(1, 60, 5);
    m_ffmpeg_cache_length.SetValue(theApp.m_play_setting_data.ffmpeg_core_cache_length);
    m_ffmpeg_max_retry_count.SetRange(-1, 30);
    m_ffmpeg_max_retry_count.SetValue(theApp.m_play_setting_data.ffmpeg_core_max_retry_count);
    m_ffmpeg_url_retry_interval.SetRange(1, 120, 5);
    m_ffmpeg_url_retry_interval.SetValue(theApp.m_play_setting_data.ffmpeg_core_url_retry_interval);
    CheckDlgButton(IDC_FFMPEG_ENABLE_WASAPI, theApp.m_play_setting_data.ffmpeg_core_enable_WASAPI);
    CheckDlgButton(IDC_FFMPEG_ENABLE_WASAPI_EXCLUSIVE, theApp.m_play_setting_data.ffmpeg_core_enable_WASAPI_exclusive_mode);
    m_ffmpeg_max_wait_time.SetRange(100, 30000, 100);
    m_ffmpeg_max_wait_time.SetValue(theApp.m_play_setting_data.ffmpeg_core_max_wait_time);

    m_sf2_path_edit.SetWindowText(m_data.sf2_path.c_str());
    wstring sf2_filter = FilterHelper::GetSF2FileFilter();
    m_sf2_path_edit.EnableFileBrowseButton(L"SF2", sf2_filter.c_str());
    CheckDlgButton(IDC_MIDI_USE_INNER_LYRIC_CHECK, m_data.midi_use_inner_lyric);

    //设置控件不响应鼠标滚轮消息
    m_output_device_combo.SetMouseWheelEnable(false);
    m_ffmpeg_cache_length.SetMouseWheelEnable(false);
    m_ffmpeg_max_retry_count.SetMouseWheelEnable(false);
    m_ffmpeg_url_retry_interval.SetMouseWheelEnable(false);
    m_ffmpeg_max_wait_time.SetMouseWheelEnable(false);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CPlaySettingsDlg::OnCbnSelchangeOutputDeviceCombo()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.device_selected = m_output_device_combo.GetCurSel();
    ShowDeviceInfo();
}


BOOL CPlaySettingsDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if (pMsg->message == WM_MOUSEMOVE)
        m_toolTip.RelayEvent(pMsg);

    return CTabDlg::PreTranslateMessage(pMsg);
}


void CPlaySettingsDlg::OnNMClickFfmpegDownSyslink(NMHDR* pNMHDR, LRESULT* pResult)
{
    // TODO: 在此添加控件通知处理程序代码
    const wstring& info = theApp.m_str_table.LoadText(L"MSG_OPT_PLAY_CORE_FFMPEG_DL_INFO");
    if (MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_YESNO) == IDYES)
    {
        ShellExecute(NULL, _T("open"), _T("https://github.com/lifegpc/ffmpeg_core/releases"), NULL, NULL, SW_SHOW);
    }

    *pResult = 0;
}


void CPlaySettingsDlg::OnBnClickedFfmpegEnableWasapi() {
    m_data.ffmpeg_core_enable_WASAPI = (IsDlgButtonChecked(IDC_FFMPEG_ENABLE_WASAPI) != FALSE);
    EnableDlgCtrl(IDC_FFMPEG_ENABLE_WASAPI_EXCLUSIVE, m_data.ffmpeg_core_enable_WASAPI);
}

void CPlaySettingsDlg::OnBnClickedMidiUseInnerLyricCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.midi_use_inner_lyric = (((CButton*)GetDlgItem(IDC_MIDI_USE_INNER_LYRIC_CHECK))->GetCheck() != 0);
}

void CPlaySettingsDlg::OnEnChangeSf2PathEdit()
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

LRESULT CPlaySettingsDlg::OnEditBrowseChanged(WPARAM wParam, LPARAM lParam)
{
    CString str;
    m_sf2_path_edit.GetWindowText(str);
    m_data.sf2_path = str;
    return 0;
}
