// AppearanceSettingDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "AppearanceSettingDlg.h"
#include "FilterHelper.h"


// CAppearanceSettingDlg 对话框

IMPLEMENT_DYNAMIC(CAppearanceSettingDlg, CTabDlg)

CAppearanceSettingDlg::CAppearanceSettingDlg(CWnd* pParent /*=NULL*/)
    : CTabDlg(IDD_APPEREANCE_SETTING_DLG, pParent)
{

}

CAppearanceSettingDlg::~CAppearanceSettingDlg()
{
}

bool CAppearanceSettingDlg::InitializeControls()
{
    wstring temp;
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_VISUAL_EFFECTS");
    SetDlgItemTextW(IDC_VISUAL_EFFECTS_STATIC, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_SPECTRUM_SHOW");
    SetDlgItemTextW(IDC_SHOW_SPECTRUM_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_SPECTRUM_LOW_FREQ_IN_CENTER");
    SetDlgItemTextW(IDC_LOW_FREQ_IN_CENTER_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_SPECTRUM_HEIGHT");
    SetDlgItemTextW(IDC_TXT_SPECTRUM_HIGHT_STATIC, temp.c_str());
    // IDC_SPECTRUM_HEIGHT_SLIDER
    // IDC_SPECTRUM_HEIGHT_STATIC
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_COVER_SHOW");
    SetDlgItemTextW(IDC_SHOW_ALBUM_COVER_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_COVER_FIT");
    SetDlgItemTextW(IDC_TXT_COVER_FIT_STATIC, temp.c_str());
    // IDC_ALBUM_FIT_COMBO
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_LYRIC_BG");
    SetDlgItemTextW(IDC_LYRIC_BACKGROUND_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_ROUND_CORNERS");
    SetDlgItemTextW(IDC_BTN_ROUND_CORNERS_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_DARK_MODE");
    SetDlgItemTextW(IDC_DARK_MODE_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_MAIN_WINDOW_TRANSPARENCY");
    SetDlgItemTextW(IDC_TXT_TRANSPARENT_STATIC, temp.c_str());
    // IDC_TRANSPARENT_SLIDER
    // IDC_TRANSPARENT_STATIC

    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_BG_SETTING");
    SetDlgItemTextW(IDC_TXT_BG_SETTING_STATIC, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_BG_ENABLE");
    SetDlgItemTextW(IDC_ENABLE_BACKGROUND_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_BG_USE_DESKTOP");
    SetDlgItemTextW(IDC_USE_DESKTOP_BACKGROUND_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_BG_TRANSPARENCY");
    SetDlgItemTextW(IDC_TXT_BG_TRANSPARENCY_STATIC, temp.c_str());
    // IDC_BACKGROUND_TRANSPARENCY_SLIDER
    // IDC_BACKGROUND_TRANSPARENCY_STATIC
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_BG_USE_COVER");
    SetDlgItemTextW(IDC_ALBUM_COVER_BACKGROUND_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_BG_GAUSS_BLUR");
    SetDlgItemTextW(IDC_BACKGROUND_GAUSS_BLUR_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_BG_GAUSS_BLUR_RADIUS");
    SetDlgItemTextW(IDC_TXT_GAUSS_BLUR_RADIUS_STATIC, temp.c_str());
    // IDC_GAUSS_BLURE_RADIUS_SLIDER
    // IDC_GAUSS_BLUR_RADIUS_STATIC
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_BG_DEFAULT_IMAGE");
    SetDlgItemTextW(IDC_TXT_DEFAULT_BG_PATH_EDIT_STATIC, temp.c_str());
    // IDC_DEFAULT_BACKGROUND_PATH_EDIT

    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_COVER_OPT");
    SetDlgItemTextW(IDC_TXT_COVER_OPT_STATIC, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_COVER_USE_INNER_FIRST");
    SetDlgItemTextW(IDC_USE_INNER_IMAGE_FIRST_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_COVER_OUT_IMAGE_ALLOW");
    SetDlgItemTextW(IDC_USE_OUT_IMAGE_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_COVER_OUT_IMAGE_FILE_NAME");
    SetDlgItemTextW(IDC_TXT_EXT_COVER_FILE_NAME_STATIC, temp.c_str());
    // IDC_DEFAULT_COVER_NAME_EDIT
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_COVER_OUT_FOLDER");
    SetDlgItemTextW(IDC_TXT_EXT_COVER_FOLDER_STATIC, temp.c_str());
    // IDC_ALBUM_COVER_PATH_EDIT

    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_COLOR_THEME");
    SetDlgItemTextW(IDC_TXT_THEME_COLOR_STATIC, temp.c_str());
    // IDC_COLOR_STATIC
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_COLOR_PRESET");
    SetDlgItemTextW(IDC_PRESET_COLOR_STATIC, temp.c_str());
    // IDC_COLOR_STATIC2
    // IDC_COLOR_STATIC3
    // IDC_COLOR_STATIC4
    // IDC_COLOR_STATIC5
    // IDC_COLOR_STATIC6
    // IDC_COLOR_STATIC7
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_COLOR_MORE");
    SetDlgItemTextW(IDC_SET_PROGRESS_COLOR_BUTTON, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_COLOR_FOLLOW_SYSTEM");
    SetDlgItemTextW(IDC_FOLLOW_SYSTEM_COLOR_CHECK, temp.c_str());

    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_STATUS_BAR");
    SetDlgItemTextW(IDC_TXT_STATUS_BAR_STATIC, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_ALWAYS_SHOW_STATUS_BAR");
    SetDlgItemTextW(IDC_ALWAYS_SHOW_STATUSBAR_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_SHOW_NEXT_TRACK");
    SetDlgItemTextW(IDC_SHOW_NEXT_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_SHOW_FPS");
    SetDlgItemTextW(IDC_SHOW_FPS_CHECK, temp.c_str());

    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_TITLE_BAR");
    SetDlgItemTextW(IDC_TXT_TITLE_BAR_STATIC, temp.c_str());

    SetDlgControlText(IDC_USE_SYSTEM_TITLEBAR_RADIO, L"TXT_OPT_APC_USE_SYSTEM_TITLE_BAR");
    SetDlgControlText(IDC_USE_OWNER_DRAW_TITLEBAR_RADIO, L"TXT_OPT_APC_USE_OWNER_DRAW_TITLE_BAR");

    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_TITLE_BAR_BTN_SEL");
    SetDlgItemTextW(IDC_TXT_TITLE_BAR_BTN_SEL_STATIC, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_TITLE_BAR_BTN_SETTING");
    SetDlgItemTextW(IDC_SHOW_SETTINGS_BTN_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_TITLE_BAR_BTN_SKIN");
    SetDlgItemTextW(IDC_SHOW_SKIN_BTN_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_TITLE_BAR_BTN_MINI_MODE");
    SetDlgItemTextW(IDC_SHOW_MINI_MODE_BTN_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_TITLE_BAR_BTN_FULLSCREEN");
    SetDlgItemTextW(IDC_SHOW_FULLSCREEN_BTN_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_TITLE_BAR_BTN_MINIMIZE");
    SetDlgItemTextW(IDC_SHOW_MINIMIZE_BTN_CHECK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_TITLE_BAR_BTN_MAXIMIZE");
    SetDlgItemTextW(IDC_SHOW_MAXIMIZE_BTN_CHECK, temp.c_str());
    SetDlgControlText(IDC_SHOW_DARK_LIGHT_BTN_CHECK, L"TXT_OPT_APC_TITLE_BAR_BTN_DARK_KIGHT");

    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_NOTIFY_ICON");
    SetDlgItemTextW(IDC_NA_ICO_STATIC, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_NOTIFY_ICON_PREVIEW");
    SetDlgItemTextW(IDC_PREVIEW_STATIC, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_NOTIFY_ICON_SEL");
    SetDlgItemTextW(IDC_TXT_NA_ICO_SEL_STATIC, temp.c_str());
    // IDC_COMBO1
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_NOTIFY_ICON_AUTO_ADAPT");
    SetDlgItemTextW(IDC_NOTIFY_ICON_AUTO_ADAPT_CHECK, temp.c_str());

    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_ADVANCED");
    SetDlgItemTextW(IDC_TXT_ADV_STATIC, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_UI_REFRESH_INTERVAL");
    SetDlgItemTextW(IDC_TXT_UI_INTERVAL_STATIC, temp.c_str());
    // IDC_UI_INTERVAL_EDIT
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_UI_REFRESH_INTERVAL_RESTORE");
    SetDlgItemTextW(IDC_RESTORE_DEFAULT_BUTTON, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OPT_APC_COVER_HQ");
    SetDlgItemTextW(IDC_DEFAULT_ALBUM_COVER_HQ, temp.c_str());

    return true;
}

void CAppearanceSettingDlg::DoDataExchange(CDataExchange* pDX)
{
    CTabDlg::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TRANSPARENT_SLIDER, m_transparency_slid);
    DDX_Control(pDX, IDC_COLOR_STATIC, m_color_static);
    DDX_Control(pDX, IDC_COLOR_STATIC2, m_color_static1);
    DDX_Control(pDX, IDC_COLOR_STATIC3, m_color_static2);
    DDX_Control(pDX, IDC_COLOR_STATIC4, m_color_static3);
    DDX_Control(pDX, IDC_COLOR_STATIC5, m_color_static4);
    DDX_Control(pDX, IDC_COLOR_STATIC6, m_color_static5);
    DDX_Control(pDX, IDC_COLOR_STATIC7, m_color_static6);
    DDX_Control(pDX, IDC_COLOR_STATIC8, m_color_static7);
    DDX_Control(pDX, IDC_FOLLOW_SYSTEM_COLOR_CHECK, m_follow_system_color_check);
    DDX_Control(pDX, IDC_SPECTRUM_HEIGHT_SLIDER, m_spectrum_height_slid);
    DDX_Control(pDX, IDC_SHOW_ALBUM_COVER_CHECK, m_show_album_cover_chk);
    DDX_Control(pDX, IDC_ALBUM_FIT_COMBO, m_album_cover_fit_combo);
    DDX_Control(pDX, IDC_ALBUM_COVER_BACKGROUND_CHECK, m_album_cover_as_background_chk);
    DDX_Control(pDX, IDC_SHOW_SPECTRUM_CHECK, m_show_spectrum_chk);
    DDX_Control(pDX, IDC_BACKGROUND_TRANSPARENCY_SLIDER, m_back_transparency_slid);
    DDX_Control(pDX, IDC_USE_OUT_IMAGE_CHECK, m_use_out_image_chk);
    DDX_Control(pDX, IDC_GAUSS_BLURE_RADIUS_SLIDER, m_gauss_blur_radius_sld);
    DDX_Control(pDX, IDC_BACKGROUND_GAUSS_BLUR_CHECK, m_background_gauss_blur_chk);
    DDX_Control(pDX, IDC_LYRIC_BACKGROUND_CHECK, m_lyric_background_chk);
    DDX_Control(pDX, IDC_DARK_MODE_CHECK, m_dark_mode_chk);
    DDX_Control(pDX, IDC_USE_INNER_IMAGE_FIRST_CHECK, m_use_inner_image_first_chk);
    DDX_Control(pDX, IDC_ENABLE_BACKGROUND_CHECK, m_enable_background_chk);
    DDX_Control(pDX, IDC_LOW_FREQ_IN_CENTER_CHECK, m_low_freq_in_center_chk);
    DDX_Control(pDX, IDC_DEFAULT_ALBUM_COVER_HQ, m_default_cover_hq_chk);
    DDX_Control(pDX, IDC_UI_INTERVAL_EDIT, m_ui_refresh_interval_edit);
    DDX_Control(pDX, IDC_COMBO1, m_icon_select_combo);
    DDX_Control(pDX, IDC_NOTIFY_ICON_AUTO_ADAPT_CHECK, m_notify_icon_auto_adapt_chk);
    DDX_Control(pDX, IDC_BTN_ROUND_CORNERS_CHECK, m_btn_round_corners_chk);
    DDX_Control(pDX, IDC_DEFAULT_BACKGROUND_PATH_EDIT, m_default_background_edit);
    DDX_Control(pDX, IDC_DEFAULT_COVER_NAME_EDIT, m_album_cover_name_edit);
    DDX_Control(pDX, IDC_ALBUM_COVER_PATH_EDIT, m_album_cover_path_edit);
}

void CAppearanceSettingDlg::SetTransparency()
{
    ::SetWindowLong(m_hMainWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
    ::SetLayeredWindowAttributes(m_hMainWnd, 0, m_data.window_transparency * 255 / 100, LWA_ALPHA);  //透明度取值范围为0~255
}

void CAppearanceSettingDlg::ClickColor()
{
    //点击了预置颜色中的其中一个时，取消“跟随系统主题色”复选按钮的选中
    m_color_static.SetFillColor(m_data.theme_color.original_color);
    m_data.theme_color_follow_system = false;
    m_follow_system_color_check.SetCheck(FALSE);
    //GetDlgItem(IDC_SET_PROGRESS_COLOR_BUTTON)->EnableWindow();
}

int CAppearanceSettingDlg::SpectrumHeightChg(int value)
{
    int rtn;
    rtn = static_cast<int>(22.72727272727 * std::sqrt(0.088 * value - 0.39) - 15.9090909090);
    if (rtn < 0) rtn = 0;
    if (rtn > 100) rtn = 100;
    return rtn;
}

int CAppearanceSettingDlg::SpectrumHeightRChg(int value)
{
    int rtn;
    rtn = static_cast<int>(0.022 * value * value + 0.7 * value + 10);
    if (rtn < 10) rtn = 10;
    if (rtn > 300) rtn = 300;
    return rtn;
}

void CAppearanceSettingDlg::SetControlEnable()
{
    m_album_cover_fit_combo.EnableWindow(m_data.show_album_cover);
    m_album_cover_name_edit.EnableWindow(m_data.use_out_image);
    m_album_cover_path_edit.EnableWindow(m_data.use_out_image);
    m_spectrum_height_slid.EnableWindow(m_data.show_spectrum);
    m_album_cover_as_background_chk.EnableWindow(m_data.enable_background);
    m_back_transparency_slid.EnableWindow(m_data.enable_background);
    m_background_gauss_blur_chk.EnableWindow(m_data.enable_background && m_data.album_cover_as_background);
    m_gauss_blur_radius_sld.EnableWindow(m_data.enable_background && m_data.album_cover_as_background && m_data.background_gauss_blur);

    m_default_background_edit.EnableWindow(m_data.enable_background && !m_data.use_desktop_background);
    EnableDlgCtrl(IDC_USE_DESKTOP_BACKGROUND_CHECK, m_data.enable_background);

    EnableDlgCtrl(IDC_SHOW_NEXT_CHECK, m_data.always_show_statusbar);
    EnableDlgCtrl(IDC_SHOW_FPS_CHECK, m_data.always_show_statusbar);

    EnableDlgCtrl(IDC_SHOW_MINIMIZE_BTN_CHECK, !m_data.show_window_frame);
    EnableDlgCtrl(IDC_SHOW_MAXIMIZE_BTN_CHECK, !m_data.show_window_frame);
}

void CAppearanceSettingDlg::CalculatePreviewBitmapRect()
{
    CWnd* pStatic = GetDlgItem(IDC_PREVIEW_STATIC);
    if (pStatic != nullptr)
    {
        CRect rc_static;
        ::GetWindowRect(pStatic->GetSafeHwnd(), rc_static);
        ScreenToClient(rc_static);
        m_notify_icon_preview.top = rc_static.bottom + theApp.DPI(4);
        m_notify_icon_preview.left = rc_static.left;
        m_notify_icon_preview.right = m_notify_icon_preview.left + PREVIEW_WIDTH;
        m_notify_icon_preview.bottom = m_notify_icon_preview.top + PREVIEW_HEIGHT;
    }

    CWnd* use_system_titlebar_radio = GetDlgItem(IDC_USE_SYSTEM_TITLEBAR_RADIO);
    if (use_system_titlebar_radio != nullptr)
    {
        ::GetWindowRect(use_system_titlebar_radio->GetSafeHwnd(), m_system_titlebar_preview_rect);
        ScreenToClient(m_system_titlebar_preview_rect);
        m_system_titlebar_preview_rect.bottom = m_system_titlebar_preview_rect.top - theApp.DPI(4);
        m_system_titlebar_preview_rect.top = m_system_titlebar_preview_rect.bottom - theApp.DPI(50);
    }

    CWnd* use_owner_draw_titlebar_radio = GetDlgItem(IDC_USE_OWNER_DRAW_TITLEBAR_RADIO);
    if (use_owner_draw_titlebar_radio != nullptr)
    {
        ::GetWindowRect(use_owner_draw_titlebar_radio->GetSafeHwnd(), m_owner_draw_titlebar_preview_rect);
        ScreenToClient(m_owner_draw_titlebar_preview_rect);
        m_owner_draw_titlebar_preview_rect.bottom = m_owner_draw_titlebar_preview_rect.top - theApp.DPI(4);
        m_owner_draw_titlebar_preview_rect.top = m_owner_draw_titlebar_preview_rect.bottom - theApp.DPI(50);
        m_owner_draw_titlebar_preview_rect.right = m_owner_draw_titlebar_preview_rect.left + m_system_titlebar_preview_rect.Width();
    }

}

void CAppearanceSettingDlg::GetDataFromUi()
{
    m_data.ui_refresh_interval = m_ui_refresh_interval_edit.GetValue();

    m_data.always_show_statusbar = (IsDlgButtonChecked(IDC_ALWAYS_SHOW_STATUSBAR_CHECK) != 0);
    m_data.show_window_frame = (IsDlgButtonChecked(IDC_USE_SYSTEM_TITLEBAR_RADIO) != 0);

    m_data.show_settings_btn_in_titlebar = (IsDlgButtonChecked(IDC_SHOW_SETTINGS_BTN_CHECK) != 0);
    m_data.show_skin_btn_in_titlebar = (IsDlgButtonChecked(IDC_SHOW_SKIN_BTN_CHECK) != 0);
    m_data.show_minimode_btn_in_titlebar = (IsDlgButtonChecked(IDC_SHOW_MINI_MODE_BTN_CHECK) != 0);
    m_data.show_fullscreen_btn_in_titlebar = (IsDlgButtonChecked(IDC_SHOW_FULLSCREEN_BTN_CHECK) != 0);
    m_data.show_minimize_btn_in_titlebar = (IsDlgButtonChecked(IDC_SHOW_MINIMIZE_BTN_CHECK) != 0);
    m_data.show_maximize_btn_in_titlebar = (IsDlgButtonChecked(IDC_SHOW_MAXIMIZE_BTN_CHECK) != 0);
    m_data.show_dark_light_btn_in_titlebar = (IsDlgButtonChecked(IDC_SHOW_DARK_LIGHT_BTN_CHECK) != 0);
}

void CAppearanceSettingDlg::ApplyDataToUi()
{
    m_color_static.SetFillColor(theApp.m_app_setting_data.theme_color.original_color);
}

void CAppearanceSettingDlg::DrawColor()
{
    m_color_static.SetFillColor(m_data.theme_color.original_color);
    m_color_static1.SetFillColor(m_color1);
    m_color_static2.SetFillColor(m_color2);
    m_color_static3.SetFillColor(m_color3);
    m_color_static4.SetFillColor(m_color4);
    m_color_static5.SetFillColor(m_color5);
    m_color_static6.SetFillColor(m_color6);
    m_color_static7.SetFillColor(m_color7);
}


BEGIN_MESSAGE_MAP(CAppearanceSettingDlg, CTabDlg)
    //	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_TRANSPARENT_SLIDER, &CAppearanceSettingDlg::OnNMReleasedcaptureTransparentSlider)
    ON_WM_HSCROLL()
    ON_BN_CLICKED(IDC_SET_PROGRESS_COLOR_BUTTON, &CAppearanceSettingDlg::OnBnClickedSetThemeButton)
    ON_STN_CLICKED(IDC_COLOR_STATIC2, &CAppearanceSettingDlg::OnStnClickedColorStatic2)
    ON_STN_CLICKED(IDC_COLOR_STATIC3, &CAppearanceSettingDlg::OnStnClickedColorStatic3)
    ON_STN_CLICKED(IDC_COLOR_STATIC4, &CAppearanceSettingDlg::OnStnClickedColorStatic4)
    ON_STN_CLICKED(IDC_COLOR_STATIC5, &CAppearanceSettingDlg::OnStnClickedColorStatic5)
    ON_STN_CLICKED(IDC_COLOR_STATIC6, &CAppearanceSettingDlg::OnStnClickedColorStatic6)
    ON_STN_CLICKED(IDC_COLOR_STATIC7, &CAppearanceSettingDlg::OnStnClickedColorStatic7)
    ON_STN_CLICKED(IDC_COLOR_STATIC8, &CAppearanceSettingDlg::OnStnClickedColorStatic8)
    ON_BN_CLICKED(IDC_FOLLOW_SYSTEM_COLOR_CHECK, &CAppearanceSettingDlg::OnBnClickedFollowSystemColorCheck)
    ON_BN_CLICKED(IDC_SHOW_ALBUM_COVER_CHECK, &CAppearanceSettingDlg::OnBnClickedShowAlbumCoverCheck)
    ON_CBN_SELCHANGE(IDC_ALBUM_FIT_COMBO, &CAppearanceSettingDlg::OnCbnSelchangeAlbumFitCombo)
    ON_BN_CLICKED(IDC_ALBUM_COVER_BACKGROUND_CHECK, &CAppearanceSettingDlg::OnBnClickedAlbumCoverBackgroundCheck)
    ON_BN_CLICKED(IDC_SHOW_SPECTRUM_CHECK, &CAppearanceSettingDlg::OnBnClickedShowSpectrumCheck)
    ON_BN_CLICKED(IDC_USE_OUT_IMAGE_CHECK, &CAppearanceSettingDlg::OnBnClickedUseOutImageCheck)
    ON_BN_CLICKED(IDC_BACKGROUND_GAUSS_BLUR_CHECK, &CAppearanceSettingDlg::OnBnClickedBackgroundGaussBlurCheck)
    ON_BN_CLICKED(IDC_LYRIC_BACKGROUND_CHECK, &CAppearanceSettingDlg::OnBnClickedLyricBackgroundCheck)
    ON_BN_CLICKED(IDC_DARK_MODE_CHECK, &CAppearanceSettingDlg::OnBnClickedDarkModeCheck)
    ON_BN_CLICKED(IDC_USE_INNER_IMAGE_FIRST_CHECK, &CAppearanceSettingDlg::OnBnClickedUseInnerImageFirstCheck)
    ON_BN_CLICKED(IDC_ENABLE_BACKGROUND_CHECK, &CAppearanceSettingDlg::OnBnClickedEnableBackgroundCheck)
    ON_BN_CLICKED(IDC_LOW_FREQ_IN_CENTER_CHECK, &CAppearanceSettingDlg::OnBnClickedLowFreqInCenterCheck)
    ON_BN_CLICKED(IDC_DEFAULT_ALBUM_COVER_HQ, &CAppearanceSettingDlg::OnBnClickedDefaultAlbumCoverHq)
    ON_BN_CLICKED(IDC_RESTORE_DEFAULT_BUTTON, &CAppearanceSettingDlg::OnBnClickedRestoreDefaultButton)
    ON_CBN_SELCHANGE(IDC_COMBO1, &CAppearanceSettingDlg::OnCbnSelchangeCombo1)
    ON_WM_PAINT()
    ON_BN_CLICKED(IDC_NOTIFY_ICON_AUTO_ADAPT_CHECK, &CAppearanceSettingDlg::OnBnClickedNotifyIconAutoAdaptCheck)
    ON_BN_CLICKED(IDC_BTN_ROUND_CORNERS_CHECK, &CAppearanceSettingDlg::OnBnClickedBtnRoundCornersCheck)
    ON_MESSAGE(WM_EDIT_BROWSE_CHANGED, &CAppearanceSettingDlg::OnEditBrowseChanged)
    ON_BN_CLICKED(IDC_USE_DESKTOP_BACKGROUND_CHECK, &CAppearanceSettingDlg::OnBnClickedUseDesktopBackgroundCheck)
    ON_BN_CLICKED(IDC_SHOW_NEXT_CHECK, &CAppearanceSettingDlg::OnBnClickedShowNextCheck)
    ON_BN_CLICKED(IDC_SHOW_FPS_CHECK, &CAppearanceSettingDlg::OnBnClickedShowFpsCheck)
    ON_BN_CLICKED(IDC_ALWAYS_SHOW_STATUSBAR_CHECK, &CAppearanceSettingDlg::OnBnClickedAlwaysShowStatusbarCheck)
    ON_BN_CLICKED(IDC_USE_SYSTEM_TITLEBAR_RADIO, &CAppearanceSettingDlg::OnBnClickedUseSystemTitlebarRadio)
    ON_BN_CLICKED(IDC_USE_OWNER_DRAW_TITLEBAR_RADIO, &CAppearanceSettingDlg::OnBnClickedUseOwnerDrawTitlebarRadio)
END_MESSAGE_MAP()


// CAppearanceSettingDlg 消息处理程序


BOOL CAppearanceSettingDlg::OnInitDialog()
{
    CTabDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

    m_transparency_slid.SetRange(20, 100);
    m_transparency_slid.SetPos(m_data.window_transparency);
    CString str;
    str.Format(_T("%d%%"), m_data.window_transparency);
    SetDlgItemText(IDC_TRANSPARENT_STATIC, str);

    m_spectrum_height_slid.SetRange(0, 100);
    m_spectrum_height_slid.SetPos(SpectrumHeightChg(theApp.m_app_setting_data.sprctrum_height));
    str.Format(_T("%d%%"), theApp.m_app_setting_data.sprctrum_height);
    SetDlgItemText(IDC_SPECTRUM_HEIGHT_STATIC, str);

    //为控件设置SS_NOTIFY属性，以允许响应消息
    DWORD dwStyle = m_color_static.GetStyle();
    ::SetWindowLong(m_color_static.GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);
    ::SetWindowLong(m_color_static1.GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);
    ::SetWindowLong(m_color_static2.GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);
    ::SetWindowLong(m_color_static3.GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);
    ::SetWindowLong(m_color_static4.GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);
    ::SetWindowLong(m_color_static5.GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);
    ::SetWindowLong(m_color_static6.GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);
    ::SetWindowLong(m_color_static7.GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);

    m_toolTip.Create(this);
    m_toolTip.SetMaxTipWidth(theApp.DPI(300));
    m_toolTip.AddTool(&m_color_static, theApp.m_str_table.LoadText(L"TIP_OPT_APC_COLOR_CURRENT").c_str());
    m_toolTip.AddTool(&m_color_static1, theApp.m_str_table.LoadText(L"TIP_OPT_APC_COLOR_LIGNT_BLUE").c_str());
    m_toolTip.AddTool(&m_color_static2, theApp.m_str_table.LoadText(L"TIP_OPT_APC_COLOR_GREEN").c_str());
    m_toolTip.AddTool(&m_color_static3, theApp.m_str_table.LoadText(L"TIP_OPT_APC_COLOR_ORANGE").c_str());
    m_toolTip.AddTool(&m_color_static4, theApp.m_str_table.LoadText(L"TIP_OPT_APC_COLOR_CYAN_GREEN").c_str());
    m_toolTip.AddTool(&m_color_static5, theApp.m_str_table.LoadText(L"TIP_OPT_APC_COLOR_PINK").c_str());
    m_toolTip.AddTool(&m_color_static6, theApp.m_str_table.LoadText(L"TIP_OPT_APC_COLOR_LIGHT_PURPLE").c_str());
    m_toolTip.AddTool(&m_color_static7, theApp.m_str_table.LoadText(L"TIP_OPT_APC_COLOR_GRAY").c_str());

    m_toolTip.SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

    DrawColor();

    ////设置“更多颜色”按钮的可用状态
    //GetDlgItem(IDC_SET_PROGRESS_COLOR_BUTTON)->EnableWindow(!m_data.theme_color_follow_system);
    //根据是否跟随系统设置复选按钮的初始状态
#ifdef COMPILE_IN_WIN_XP
    m_follow_system_color_check.EnableWindow(FALSE);
#else
    //if (CWinVersionHelper::IsWindows8OrLater())
    m_follow_system_color_check.SetCheck(m_data.theme_color_follow_system);
    //else
    //	m_follow_system_color_check.EnableWindow(FALSE);		//Win8以下系统禁用此复选按钮
#endif // !COMPILE_IN_WIN_XP

    //
    m_show_album_cover_chk.SetCheck(m_data.show_album_cover);
    m_album_cover_fit_combo.AddString(theApp.m_str_table.LoadText(L"TXT_OPT_APC_COVER_FIT_STRETCH").c_str());
    m_album_cover_fit_combo.AddString(theApp.m_str_table.LoadText(L"TXT_OPT_APC_COVER_FIT_FILL").c_str());
    m_album_cover_fit_combo.AddString(theApp.m_str_table.LoadText(L"TXT_OPT_APC_COVER_FIT_ADAPT").c_str());
    m_album_cover_fit_combo.SetCurSel(static_cast<int>(m_data.album_cover_fit));
    m_toolTip.AddTool(&m_album_cover_fit_combo, theApp.m_str_table.LoadText(L"TIP_OPT_APC_COVER_FIT").c_str());
    m_toolTip.AddTool(&m_use_out_image_chk, theApp.m_str_table.LoadText(L"TIP_OPT_APC_COVER_OUT_IMAGE_ALLOW").c_str());
    m_toolTip.AddTool(&m_album_cover_name_edit, theApp.m_str_table.LoadText(L"TIP_OPT_APC_COVER_OUT_IMAGE_FILE_NAME").c_str());
    m_toolTip.AddTool(&m_album_cover_path_edit, theApp.m_str_table.LoadText(L"TIP_OPT_APC_COVER_OUT_FOLDER").c_str());

    m_album_cover_name_edit.SetWindowTextW(CCommon::StringMerge(m_data.default_album_name, L',').c_str());
    m_album_cover_name_edit.SetEditBrowseMode(CBrowseEdit::EditBrowseMode::LIST);
    m_album_cover_name_edit.SetPopupDlgTitle(theApp.m_str_table.LoadText(L"TITLE_BROWSE_EDIT_SET_MULTI_OUT_ALBUM_COVER_FILE_NAME"));
    m_album_cover_path_edit.SetWindowTextW(m_data.album_cover_path.c_str());

    m_enable_background_chk.SetCheck(m_data.enable_background);
    m_album_cover_as_background_chk.SetCheck(m_data.album_cover_as_background);
    m_show_spectrum_chk.SetCheck(m_data.show_spectrum);

    m_back_transparency_slid.SetRange(10, 98);
    m_back_transparency_slid.SetPos(theApp.m_app_setting_data.background_transparency);
    str.Format(_T("%d%%"), theApp.m_app_setting_data.background_transparency);
    SetDlgItemText(IDC_BACKGROUND_TRANSPARENCY_STATIC, str);

    m_use_out_image_chk.SetCheck(m_data.use_out_image);

    m_background_gauss_blur_chk.SetCheck(m_data.background_gauss_blur);
    m_gauss_blur_radius_sld.SetRange(10, 300);
    m_gauss_blur_radius_sld.SetPos(m_data.gauss_blur_radius);
    str.Format(_T("%.1f"), static_cast<float>(m_data.gauss_blur_radius) / 10);
    SetDlgItemText(IDC_GAUSS_BLUR_RADIUS_STATIC, str);
    m_lyric_background_chk.SetCheck(m_data.lyric_background);
    m_dark_mode_chk.SetCheck(m_data.dark_mode);
    m_use_inner_image_first_chk.SetCheck(m_data.use_inner_image_first);
    m_low_freq_in_center_chk.SetCheck(m_data.spectrum_low_freq_in_center);

    m_default_background_edit.SetWindowText(m_data.default_background.c_str());
    wstring img_fliter = FilterHelper::GetImageFileFilter();
    m_default_background_edit.EnableFileBrowseButton(NULL, img_fliter.c_str());
    CheckDlgButton(IDC_USE_DESKTOP_BACKGROUND_CHECK, m_data.use_desktop_background);

    m_default_cover_hq_chk.SetCheck(m_data.draw_album_high_quality);

    m_ui_refresh_interval_edit.SetRange(MIN_UI_INTERVAL, MAX_UI_INTERVAL, UI_INTERVAL_STEP);
    m_ui_refresh_interval_edit.SetValue(m_data.ui_refresh_interval);

    m_icon_select_combo.AddString(theApp.m_str_table.LoadText(L"TXT_OPT_APC_NOTIFY_ICON_DEFAULT").c_str());
    m_icon_select_combo.AddString(theApp.m_str_table.LoadText(L"TXT_OPT_APC_NOTIFY_ICON_LIGHT").c_str());
    m_icon_select_combo.AddString(theApp.m_str_table.LoadText(L"TXT_OPT_APC_NOTIFY_ICON_DARK").c_str());
    m_icon_select_combo.SetCurSel(m_data.notify_icon_selected);

    m_notify_icon_auto_adapt_chk.SetCheck(m_data.notify_icon_auto_adapt);
    m_btn_round_corners_chk.SetCheck(m_data.button_round_corners);

    CheckDlgButton(IDC_ALWAYS_SHOW_STATUSBAR_CHECK, m_data.always_show_statusbar);
    CheckDlgButton(IDC_SHOW_NEXT_CHECK, m_data.show_next_track);
    CheckDlgButton(IDC_SHOW_FPS_CHECK, m_data.show_fps);

    if (m_data.show_window_frame)
        CheckDlgButton(IDC_USE_SYSTEM_TITLEBAR_RADIO, TRUE);
    else
        CheckDlgButton(IDC_USE_OWNER_DRAW_TITLEBAR_RADIO, TRUE);
    CheckDlgButton(IDC_SHOW_SETTINGS_BTN_CHECK, m_data.show_settings_btn_in_titlebar);
    CheckDlgButton(IDC_SHOW_SKIN_BTN_CHECK, m_data.show_skin_btn_in_titlebar);
    CheckDlgButton(IDC_SHOW_MINI_MODE_BTN_CHECK, m_data.show_minimode_btn_in_titlebar);
    CheckDlgButton(IDC_SHOW_FULLSCREEN_BTN_CHECK, m_data.show_fullscreen_btn_in_titlebar);
    CheckDlgButton(IDC_SHOW_MINIMIZE_BTN_CHECK, m_data.show_minimize_btn_in_titlebar);
    CheckDlgButton(IDC_SHOW_MAXIMIZE_BTN_CHECK, m_data.show_maximize_btn_in_titlebar);
    CheckDlgButton(IDC_SHOW_DARK_LIGHT_BTN_CHECK, m_data.show_dark_light_btn_in_titlebar);

    //设置通知区图标预览区域的位置
    CalculatePreviewBitmapRect();

    //载入预览图
    m_preview_dark.LoadFromResource(AfxGetResourceHandle(), IDB_NOTIFY_ICON_PREVIEW);
    m_preview_light.LoadFromResource(AfxGetResourceHandle(), IDB_NOTIFY_ICON_PREVIEW_LIGHT);
    m_preview_system_titlebar.LoadFromResource(AfxGetResourceHandle(), IDB_SYSTEM_TITLEBAR_PREVIEW);
    m_preview_owner_draw_titlebar.LoadFromResource(AfxGetResourceHandle(), IDB_OWNER_DRAW_TITLEBAR_PREVIEW);

    SetControlEnable();

    //设置控件不响应鼠标滚轮消息
    m_spectrum_height_slid.SetMouseWheelEnable(false);
    m_transparency_slid.SetMouseWheelEnable(false);
    m_album_cover_fit_combo.SetMouseWheelEnable(false);
    m_back_transparency_slid.SetMouseWheelEnable(false);
    m_gauss_blur_radius_sld.SetMouseWheelEnable(false);
    m_ui_refresh_interval_edit.SetMouseWheelEnable(false);
    m_icon_select_combo.SetMouseWheelEnable(false);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


//void CAppearanceSettingDlg::OnNMReleasedcaptureTransparentSlider(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	// TODO: 在此添加控件通知处理程序代码
//	m_data.window_transparency = m_transparency_slid.GetPos();
//	CString str;
//	str.Format(_T("不透明度：%%%d"), m_data.window_transparency);
//	SetDlgItemText(IDC_TRANSPARENT_STATIC, str);
//	*pResult = 0;
//}


void CAppearanceSettingDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    // TODO: 在此添加消息处理程序代码和/
    //滑动水平滑动条时响应此函数
    if ((pScrollBar->GetDlgCtrlID() == IDC_TRANSPARENT_SLIDER))
    {
        m_data.window_transparency = m_transparency_slid.GetPos();
        CString str;
        str.Format(_T("%d%%"), m_data.window_transparency);
        SetDlgItemText(IDC_TRANSPARENT_STATIC, str);

        SetTransparency();		//实时设置窗口不透明度
    }
    if ((pScrollBar->GetDlgCtrlID() == IDC_SPECTRUM_HEIGHT_SLIDER))
    {
        theApp.m_app_setting_data.sprctrum_height = SpectrumHeightRChg(m_spectrum_height_slid.GetPos());
        m_data.sprctrum_height = theApp.m_app_setting_data.sprctrum_height;
        CString str;
        str.Format(_T("%d%%"), theApp.m_app_setting_data.sprctrum_height);
        SetDlgItemText(IDC_SPECTRUM_HEIGHT_STATIC, str);
    }
    if ((pScrollBar->GetDlgCtrlID() == IDC_BACKGROUND_TRANSPARENCY_SLIDER))
    {
        theApp.m_app_setting_data.background_transparency = m_back_transparency_slid.GetPos();
        m_data.background_transparency = theApp.m_app_setting_data.background_transparency;
        CString str;
        str.Format(_T("%d%%"), theApp.m_app_setting_data.background_transparency);
        SetDlgItemText(IDC_BACKGROUND_TRANSPARENCY_STATIC, str);
    }
    if ((pScrollBar->GetDlgCtrlID() == IDC_GAUSS_BLURE_RADIUS_SLIDER))
    {
        m_data.gauss_blur_radius = m_gauss_blur_radius_sld.GetPos();
        CString str;
        str.Format(_T("%.1f"), static_cast<float>(m_data.gauss_blur_radius) / 10);
        SetDlgItemText(IDC_GAUSS_BLUR_RADIUS_STATIC, str);
    }

    CTabDlg::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CAppearanceSettingDlg::OnBnClickedSetThemeButton()
{
    // TODO: 在此添加控件通知处理程序代码
    CColorDialog colorDlg(m_data.theme_color.original_color, 0, this);
    if (colorDlg.DoModal() == IDOK)
    {
        m_data.theme_color.original_color = colorDlg.GetColor();
        //if (m_data.theme_color.original_color == 0)
        //	MessageBox(_T("警告：将主题颜色设置成黑色会使播放列表中正在播放的项目看不见！"), NULL, MB_ICONWARNING);
        //if(m_data.theme_color.original_color == RGB(255,255,255))
        //	MessageBox(WHITE_THEME_COLOR_WARNING), NULL, MB_ICONWARNING);
        m_color_static.SetFillColor(m_data.theme_color.original_color);
        //设置了“更多颜色”之后，取消“跟随系统主题色”复选按钮的选中
        m_data.theme_color_follow_system = false;
        m_follow_system_color_check.SetCheck(FALSE);
    }
}


void CAppearanceSettingDlg::OnStnClickedColorStatic2()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.theme_color.original_color = m_color1;
    ClickColor();
}


void CAppearanceSettingDlg::OnStnClickedColorStatic3()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.theme_color.original_color = m_color2;
    ClickColor();
}


void CAppearanceSettingDlg::OnStnClickedColorStatic4()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.theme_color.original_color = m_color3;
    ClickColor();
}


void CAppearanceSettingDlg::OnStnClickedColorStatic5()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.theme_color.original_color = m_color4;
    ClickColor();
}


void CAppearanceSettingDlg::OnStnClickedColorStatic6()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.theme_color.original_color = m_color5;
    ClickColor();
}


void CAppearanceSettingDlg::OnStnClickedColorStatic7()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.theme_color.original_color = m_color6;
    ClickColor();
}

void CAppearanceSettingDlg::OnStnClickedColorStatic8()
{
    m_data.theme_color.original_color = m_color7;
    ClickColor();
}


BOOL CAppearanceSettingDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if (pMsg->message == WM_MOUSEMOVE)
        m_toolTip.RelayEvent(pMsg);

    return CTabDlg::PreTranslateMessage(pMsg);
}


void CAppearanceSettingDlg::OnBnClickedFollowSystemColorCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.theme_color_follow_system = (m_follow_system_color_check.GetCheck() != 0);
    //GetDlgItem(IDC_SET_PROGRESS_COLOR_BUTTON)->EnableWindow(!m_data.theme_color_follow_system);
}


void CAppearanceSettingDlg::OnBnClickedShowAlbumCoverCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.show_album_cover = (m_show_album_cover_chk.GetCheck() != 0);
    SetControlEnable();
}


void CAppearanceSettingDlg::OnCbnSelchangeAlbumFitCombo()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.album_cover_fit = static_cast<CDrawCommon::StretchMode>(m_album_cover_fit_combo.GetCurSel());
}


void CAppearanceSettingDlg::OnBnClickedAlbumCoverBackgroundCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.album_cover_as_background = (m_album_cover_as_background_chk.GetCheck() != 0);
    SetControlEnable();
}


void CAppearanceSettingDlg::OnBnClickedShowSpectrumCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.show_spectrum = (m_show_spectrum_chk.GetCheck() != 0);
    SetControlEnable();
}


void CAppearanceSettingDlg::OnBnClickedUseOutImageCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.use_out_image = (m_use_out_image_chk.GetCheck() != 0);
    SetControlEnable();
}


void CAppearanceSettingDlg::OnBnClickedBackgroundGaussBlurCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.background_gauss_blur = (m_background_gauss_blur_chk.GetCheck() != 0);
    SetControlEnable();
}


void CAppearanceSettingDlg::OnBnClickedLyricBackgroundCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.lyric_background = (m_lyric_background_chk.GetCheck() != 0);
}


void CAppearanceSettingDlg::OnBnClickedDarkModeCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.dark_mode = (m_dark_mode_chk.GetCheck() != 0);
}


void CAppearanceSettingDlg::OnBnClickedUseInnerImageFirstCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.use_inner_image_first = (m_use_inner_image_first_chk.GetCheck() != 0);
}


void CAppearanceSettingDlg::OnBnClickedEnableBackgroundCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.enable_background = (m_enable_background_chk.GetCheck() != 0);
    SetControlEnable();
}


void CAppearanceSettingDlg::OnBnClickedLowFreqInCenterCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.spectrum_low_freq_in_center = (m_low_freq_in_center_chk.GetCheck() != 0);
}


void CAppearanceSettingDlg::OnBnClickedDefaultAlbumCoverHq()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.draw_album_high_quality = (m_default_cover_hq_chk.GetCheck() != 0);
}


void CAppearanceSettingDlg::OnBnClickedRestoreDefaultButton()
{
    // TODO: 在此添加控件通知处理程序代码
    m_ui_refresh_interval_edit.SetValue(UI_INTERVAL_DEFAULT);
}


void CAppearanceSettingDlg::OnCbnSelchangeCombo1()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.notify_icon_selected = m_icon_select_combo.GetCurSel();
    InvalidateRect(m_notify_icon_preview, FALSE);
}


void CAppearanceSettingDlg::OnPaint()
{
    CPaintDC dc(this); // device context for painting
                       // TODO: 在此处添加消息处理程序代码
                       // 不为绘图消息调用 CTabDlg::OnPaint()

    CalculatePreviewBitmapRect();
    CDrawCommon drawer;
    drawer.Create(&dc);
    
    //绘制通知区图标预览图
    CImage& image{ m_data.notify_icon_selected == 2 ? m_preview_light : m_preview_dark };
    //绘制背景
    drawer.DrawImage(image, m_notify_icon_preview.TopLeft(), m_notify_icon_preview.Size(), CDrawCommon::StretchMode::STRETCH);
    //绘制图标
    if (m_data.notify_icon_selected >= 0 && m_data.notify_icon_selected < MAX_NOTIFY_ICON)
    {
        drawer.DrawIcon(theApp.GetNotifyIncon(m_data.notify_icon_selected),
            CPoint(m_notify_icon_preview.left + ICON_X, m_notify_icon_preview.top + ICON_Y),
            CSize(theApp.DPI(16), theApp.DPI(16)));
    }

    //绘制标题栏预览图
    drawer.DrawImage(m_preview_system_titlebar, m_system_titlebar_preview_rect.TopLeft(), m_system_titlebar_preview_rect.Size(), CDrawCommon::StretchMode::FIT);
    drawer.DrawImage(m_preview_owner_draw_titlebar, m_owner_draw_titlebar_preview_rect.TopLeft(), m_owner_draw_titlebar_preview_rect.Size(), CDrawCommon::StretchMode::FIT);
}


void CAppearanceSettingDlg::OnBnClickedNotifyIconAutoAdaptCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.notify_icon_auto_adapt = (m_notify_icon_auto_adapt_chk.GetCheck() != 0);
}


void CAppearanceSettingDlg::OnBnClickedBtnRoundCornersCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.button_round_corners = (m_btn_round_corners_chk.GetCheck() != 0);
}


afx_msg LRESULT CAppearanceSettingDlg::OnEditBrowseChanged(WPARAM wParam, LPARAM lParam)
{
    CBrowseEdit* pEdit = (CBrowseEdit*)lParam;
    if (pEdit == &m_default_background_edit)
    {
        CString str;
        m_default_background_edit.GetWindowText(str);
        m_data.default_background = str.GetString();
    }
    else if (pEdit == &m_album_cover_name_edit)
    {
        CString temp;
        m_album_cover_name_edit.GetWindowTextW(temp);
        temp.Replace(L'/', L'\\');
        CCommon::StringSplit(wstring(temp), L',', m_data.default_album_name);
        for (auto& album_name : m_data.default_album_name)
        {
            // 虽然开头的.\\不是后续识别必须的但是如果发现缺少还是加上
            // 这是一个非简写的相对路径，绝对路径此处不做处理
            if (album_name.find(L'\\') != wstring::npos && album_name.front() != L'\\' && !CCommon::IsPath(album_name))
            {
                // 确保相对路径以".\\"开头
                if (album_name.at(0) == L'.' && album_name.at(1) == L'\\')
                    continue;
                album_name = L".\\" + album_name;
            }
        }
        m_album_cover_name_edit.SetWindowTextW(CCommon::StringMerge(m_data.default_album_name, L',').c_str());
    }
    else if (pEdit == &m_album_cover_path_edit)
    {
        CString str;
        m_album_cover_path_edit.GetWindowTextW(str);
        m_data.album_cover_path = str.GetString();
    }
    return 0;
}


void CAppearanceSettingDlg::OnBnClickedUseDesktopBackgroundCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.use_desktop_background = (IsDlgButtonChecked(IDC_USE_DESKTOP_BACKGROUND_CHECK) != 0);
    SetControlEnable();
}


void CAppearanceSettingDlg::OnBnClickedShowNextCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.show_next_track = (IsDlgButtonChecked(IDC_SHOW_NEXT_CHECK) != 0);
}


void CAppearanceSettingDlg::OnBnClickedShowFpsCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.show_fps = (IsDlgButtonChecked(IDC_SHOW_FPS_CHECK) != 0);
}


void CAppearanceSettingDlg::OnBnClickedAlwaysShowStatusbarCheck()
{
    m_data.always_show_statusbar = (IsDlgButtonChecked(IDC_ALWAYS_SHOW_STATUSBAR_CHECK) != 0);
    SetControlEnable();
}


void CAppearanceSettingDlg::OnBnClickedUseSystemTitlebarRadio()
{
    m_data.show_window_frame = true;
    SetControlEnable();
}


void CAppearanceSettingDlg::OnBnClickedUseOwnerDrawTitlebarRadio()
{
    m_data.show_window_frame = false;
    SetControlEnable();
}
