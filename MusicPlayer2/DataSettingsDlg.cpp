// DataSettingsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "DataSettingsDlg.h"
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

bool CDataSettingsDlg::InitializeControls()
{
    wstring temp;
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_DATA_APP_SETTING");
    SetDlgItemTextW(IDC_TXT_OPT_DATA_APP_SETTING_STATIC, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_DATA_UPDATE_AUTO_CHECK");
    SetDlgItemTextW(IDC_CHECK_UPDATE_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_DATA_UPDATE_SOURCE_SEL");
    SetDlgItemTextW(IDC_TXT_OPT_DATA_UPDATE_SOURCE_SEL_STATIC, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_DATA_UPDATE_SOURCE_GITHUB");
    SetDlgItemTextW(IDC_GITHUB_RADIO, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_DATA_UPDATE_SOURCE_GITEE");
    SetDlgItemTextW(IDC_GITEE_RADIO, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_DATA_AUTO_RUN");
    SetDlgItemTextW(IDC_AUTO_RUN_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_DATA_LANGUAGE_SEL");
    SetDlgItemTextW(IDC_TXT_OPT_DATA_LANGUAGE_SEL_STATIC, temp.c_str());
    // IDC_COMBO1
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_DATA_DATA_FILE_CFG");
    SetDlgItemTextW(IDC_TXT_OPT_DATA_DATA_FILE_CFG_STATIC, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_DATA_DATA_FILE_SAVE_DIR_APPDATA");
    SetDlgItemTextW(IDC_SAVE_TO_APPDATA_RADIO, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_DATA_DATA_FILE_SAVE_DIR_PROGRAM");
    SetDlgItemTextW(IDC_SAVE_TO_PROGRAM_DIR_RADIO, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_DATA_DATA_FILE_DIR_OPEN");
    SetDlgItemTextW(IDC_OPEN_CONFIG_PATH_BUTTON, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_DATA_CLOSE_MAIN_WINDOW");
    SetDlgItemTextW(IDC_TXT_OPT_DATA_CLOSE_MAIN_WINDOW_STATIC, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_DATA_CLOSE_MAIN_WINDOW_MINIMIZE_NOTIFY_AREA");
    SetDlgItemTextW(IDC_MINIMIZE_TO_NOTIFY_RADIO, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_DATA_CLOSE_MAIN_WINDOW_EXIT");
    SetDlgItemTextW(IDC_EXIT_PROGRAM_RADIO, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_DATA_DL_SETTING");
    SetDlgItemTextW(IDC_TXT_OPT_DATA_DL_SETTING_STATIC, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_DATA_AUTO_DL_LYRIC");
    SetDlgItemTextW(IDC_LYRIC_AUTO_DOWNLOAD_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_DATA_AUTO_DL_LYRIC_SAVE_SEL");
    SetDlgItemTextW(IDC_TXT_OPT_DATA_AUTO_DL_LYRIC_SAVE_SEL_STATIC, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_DATA_AUTO_DL_LYRIC_SAVE_SONG_DIR");
    SetDlgItemTextW(IDC_SAVE_TO_SONG_FOLDER, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_DATA_AUTO_DL_LYRIC_SAVE_LYRIC_DIR");
    SetDlgItemTextW(IDC_SAVE_TO_LYRIC_FOLDER, temp.c_str());

    SetDlgControlText(IDC_DOWN_LOAD_LYRIC_TRANSLATION_FORMAT_STATIC, L"TXT_OPT_DATA_DL_LYRIC_TRANSLATION_FORMAT_SEL");
    SetDlgControlText(IDC_LYRIC_AND_TRANSLATION_IN_SAME_LINE_RADIO, L"TXT_OPT_DATA_DL_LYRIC_TRANSLATION_FORMAT_SAME_LINE");
    SetDlgControlText(IDC_LYRIC_AND_TRANSLATION_IN_DIFFERENT_LINE_RADIO, L"TXT_OPT_DATA_DL_LYRIC_TRANSLATION_FORMAT_DIFFERENT_LINE");

    temp = theApp.m_str_table.LoadText(L"TXT_OPT_DATA_AUTO_DL_COVER");
    SetDlgItemTextW(IDC_COVER_AUTO_DOWNLOAD_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_DATA_AUTO_DL_COVER_SAVE_SEL");
    SetDlgItemTextW(IDC_TXT_OPT_DATA_AUTO_DL_COVER_SAVE_SEL_STATIC, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_DATA_AUTO_DL_COVER_SAVE_SONG_DIR");
    SetDlgItemTextW(IDC_SAVE_TO_SONG_FOLDER3, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_DATA_AUTO_DL_COVER_SAVE_COVER_DIR");
    SetDlgItemTextW(IDC_SAVE_TO_ALBUM_FOLDER3, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_DATA_AUTO_DL_ONLY_WHEN_TAG_FULL");
    SetDlgItemTextW(IDC_DOWNLOAD_WHEN_TAG_FULL_CHECK, temp.c_str());

    SetDlgControlText(IDC_TXT_OPERATION_SETTINGS_STATIC, L"TXT_OPT_DATA_OPERATION_SETTINGS");
    SetDlgControlText(IDC_GLOBAL_MOUSE_WHEEL_VOLUME_ADJUSTMENT_CHECK, L"TXT_OPT_DATA_GLOBAL_MOUSE_WHEEL_VOLUME_ADJUSTMENT");

    return false;
}

void CDataSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
    CTabDlg::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO1, m_language_combo);
}

void CDataSettingsDlg::GetDataFromUi()
{
    m_data.minimize_to_notify_icon = (((CButton*)GetDlgItem(IDC_MINIMIZE_TO_NOTIFY_RADIO))->GetCheck() != 0);
    m_data.save_lyric_to_song_folder = (((CButton*)GetDlgItem(IDC_SAVE_TO_SONG_FOLDER))->GetCheck() != 0);
    m_data.save_album_to_song_folder = (((CButton*)GetDlgItem(IDC_SAVE_TO_SONG_FOLDER3))->GetCheck() != 0);
    m_data.download_lyric_text_and_translation_in_same_line = (IsDlgButtonChecked(IDC_LYRIC_AND_TRANSLATION_IN_SAME_LINE_RADIO) != 0);

    //获取语言的设置
    int sel_language = m_language_combo.GetCurSel();
    if (sel_language == 0)
        m_data.language_.clear();
    else
    {
        sel_language -= 1;
        const auto& language_list = theApp.m_str_table.GetLanguageList();
        if (sel_language >= 0 && sel_language < static_cast<int>(language_list.size()))
            m_data.language_ = language_list[sel_language].bcp_47;
    }
    if (m_data.language_ != theApp.m_general_setting_data.language_)
    {
        const wstring& info = theApp.m_str_table.LoadText(L"MSG_OPT_DATA_LANGUAGE_CHANGE_INFO");
        MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
    }

    //获取数据文件保存位置的设置
    m_data.portable_mode = (IsDlgButtonChecked(IDC_SAVE_TO_PROGRAM_DIR_RADIO) != 0);
    if (m_data.portable_mode != theApp.m_general_setting_data.portable_mode)
    {
        const wstring& info = theApp.m_str_table.LoadText(L"MSG_OPT_DATA_CFG_DIR_CHANGED_INFO");
        MessageBox(info.c_str(), NULL, MB_ICONINFORMATION | MB_OK);
    }

    m_data.global_mouse_wheel_volume_adjustment = (IsDlgButtonChecked(IDC_GLOBAL_MOUSE_WHEEL_VOLUME_ADJUSTMENT_CHECK) != 0);
}

void CDataSettingsDlg::ApplyDataToUi()
{
    EnableControl();
}


BEGIN_MESSAGE_MAP(CDataSettingsDlg, CTabDlg)
    ON_BN_CLICKED(IDC_COVER_AUTO_DOWNLOAD_CHECK, &CDataSettingsDlg::OnBnClickedCoverAutoDownloadCheck)
    ON_BN_CLICKED(IDC_LYRIC_AUTO_DOWNLOAD_CHECK, &CDataSettingsDlg::OnBnClickedLyricAutoDownloadCheck)
    ON_BN_CLICKED(IDC_CHECK_UPDATE_CHECK, &CDataSettingsDlg::OnBnClickedCheckUpdateCheck)
    ON_BN_CLICKED(IDC_DOWNLOAD_WHEN_TAG_FULL_CHECK, &CDataSettingsDlg::OnBnClickedDownloadWhenTagFullCheck)
    ON_BN_CLICKED(IDC_AUTO_RUN_CHECK, &CDataSettingsDlg::OnBnClickedAutoRunCheck)
    ON_BN_CLICKED(IDC_GITHUB_RADIO, &CDataSettingsDlg::OnBnClickedGithubRadio)
    ON_BN_CLICKED(IDC_GITEE_RADIO, &CDataSettingsDlg::OnBnClickedGiteeRadio)
    ON_BN_CLICKED(IDC_OPEN_CONFIG_PATH_BUTTON, &CDataSettingsDlg::OnBnClickedOpenConfigPathButton)
END_MESSAGE_MAP()


// CDataSettingsDlg 消息处理程序


BOOL CDataSettingsDlg::OnInitDialog()
{
    CTabDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

    m_language_combo.AddString(theApp.m_str_table.LoadText(L"TXT_OPT_DATA_LANGUAGE_FOLLOWING_SYSTEM").c_str());
    const auto& language_list = theApp.m_str_table.GetLanguageList();
    int language_sel{};
    for (size_t i{}; i < language_list.size(); ++i)
    {
        m_language_combo.AddString(language_list[i].display_name.c_str());
        if (language_list[i].bcp_47 == m_data.language_)
            language_sel = i + 1;
    }
    ASSERT(language_sel != 0 || m_data.language_.empty());  // 仅当设置为“跟随系统(空)”时索引才可能为0
    m_language_combo.SetCurSel(language_sel);

    m_auto_run = theApp.GetAutoRun();
    CheckDlgButton(IDC_AUTO_RUN_CHECK, m_auto_run);
    if (m_data.update_source == static_cast<int>(CUpdateHelper::UpdateSource::GitHubSource))
        CheckDlgButton(IDC_GITHUB_RADIO, TRUE);
    else
        CheckDlgButton(IDC_GITEE_RADIO, TRUE);

    CheckDlgButton(IDC_SAVE_TO_APPDATA_RADIO, !m_data.portable_mode);
    CheckDlgButton(IDC_SAVE_TO_PROGRAM_DIR_RADIO, m_data.portable_mode);
    EnableDlgCtrl(IDC_SAVE_TO_PROGRAM_DIR_RADIO, theApp.m_module_dir_writable);

    ((CButton*)GetDlgItem(IDC_COVER_AUTO_DOWNLOAD_CHECK))->SetCheck(m_data.auto_download_album_cover);
    ((CButton*)GetDlgItem(IDC_LYRIC_AUTO_DOWNLOAD_CHECK))->SetCheck(m_data.auto_download_lyric);
    ((CButton*)GetDlgItem(IDC_DOWNLOAD_WHEN_TAG_FULL_CHECK))->SetCheck(m_data.auto_download_only_tag_full);
    ((CButton*)GetDlgItem(IDC_CHECK_UPDATE_CHECK))->SetCheck(m_data.check_update_when_start);

    CheckDlgButton(IDC_LYRIC_AND_TRANSLATION_IN_SAME_LINE_RADIO, m_data.download_lyric_text_and_translation_in_same_line);
    CheckDlgButton(IDC_LYRIC_AND_TRANSLATION_IN_DIFFERENT_LINE_RADIO, !m_data.download_lyric_text_and_translation_in_same_line);

    if (m_data.minimize_to_notify_icon)
        ((CButton*)GetDlgItem(IDC_MINIMIZE_TO_NOTIFY_RADIO))->SetCheck(TRUE);
    else
        ((CButton*)GetDlgItem(IDC_EXIT_PROGRAM_RADIO))->SetCheck(TRUE);

    if (m_data.save_lyric_to_song_folder)
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
    
    // 设置封面存储位置选项
    if (m_data.save_lyric_to_song_folder)
        ((CButton*)GetDlgItem(IDC_SAVE_TO_SONG_FOLDER3))->SetCheck(TRUE);
    else
        ((CButton*)GetDlgItem(IDC_SAVE_TO_ALBUM_FOLDER3))->SetCheck(TRUE);

    // 判断封面文件夹是否存在
    bool album_path_exist = CCommon::FolderExist(theApp.m_app_setting_data.album_cover_path);
    if (!album_path_exist)		// 如果封面文件夹不存在，则禁用“保存到歌词文件夹”单选按钮，并强制选中“保存到歌曲所在目录”
    {
        ((CButton*)GetDlgItem(IDC_SAVE_TO_ALBUM_FOLDER3))->EnableWindow(FALSE);
        ((CButton*)GetDlgItem(IDC_SAVE_TO_ALBUM_FOLDER3))->SetCheck(FALSE);
        ((CButton*)GetDlgItem(IDC_SAVE_TO_SONG_FOLDER3))->SetCheck(TRUE);
        m_data.save_album_to_song_folder = true;
    }

    CheckDlgButton(IDC_GLOBAL_MOUSE_WHEEL_VOLUME_ADJUSTMENT_CHECK, m_data.global_mouse_wheel_volume_adjustment);

    m_toolTip.Create(this);
    m_toolTip.SetMaxTipWidth(theApp.DPI(300));
    m_toolTip.AddTool(GetDlgItem(IDC_DOWNLOAD_WHEN_TAG_FULL_CHECK), theApp.m_str_table.LoadText(L"TIP_OPT_DATA_AUTO_DL_ONLY_WHEN_TAG_FULL").c_str());
    m_toolTip.AddTool(GetDlgItem(IDC_SAVE_TO_APPDATA_RADIO), theApp.m_appdata_dir.c_str());
    m_toolTip.AddTool(GetDlgItem(IDC_SAVE_TO_PROGRAM_DIR_RADIO), theApp.m_module_dir.c_str());
    m_toolTip.AddTool(GetDlgItem(IDC_GLOBAL_MOUSE_WHEEL_VOLUME_ADJUSTMENT_CHECK), theApp.m_str_table.LoadText(L"TXT_OPT_DATA_GLOBAL_MOUSE_WHEEL_VOLUME_ADJUSTMENT_TIP").c_str());

    m_toolTip.SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

    EnableControl();

    //设置控件不响应鼠标滚轮消息
    m_language_combo.SetMouseWheelEnable(false);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CDataSettingsDlg::EnableControl()
{
    // bool enable = CPlayer::GetInstance().IsBassCore();
    bool enable = !theApp.m_play_setting_data.use_ffmpeg && !theApp.m_play_setting_data.use_mci;
    CWnd* pWnd = GetDlgItem(IDC_BROWSE_BUTTON);
    if (pWnd != nullptr)
        pWnd->EnableWindow(enable && theApp.m_format_convert_dialog_exit);

    ((CButton*)GetDlgItem(IDC_SAVE_TO_SONG_FOLDER))->EnableWindow(m_data.auto_download_lyric);
    ((CButton*)GetDlgItem(IDC_SAVE_TO_LYRIC_FOLDER))->EnableWindow(m_data.auto_download_lyric && CCommon::FolderExist(theApp.m_lyric_setting_data.lyric_path));

    ((CButton*)GetDlgItem(IDC_SAVE_TO_SONG_FOLDER3))->EnableWindow(m_data.auto_download_album_cover);
    ((CButton*)GetDlgItem(IDC_SAVE_TO_ALBUM_FOLDER3))->EnableWindow(m_data.auto_download_album_cover && CCommon::FolderExist(theApp.m_app_setting_data.album_cover_path));
}


void CDataSettingsDlg::OnBnClickedCoverAutoDownloadCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.auto_download_album_cover = (((CButton*)GetDlgItem(IDC_COVER_AUTO_DOWNLOAD_CHECK))->GetCheck() != 0);
    EnableControl();
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


void CDataSettingsDlg::OnBnClickedDownloadWhenTagFullCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.auto_download_only_tag_full = (((CButton*)GetDlgItem(IDC_DOWNLOAD_WHEN_TAG_FULL_CHECK))->GetCheck() != 0);
}


void CDataSettingsDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类


    CTabDlg::OnOK();
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


void CDataSettingsDlg::OnBnClickedOpenConfigPathButton()
{
    // TODO: 在此添加控件通知处理程序代码
    ShellExecute(NULL, _T("explore"), theApp.m_config_dir.c_str(), NULL, NULL, SW_SHOWNORMAL);
}
