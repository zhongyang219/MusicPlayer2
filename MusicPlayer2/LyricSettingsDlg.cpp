// PlaySettingsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "LyricSettingsDlg.h"
#include "afxdialogex.h"
#include "CortanaLyric.h"


// CLyricSettingsDlg 对话框

IMPLEMENT_DYNAMIC(CLyricSettingsDlg, CTabDlg)

CLyricSettingsDlg::CLyricSettingsDlg(CWnd* pParent /*=NULL*/)
    : CTabDlg(IDD_LYRIC_SETTING_DIALOG, pParent)
{

}

CLyricSettingsDlg::~CLyricSettingsDlg()
{
}

void CLyricSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
    CTabDlg::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_KARAOKE_DISP, m_karaoke_disp_check);
    DDX_Control(pDX, IDC_LYRIC_FUZZY_MATCH, m_lyric_fuzzy_match_check);
    DDX_Control(pDX, IDC_SHOW_LYRIC_IN_CORTANA, m_cortana_info_enable_check);
    DDX_Control(pDX, IDC_LYRIC_DOUBLE_LINE_CHECK, m_lyric_double_line_chk);
    DDX_Control(pDX, IDC_SHOW_ALBUM_COVER_IN_CORTANA, m_show_album_cover_in_cortana_check);
    DDX_Control(pDX, IDC_CORTANA_ICON_DEAT_CHECK, m_cortana_icon_beat_check);
    DDX_Control(pDX, IDC_CORTANA_COLOR_COMBO, m_cortana_color_combo);
    DDX_Control(pDX, IDC_LYRIC_COMPATIBLE_MODE, m_lyric_compatible_mode_chk);
    DDX_Control(pDX, IDC_KEEP_DISPLAY_CHECK, m_keep_display_chk);
    DDX_Control(pDX, IDC_SHOW_SPECTRUM_IN_CORTANA, m_show_spectrum_chk);
    DDX_Control(pDX, IDC_SHOW_LYRIC_IN_CORTANA2, m_show_lyric_in_cortana_chk);
    //DDX_Control(pDX, IDC_SEARCH_BOX_OPAQUE_CHECK, m_search_box_opaque_chk);
    DDX_Control(pDX, IDC_SHOW_DESKTOP_LYRIC, m_show_desktop_lyric_chk);
    DDX_Control(pDX, IDC_LYRIC_DOUBLE_LINE_CHECK2, m_desktop_lyric_double_line_chk);
    DDX_Control(pDX, IDC_TEXT_COLOR1_STATIC, m_text_color1_static);
    DDX_Control(pDX, IDC_TEXT_COLOR2_STATIC, m_text_color2_static);
    DDX_Control(pDX, IDC_TEXT_GRADIENT_COMBO, m_text_gradient_combo);
    DDX_Control(pDX, IDC_HIGHLIGHT_COLOR1_STATIC, m_highlight_color1_static);
    DDX_Control(pDX, IDC_HIGHLIGHT_COLOR2_STATIC, m_highlight_color2_static);
    DDX_Control(pDX, IDC_HIGHLIGHT_GRADIENT_COMBO, m_highlight_gradient_combo);
    DDX_Control(pDX, IDC_LYRIC_OPACITY_SLIDER, m_desktop_lyric_opacity_sld);
    DDX_Control(pDX, IDC_LOCK_DESKTOP_LYRIC_CHECK, m_lock_desktop_lyric_chk);
    DDX_Control(pDX, IDC_HIDE_LYRIC_WITHOUT_LYRIC_CHECK, m_hide_lyric_without_lyric_chk);
    DDX_Control(pDX, IDC_HIDE_LYRIC_PAUSE_CHECK, m_hide_lyric_paused_chk);
    DDX_Control(pDX, IDC_HIDE_LYRIC_WITHOUT_LYRIC_CHECK3, m_lyric_background_penetrate_chk);
    DDX_Control(pDX, IDC_LYRIC_PATH_EDIT, m_lyric_dir_edit);
    DDX_Control(pDX, IDC_ALIGNMENT_COMBO, m_alignment_combo);
    DDX_Control(pDX, IDC_LYRIC_SAVE_COMBO, m_lyric_save_policy_combo);
    DDX_Control(pDX, IDC_USE_INNER_LYRIC_CHECK, m_use_inner_lyric_chk);
    DDX_Control(pDX, IDC_SHOW_UNLOCK_WHEN_LOCKED, m_show_unlock_when_locked_chk);
    DDX_Control(pDX, IDC_SHOW_DEFAULT_ALBUM_ICON_CHK, m_show_default_album_icon_chk);
    DDX_Control(pDX, IDC_FONT_SIZE_EDIT2, m_lyric_line_space_edit);
    DDX_Control(pDX, IDC_ALIGNMENT_COMBO2, m_lyric_alignment_combo);
    DDX_Control(pDX, IDC_DESKTOP_LYRIC_ALIGNMENT_COMBO, m_desktop_lyric_alignment_combo);
}


BEGIN_MESSAGE_MAP(CLyricSettingsDlg, CTabDlg)
    ON_BN_CLICKED(IDC_KARAOKE_DISP, &CLyricSettingsDlg::OnBnClickedKaraokeDisp)
    //ON_BN_CLICKED(IDC_EXPLORE_LYRIC_BUTTON, &CLyricSettingsDlg::OnBnClickedExploreLyricButton)
    ON_BN_CLICKED(IDC_LYRIC_FUZZY_MATCH, &CLyricSettingsDlg::OnBnClickedLyricFuzzyMatch)
    ON_BN_CLICKED(IDC_SHOW_LYRIC_IN_CORTANA, &CLyricSettingsDlg::OnBnClickedShowLyricInCortana)
    ON_BN_CLICKED(IDC_LYRIC_DOUBLE_LINE_CHECK, &CLyricSettingsDlg::OnBnClickedLyricDoubleLineCheck)
    ON_CBN_SELCHANGE(IDC_CORTANA_COLOR_COMBO, &CLyricSettingsDlg::OnCbnSelchangeCortanaColorCombo)
    ON_BN_CLICKED(IDC_SHOW_ALBUM_COVER_IN_CORTANA, &CLyricSettingsDlg::OnBnClickedShowAlbumCoverInCortana)
    ON_BN_CLICKED(IDC_CORTANA_ICON_DEAT_CHECK, &CLyricSettingsDlg::OnBnClickedCortanaIconDeatCheck)
    ON_BN_CLICKED(IDC_LYRIC_COMPATIBLE_MODE, &CLyricSettingsDlg::OnBnClickedLyricCompatibleMode)
    ON_BN_CLICKED(IDC_SET_FONT, &CLyricSettingsDlg::OnBnClickedSetFont)
    ON_BN_CLICKED(IDC_KEEP_DISPLAY_CHECK, &CLyricSettingsDlg::OnBnClickedKeepDisplayCheck)
    ON_BN_CLICKED(IDC_SHOW_SPECTRUM_IN_CORTANA, &CLyricSettingsDlg::OnBnClickedShowSpectrumInCortana)
    ON_BN_CLICKED(IDC_SHOW_LYRIC_IN_CORTANA2, &CLyricSettingsDlg::OnBnClickedShowLyricInCortana2)
    //ON_BN_CLICKED(IDC_SEARCH_BOX_OPAQUE_CHECK, &CLyricSettingsDlg::OnBnClickedSearchBoxOpaqueCheck)
    ON_BN_CLICKED(IDC_SHOW_DESKTOP_LYRIC, &CLyricSettingsDlg::OnBnClickedShowDesktopLyric)
    ON_BN_CLICKED(IDC_SET_FONT2, &CLyricSettingsDlg::OnBnClickedSetFont2)
    ON_CBN_SELCHANGE(IDC_TEXT_GRADIENT_COMBO, &CLyricSettingsDlg::OnCbnSelchangeTextGradientCombo)
    ON_CBN_SELCHANGE(IDC_HIGHLIGHT_GRADIENT_COMBO, &CLyricSettingsDlg::OnCbnSelchangeHighlightGradientCombo)
    ON_BN_CLICKED(IDC_LOCK_DESKTOP_LYRIC_CHECK, &CLyricSettingsDlg::OnBnClickedLockDesktopLyricCheck)
    ON_WM_CTLCOLOR()
    ON_WM_HSCROLL()
    ON_BN_CLICKED(IDC_HIDE_LYRIC_WITHOUT_LYRIC_CHECK, &CLyricSettingsDlg::OnBnClickedHideLyricWithoutLyricCheck)
    ON_BN_CLICKED(IDC_HIDE_LYRIC_PAUSE_CHECK, &CLyricSettingsDlg::OnBnClickedHideLyricPauseCheck)
    ON_BN_CLICKED(IDC_LYRIC_DOUBLE_LINE_CHECK2, &CLyricSettingsDlg::OnBnClickedLyricDoubleLineCheck2)
    ON_BN_CLICKED(IDC_DEFAULT_STYLE, &CLyricSettingsDlg::OnBnClickedDefaultStyle)
    ON_COMMAND(ID_LYRIC_DEFAULT_STYLE1, &CLyricSettingsDlg::OnLyricDefaultStyle1)
    ON_COMMAND(ID_LYRIC_DEFAULT_STYLE2, &CLyricSettingsDlg::OnLyricDefaultStyle2)
    ON_COMMAND(ID_LYRIC_DEFAULT_STYLE3, &CLyricSettingsDlg::OnLyricDefaultStyle3)
    ON_BN_CLICKED(IDC_HIDE_LYRIC_WITHOUT_LYRIC_CHECK3, &CLyricSettingsDlg::OnBnClickedHideLyricWithoutLyricCheck3)
    ON_MESSAGE(WM_COLOR_SELECTED, &CLyricSettingsDlg::OnColorSelected)
    ON_COMMAND(ID_LYRIC_DEFAULT_STYLE1_MODIFY, &CLyricSettingsDlg::OnLyricDefaultStyle1Modify)
    ON_COMMAND(ID_LYRIC_DEFAULT_STYLE2_MODIFY, &CLyricSettingsDlg::OnLyricDefaultStyle2Modify)
    ON_COMMAND(ID_LYRIC_DEFAULT_STYLE3_MODIFY, &CLyricSettingsDlg::OnLyricDefaultStyle3Modify)
    ON_COMMAND(ID_RESTORE_DEFAULT_STYLE, &CLyricSettingsDlg::OnRestoreDefaultStyle)
    //ON_EN_CHANGE(IDC_LYRIC_PATH_EDIT, &CLyricSettingsDlg::OnEnChangeLyricPathEdit)
    ON_MESSAGE(WM_EDIT_BROWSE_CHANGED, &CLyricSettingsDlg::OnEditBrowseChanged)
    ON_CBN_SELCHANGE(IDC_ALIGNMENT_COMBO, &CLyricSettingsDlg::OnCbnSelchangeAlignmentCombo)
    ON_CBN_SELCHANGE(IDC_LYRIC_SAVE_COMBO, &CLyricSettingsDlg::OnCbnSelchangeLyricSaveCombo)
    ON_BN_CLICKED(IDC_USE_INNER_LYRIC_CHECK, &CLyricSettingsDlg::OnBnClickedUseInnerLyricCheck)
    ON_BN_CLICKED(IDC_SHOW_UNLOCK_WHEN_LOCKED, &CLyricSettingsDlg::OnBnClickedShowUnlockWhenLocked)
    ON_BN_CLICKED(IDC_SHOW_DEFAULT_ALBUM_ICON_CHK, &CLyricSettingsDlg::OnBnClickedShowDefaultAlbumIconChk)
    ON_BN_CLICKED(IDC_SEARCH_BOX_TRANSPARENT_IN_WHITE_MODE, &CLyricSettingsDlg::OnBnClickedSearchBoxTransparentInWhiteMode)
    ON_BN_CLICKED(IDC_SET_FONT_BUTTON, &CLyricSettingsDlg::OnBnClickedSetFontButton)
    ON_CBN_SELCHANGE(IDC_ALIGNMENT_COMBO2, &CLyricSettingsDlg::OnCbnSelchangeAlignmentCombo2)
    ON_CBN_SELCHANGE(IDC_DESKTOP_LYRIC_ALIGNMENT_COMBO, &CLyricSettingsDlg::OnCbnSelchangeDesktopLyricAlignmentCombo)
    ON_BN_CLICKED(IDC_SHOW_LYRIC_TRANSLATE_CHECK, &CLyricSettingsDlg::OnBnClickedShowLyricTranslateCheck)
END_MESSAGE_MAP()


// CLyricSettingsDlg 消息处理程序


BOOL CLyricSettingsDlg::OnInitDialog()
{
    CTabDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

    //初始化各控件的状态
    m_karaoke_disp_check.SetCheck(m_data.lyric_karaoke_disp);
    m_lyric_fuzzy_match_check.SetCheck(m_data.lyric_fuzzy_match);
    m_use_inner_lyric_chk.SetCheck(m_data.use_inner_lyric_first);
    m_lyric_save_policy_combo.AddString(CCommon::LoadText(IDS_DO_NOT_SAVE));
    m_lyric_save_policy_combo.AddString(CCommon::LoadText(IDS_AUTO_SAVE));
    m_lyric_save_policy_combo.AddString(CCommon::LoadText(IDS_INQUIRY));
    m_lyric_save_policy_combo.SetCurSel(static_cast<int>(m_data.lyric_save_policy));
    CheckDlgButton(IDC_SHOW_LYRIC_TRANSLATE_CHECK, m_data.show_translate);

    m_lyric_line_space_edit.SetRange(MIM_LINE_SPACE, MAX_LINE_SPACE);
    m_lyric_line_space_edit.SetValue(m_data.lyric_line_space);

    m_lyric_alignment_combo.AddString(CCommon::LoadText(IDS_ALIGN_LEFT));
    m_lyric_alignment_combo.AddString(CCommon::LoadText(IDS_ALIGN_RIGHT));
    m_lyric_alignment_combo.AddString(CCommon::LoadText(IDS_CENTER));
    m_lyric_alignment_combo.SetCurSel(static_cast<int>(m_data.lyric_align));

    m_lyric_double_line_chk.SetCheck(m_data.cortana_lyric_double_line);
    m_show_album_cover_in_cortana_check.SetCheck(m_data.cortana_show_album_cover);
    m_cortana_icon_beat_check.SetCheck(m_data.cortana_icon_beat);
    m_lyric_compatible_mode_chk.SetCheck(m_data.cortana_lyric_compatible_mode);
    m_keep_display_chk.SetCheck(m_data.cortana_lyric_keep_display);
    m_show_spectrum_chk.SetCheck(m_data.cortana_show_spectrum);
    m_show_lyric_in_cortana_chk.SetCheck(m_data.cortana_show_lyric);
    //m_search_box_opaque_chk.SetCheck(m_data.cortana_opaque);
    //m_search_box_opaque_chk.ShowWindow(SW_HIDE);		//此选项已经没有作用，把它隐藏起来
    m_show_default_album_icon_chk.SetCheck(m_data.show_default_album_icon_in_search_box);
    CheckDlgButton(IDC_SEARCH_BOX_TRANSPARENT_IN_WHITE_MODE, IsSearchBoxTransparentInWhiteTheme());

    m_show_desktop_lyric_chk.SetCheck(m_data.show_desktop_lyric);
    m_text_color1_static.SetFillColor(m_data.desktop_lyric_data.text_color1);
    m_text_color2_static.SetFillColor(m_data.desktop_lyric_data.text_color2);
    m_highlight_color1_static.SetFillColor(m_data.desktop_lyric_data.highlight_color1);
    m_highlight_color2_static.SetFillColor(m_data.desktop_lyric_data.highlight_color2);
    m_desktop_lyric_double_line_chk.SetCheck(m_data.desktop_lyric_data.lyric_double_line);
    m_lock_desktop_lyric_chk.SetCheck(m_data.desktop_lyric_data.lock_desktop_lyric);
    m_hide_lyric_without_lyric_chk.SetCheck(m_data.desktop_lyric_data.hide_lyric_window_without_lyric);
    m_hide_lyric_paused_chk.SetCheck(m_data.desktop_lyric_data.hide_lyric_window_when_paused);
    m_lyric_background_penetrate_chk.SetCheck(m_data.desktop_lyric_data.lyric_background_penetrate);
    m_show_unlock_when_locked_chk.SetCheck(m_data.desktop_lyric_data.show_unlock_when_locked);

    m_text_gradient_combo.AddString(CCommon::LoadText(IDS_NO_GRADIENT));
    m_text_gradient_combo.AddString(CCommon::LoadText(IDS_TOW_COLOR_GRADIENT));
    m_text_gradient_combo.AddString(CCommon::LoadText(IDS_THREE_COLOR_GRADIENT));
    m_text_gradient_combo.SetCurSel(m_data.desktop_lyric_data.text_gradient);
    m_highlight_gradient_combo.AddString(CCommon::LoadText(IDS_NO_GRADIENT));
    m_highlight_gradient_combo.AddString(CCommon::LoadText(IDS_TOW_COLOR_GRADIENT));
    m_highlight_gradient_combo.AddString(CCommon::LoadText(IDS_THREE_COLOR_GRADIENT));
    m_highlight_gradient_combo.SetCurSel(m_data.desktop_lyric_data.highlight_gradient);

    m_desktop_lyric_opacity_sld.SetRange(20, 100);
    m_desktop_lyric_opacity_sld.SetPos(m_data.desktop_lyric_data.opacity);

    m_desktop_lyric_alignment_combo.AddString(CCommon::LoadText(IDS_ALIGN_LEFT));
    m_desktop_lyric_alignment_combo.AddString(CCommon::LoadText(IDS_ALIGN_RIGHT));
    m_desktop_lyric_alignment_combo.AddString(CCommon::LoadText(IDS_CENTER));
    m_desktop_lyric_alignment_combo.SetCurSel(static_cast<int>(m_data.desktop_lyric_data.lyric_align));

    CString str;
    str.Format(_T("%d%%"), m_data.desktop_lyric_data.opacity);
    SetDlgItemText(IDC_LYRIC_OPACITY_STATIC, str);

    if (CWinVersionHelper::IsWindows10())    //搜索框显示播放信息只有Windows10可用
    {
        m_cortana_info_enable_check.SetCheck(m_data.cortana_info_enable);
    }
    else
    {
        m_cortana_info_enable_check.EnableWindow(FALSE);		//Win10以下或Win11系统禁用此复选按钮
        m_data.cortana_info_enable = false;
    }

    EnableControl();
    EnableControlForDesktopLyric();

    m_lyric_dir_edit.SetWindowText(m_data.lyric_path.c_str());
    m_lyric_dir_edit.EnableFolderBrowseButton();

    m_tool_tip.Create(this);
    m_tool_tip.SetMaxTipWidth(300);
    m_tool_tip.AddTool(&m_lyric_fuzzy_match_check, CCommon::LoadText(IDS_LYRIC_FUZZY_MATHC_TIP_INFO));
    m_tool_tip.AddTool(&m_lyric_dir_edit, CCommon::LoadText(IDS_LYRIC_PATH_TIP_INFO));
    m_tool_tip.AddTool(GetDlgItem(IDC_SHOW_LYRIC_IN_CORTANA), CCommon::LoadText(IDS_CORTANA_SHOW_LYRIC_TIP_INFO));
    //m_tool_tip.AddTool(&m_search_box_opaque_chk, CCommon::LoadText(IDS_SEARCH_BOX_OPAQUE_TIP));

    m_tool_tip.SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

    m_cortana_color_combo.AddString(CCommon::LoadText(IDS_FOLLOWING_SYSTEM));
    m_cortana_color_combo.AddString(CCommon::LoadText(IDS_DARK));
    m_cortana_color_combo.AddString(CCommon::LoadText(IDS_LIGHT));
    m_cortana_color_combo.SetCurSel(m_data.cortana_color);

    m_alignment_combo.AddString(CCommon::LoadText(IDS_ALIGN_LEFT));
    m_alignment_combo.AddString(CCommon::LoadText(IDS_ALIGN_RIGHT));
    m_alignment_combo.AddString(CCommon::LoadText(IDS_CENTER));
    m_alignment_combo.SetCurSel(static_cast<int>(m_data.cortana_lyric_align));

    //设置控件不响应鼠标滚轮消息
    m_cortana_color_combo.SetMouseWheelEnable(false);
    m_lyric_save_policy_combo.SetMouseWheelEnable(false);
    m_text_gradient_combo.SetMouseWheelEnable(false);
    m_highlight_gradient_combo.SetMouseWheelEnable(false);
    m_alignment_combo.SetMouseWheelEnable(false);
    m_desktop_lyric_opacity_sld.SetMouseWheelEnable(false);
    m_lyric_alignment_combo.SetMouseWheelEnable(false);
    m_lyric_line_space_edit.SetMouseWheelEnable(false);
    m_desktop_lyric_alignment_combo.SetMouseWheelEnable(false);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}

void CLyricSettingsDlg::EnableControl()
{
    bool enable = m_data.cortana_info_enable && !m_data.cortana_lyric_compatible_mode;
    m_lyric_double_line_chk.EnableWindow(enable && m_data.cortana_show_lyric);
    m_show_lyric_in_cortana_chk.EnableWindow(enable);
    m_show_album_cover_in_cortana_check.EnableWindow(enable);
    m_cortana_color_combo.EnableWindow(enable);
    GetDlgItem(IDC_SET_FONT)->EnableWindow(enable);
    m_keep_display_chk.EnableWindow(enable);
    m_show_spectrum_chk.EnableWindow(enable);
    m_lyric_compatible_mode_chk.EnableWindow(m_data.cortana_info_enable);
    //m_search_box_opaque_chk.EnableWindow(m_data.cortana_info_enable);
    m_alignment_combo.EnableWindow(enable && m_data.cortana_show_lyric/* && !m_data.cortana_lyric_double_line*/);

    m_cortana_icon_beat_check.EnableWindow(enable && !m_data.show_default_album_icon_in_search_box);
    m_show_default_album_icon_chk.EnableWindow(enable);

    EnableDlgCtrl(IDC_SEARCH_BOX_TRANSPARENT_IN_WHITE_MODE, enable);
}

void CLyricSettingsDlg::EnableControlForDesktopLyric()
{
    bool desktop_lyric_enable = m_data.show_desktop_lyric;
    m_desktop_lyric_double_line_chk.EnableWindow(desktop_lyric_enable);
    m_text_color1_static.EnableWindow(desktop_lyric_enable);
    m_text_color1_static.Invalidate();
    m_text_color2_static.EnableWindow(desktop_lyric_enable);
    m_text_color2_static.Invalidate();
    m_text_gradient_combo.EnableWindow(desktop_lyric_enable);
    m_highlight_color1_static.EnableWindow(desktop_lyric_enable);
    m_highlight_color1_static.Invalidate();
    m_highlight_color2_static.EnableWindow(desktop_lyric_enable);
    m_highlight_color2_static.Invalidate();
    m_highlight_gradient_combo.EnableWindow(desktop_lyric_enable);
    m_desktop_lyric_opacity_sld.EnableWindow(desktop_lyric_enable);
    m_lock_desktop_lyric_chk.EnableWindow(desktop_lyric_enable);
    m_hide_lyric_without_lyric_chk.EnableWindow(desktop_lyric_enable);
    m_hide_lyric_paused_chk.EnableWindow(desktop_lyric_enable);
    m_lyric_background_penetrate_chk.EnableWindow(desktop_lyric_enable);
    GetDlgItem(IDC_SET_FONT2)->EnableWindow(desktop_lyric_enable);
    GetDlgItem(IDC_DEFAULT_STYLE)->EnableWindow(desktop_lyric_enable);
    m_show_unlock_when_locked_chk.EnableWindow(desktop_lyric_enable);
}


void CLyricSettingsDlg::ApplyDefaultLyricStyle(const LyricStyleDefaultData& style)
{
    CDesktopLyric::LyricStyleDefaultDataToLyricSettingData(style, m_data.desktop_lyric_data);

    m_text_color1_static.SetFillColor(style.normal_style.color1);
    m_text_color2_static.SetFillColor(style.normal_style.color2);
    m_text_gradient_combo.SetCurSel(m_data.desktop_lyric_data.text_gradient);
    m_highlight_color1_static.SetFillColor(style.highlight_style.color1);
    m_highlight_color2_static.SetFillColor(style.highlight_style.color2);
    m_highlight_gradient_combo.SetCurSel(m_data.desktop_lyric_data.highlight_gradient);
}

void CLyricSettingsDlg::SetSearchBoxTransparentInWhiteTheme(bool transparent)
{
    if (transparent)
        m_data.cortana_transparent_color = LIGHT_MODE_SEARCH_BOX_BACKGROUND_COLOR;      //要设置成透明，则将透明色设置成和背景色相同
    else
        m_data.cortana_transparent_color = SEARCH_BOX_DEFAULT_TRANSPARENT_COLOR;      //要设置成不透明，则将透明色设置成和背景色不同
}

bool CLyricSettingsDlg::IsSearchBoxTransparentInWhiteTheme() const
{
    return (m_data.cortana_transparent_color == LIGHT_MODE_SEARCH_BOX_BACKGROUND_COLOR);
}

void CLyricSettingsDlg::OnBnClickedKaraokeDisp()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.lyric_karaoke_disp = (m_karaoke_disp_check.GetCheck() != 0);
}


void CLyricSettingsDlg::OnCancel()
{
    // TODO: 在此添加专用代码和/或调用基类

    //CTabDlg::OnCancel();
}


void CLyricSettingsDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类
    //m_data.desktop_lyric_data.text_color1 = m_text_color1_static.GetFillColor();
    //m_data.desktop_lyric_data.text_color2 = m_text_color2_static.GetFillColor();
    //m_data.desktop_lyric_data.highlight_color1 = m_highlight_color1_static.GetFillColor();
    //m_data.desktop_lyric_data.highlight_color2 = m_highlight_color2_static.GetFillColor();

    m_data.lyric_line_space = m_lyric_line_space_edit.GetValue();

    //CTabDlg::OnOK();
}


void CLyricSettingsDlg::OnBnClickedLyricFuzzyMatch()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.lyric_fuzzy_match = (m_lyric_fuzzy_match_check.GetCheck() != 0);
}


BOOL CLyricSettingsDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if (pMsg->message == WM_MOUSEMOVE)
        m_tool_tip.RelayEvent(pMsg);

    return CTabDlg::PreTranslateMessage(pMsg);
}


void CLyricSettingsDlg::OnBnClickedShowLyricInCortana()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.cortana_info_enable = (m_cortana_info_enable_check.GetCheck() != 0);
    EnableControl();
}


void CLyricSettingsDlg::OnBnClickedLyricDoubleLineCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.cortana_lyric_double_line = (m_lyric_double_line_chk.GetCheck() != 0);
}


void CLyricSettingsDlg::OnCbnSelchangeCortanaColorCombo()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.cortana_color = m_cortana_color_combo.GetCurSel();
}


void CLyricSettingsDlg::OnBnClickedShowAlbumCoverInCortana()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.cortana_show_album_cover = (m_show_album_cover_in_cortana_check.GetCheck() != 0);
    EnableControl();
}


void CLyricSettingsDlg::OnBnClickedCortanaIconDeatCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.cortana_icon_beat = (m_cortana_icon_beat_check.GetCheck() != 0);
}


void CLyricSettingsDlg::OnBnClickedLyricCompatibleMode()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.cortana_lyric_compatible_mode = (m_lyric_compatible_mode_chk.GetCheck() != 0);
    EnableControl();
}


void CLyricSettingsDlg::OnBnClickedSetFont()
{
    // TODO: 在此添加控件通知处理程序代码
    LOGFONT lf{};
    theApp.m_font_set.cortana.GetFont().GetLogFont(&lf);
    CCommon::NormalizeFont(lf);
    CFontDialog fontDlg(&lf);	//构造字体对话框，初始选择字体为之前字体
    if (IDOK == fontDlg.DoModal())     // 显示字体对话框
    {
        //获取字体信息
        m_data.cortana_font.name = fontDlg.GetFaceName();
        m_data.cortana_font.size = fontDlg.GetSize() / 10;
        m_data.cortana_font.style.bold = (fontDlg.IsBold() != FALSE);
        m_data.cortana_font.style.italic = (fontDlg.IsItalic() != FALSE);
        m_data.cortana_font.style.underline = (fontDlg.IsUnderline() != FALSE);
        m_data.cortana_font.style.strike_out = (fontDlg.IsStrikeOut() != FALSE);
        m_search_box_font_changed = true;
    }
}


void CLyricSettingsDlg::OnBnClickedKeepDisplayCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.cortana_lyric_keep_display = (m_keep_display_chk.GetCheck() != 0);
}


void CLyricSettingsDlg::OnBnClickedShowSpectrumInCortana()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.cortana_show_spectrum = (m_show_spectrum_chk.GetCheck() != 0);
}


void CLyricSettingsDlg::OnBnClickedShowLyricInCortana2()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.cortana_show_lyric = (m_show_lyric_in_cortana_chk.GetCheck() != 0);
    EnableControl();
}


//void CLyricSettingsDlg::OnBnClickedSearchBoxOpaqueCheck()
//{
//    // TODO: 在此添加控件通知处理程序代码
//    m_data.cortana_opaque = (m_search_box_opaque_chk.GetCheck() != 0);
//}


void CLyricSettingsDlg::OnBnClickedShowDesktopLyric()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.show_desktop_lyric = (m_show_desktop_lyric_chk.GetCheck() != 0);
    EnableControlForDesktopLyric();
}


void CLyricSettingsDlg::OnBnClickedSetFont2()
{
    // TODO: 在此添加控件通知处理程序代码
    LOGFONT lf{};
    CFont font;
    UIFont::CreateFontSimple(font, m_data.desktop_lyric_data.lyric_font.size, m_data.desktop_lyric_data.lyric_font.name.c_str(), m_data.desktop_lyric_data.lyric_font.style);
    font.GetLogFont(&lf);
    CCommon::NormalizeFont(lf);
    CFontDialog fontDlg(&lf);	//构造字体对话框，初始选择字体为之前字体
    if (IDOK == fontDlg.DoModal())     // 显示字体对话框
    {
        //获取字体信息
        m_data.desktop_lyric_data.lyric_font.name = fontDlg.GetFaceName();
        m_data.desktop_lyric_data.lyric_font.size = fontDlg.GetSize() / 10;
        m_data.desktop_lyric_data.lyric_font.style.bold = (fontDlg.IsBold() != FALSE);
        m_data.desktop_lyric_data.lyric_font.style.italic = (fontDlg.IsItalic() != FALSE);
        m_data.desktop_lyric_data.lyric_font.style.underline = (fontDlg.IsUnderline() != FALSE);
        m_data.desktop_lyric_data.lyric_font.style.strike_out = (fontDlg.IsStrikeOut() != FALSE);
    }
}


void CLyricSettingsDlg::OnCbnSelchangeTextGradientCombo()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.desktop_lyric_data.text_gradient = m_text_gradient_combo.GetCurSel();
}


void CLyricSettingsDlg::OnCbnSelchangeHighlightGradientCombo()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.desktop_lyric_data.highlight_gradient = m_highlight_gradient_combo.GetCurSel();
}


void CLyricSettingsDlg::OnBnClickedLockDesktopLyricCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.desktop_lyric_data.lock_desktop_lyric = (m_lock_desktop_lyric_chk.GetCheck() != 0);
}


HBRUSH CLyricSettingsDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CTabDlg::OnCtlColor(pDC, pWnd, nCtlColor);

    // TODO:  在此更改 DC 的任何特性
    if (pWnd == &m_desktop_lyric_opacity_sld)		//设置滑动条控件的背景色为白色
    {
        return (HBRUSH)::GetStockObject(WHITE_BRUSH);
    }

    // TODO:  如果默认的不是所需画笔，则返回另一个画笔
    return hbr;
}


void CLyricSettingsDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    if ((pScrollBar->GetDlgCtrlID() == IDC_LYRIC_OPACITY_SLIDER))
    {
        m_data.desktop_lyric_data.opacity = m_desktop_lyric_opacity_sld.GetPos();
        CString str;
        str.Format(_T("%d%%"), m_data.desktop_lyric_data.opacity);
        SetDlgItemText(IDC_LYRIC_OPACITY_STATIC, str);

        //实时设置窗口不透明度
        if (m_pDesktopLyric != nullptr)
            m_pDesktopLyric->SetLyricOpacity(m_data.desktop_lyric_data.opacity);
    }

    CTabDlg::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CLyricSettingsDlg::OnBnClickedHideLyricWithoutLyricCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.desktop_lyric_data.hide_lyric_window_without_lyric = (m_hide_lyric_without_lyric_chk.GetCheck() != 0);
}


void CLyricSettingsDlg::OnBnClickedHideLyricPauseCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.desktop_lyric_data.hide_lyric_window_when_paused = (m_hide_lyric_paused_chk.GetCheck() != 0);
}


void CLyricSettingsDlg::OnBnClickedLyricDoubleLineCheck2()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.desktop_lyric_data.lyric_double_line = (m_desktop_lyric_double_line_chk.GetCheck() != 0);
}


void CLyricSettingsDlg::OnBnClickedDefaultStyle()
{
    // TODO: 在此添加控件通知处理程序代码
    CWnd* pBtn = GetDlgItem(IDC_DEFAULT_STYLE);
    CPoint point;
    if (pBtn != nullptr)
    {
        CRect rect;
        pBtn->GetWindowRect(rect);
        point.x = rect.left;
        point.y = rect.bottom;
        CMenu* pMenu = theApp.m_menu_set.m_lyric_default_style.GetSubMenu(1);
        if (pMenu != NULL)
            pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
    }
}


void CLyricSettingsDlg::OnLyricDefaultStyle1()
{
    // TODO: 在此添加命令处理程序代码
    if (m_pDesktopLyric != nullptr)
    {
        auto style = m_pDesktopLyric->GetDefaultStyle(0);
        ApplyDefaultLyricStyle(style);
    }
}


void CLyricSettingsDlg::OnLyricDefaultStyle2()
{
    // TODO: 在此添加命令处理程序代码
    if (m_pDesktopLyric != nullptr)
    {
        auto style = m_pDesktopLyric->GetDefaultStyle(1);
        ApplyDefaultLyricStyle(style);
    }
}


void CLyricSettingsDlg::OnLyricDefaultStyle3()
{
    // TODO: 在此添加命令处理程序代码
    if (m_pDesktopLyric != nullptr)
    {
        auto style = m_pDesktopLyric->GetDefaultStyle(2);
        ApplyDefaultLyricStyle(style);
    }
}


void CLyricSettingsDlg::OnBnClickedHideLyricWithoutLyricCheck3()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.desktop_lyric_data.lyric_background_penetrate = !m_data.desktop_lyric_data.lyric_background_penetrate;
}


afx_msg LRESULT CLyricSettingsDlg::OnColorSelected(WPARAM wParam, LPARAM lParam)
{
    CWnd* pControl = (CWnd*)wParam;
    if (pControl == &m_text_color1_static)
        m_data.desktop_lyric_data.text_color1 = m_text_color1_static.GetFillColor();
    else if (pControl == &m_text_color2_static)
        m_data.desktop_lyric_data.text_color2 = m_text_color2_static.GetFillColor();
    else if (pControl == &m_highlight_color1_static)
        m_data.desktop_lyric_data.highlight_color1 = m_highlight_color1_static.GetFillColor();
    else if (pControl == &m_highlight_color2_static)
        m_data.desktop_lyric_data.highlight_color2 = m_highlight_color2_static.GetFillColor();
    return 0;
}


void CLyricSettingsDlg::OnLyricDefaultStyle1Modify()
{
    // TODO: 在此添加命令处理程序代码
    if (MessageBox(CCommon::LoadTextFormat(IDS_MODIFY_DEFAULT_STYLE_WARNING, { 1 }), NULL, MB_ICONWARNING | MB_YESNO) == IDYES)
    {
        LyricStyleDefaultData style_data;
        CDesktopLyric::LyricSettingDatatOLyricStyleDefaultData(m_data.desktop_lyric_data, style_data);
        m_pDesktopLyric->SetDefaultStyle(style_data, 0);
    }
}


void CLyricSettingsDlg::OnLyricDefaultStyle2Modify()
{
    // TODO: 在此添加命令处理程序代码
    if (MessageBox(CCommon::LoadTextFormat(IDS_MODIFY_DEFAULT_STYLE_WARNING, { 2 }), NULL, MB_ICONWARNING | MB_YESNO) == IDYES)
    {
        LyricStyleDefaultData style_data;
        CDesktopLyric::LyricSettingDatatOLyricStyleDefaultData(m_data.desktop_lyric_data, style_data);
        m_pDesktopLyric->SetDefaultStyle(style_data, 1);
    }
}


void CLyricSettingsDlg::OnLyricDefaultStyle3Modify()
{
    // TODO: 在此添加命令处理程序代码
    if (MessageBox(CCommon::LoadTextFormat(IDS_MODIFY_DEFAULT_STYLE_WARNING, { 3 }), NULL, MB_ICONWARNING | MB_YESNO) == IDYES)
    {
        LyricStyleDefaultData style_data;
        CDesktopLyric::LyricSettingDatatOLyricStyleDefaultData(m_data.desktop_lyric_data, style_data);
        m_pDesktopLyric->SetDefaultStyle(style_data, 2);
    }
}


void CLyricSettingsDlg::OnRestoreDefaultStyle()
{
    // TODO: 在此添加命令处理程序代码
    if (MessageBox(CCommon::LoadText(IDS_RESTORE_DEFAULT_STYTLE_WARNING), NULL, MB_ICONWARNING | MB_YESNO) == IDYES)
    {
        m_pDesktopLyric->RestoreDefaultStyle();
    }
}


afx_msg LRESULT CLyricSettingsDlg::OnEditBrowseChanged(WPARAM wParam, LPARAM lParam)
{
    CString str;
    GetDlgItemText(IDC_LYRIC_PATH_EDIT, str);
    m_data.lyric_path = str.GetString();
    return 0;
}


void CLyricSettingsDlg::OnCbnSelchangeAlignmentCombo()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.cortana_lyric_align = static_cast<Alignment>(m_alignment_combo.GetCurSel());
}


void CLyricSettingsDlg::OnCbnSelchangeLyricSaveCombo()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.lyric_save_policy = static_cast<LyricSettingData::LyricSavePolicy>(m_lyric_save_policy_combo.GetCurSel());
}


void CLyricSettingsDlg::OnBnClickedUseInnerLyricCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.use_inner_lyric_first = (m_use_inner_lyric_chk.GetCheck() != 0);
}


void CLyricSettingsDlg::OnBnClickedShowUnlockWhenLocked()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.desktop_lyric_data.show_unlock_when_locked = (m_show_unlock_when_locked_chk.GetCheck() != 0);
}


void CLyricSettingsDlg::OnBnClickedShowDefaultAlbumIconChk()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.show_default_album_icon_in_search_box = (m_show_default_album_icon_chk.GetCheck() != 0);
    EnableControl();
}


void CLyricSettingsDlg::OnBnClickedSearchBoxTransparentInWhiteMode()
{
    // TODO: 在此添加控件通知处理程序代码
    bool checked = (IsDlgButtonChecked(IDC_SEARCH_BOX_TRANSPARENT_IN_WHITE_MODE) != 0);
    SetSearchBoxTransparentInWhiteTheme(checked);
}


void CLyricSettingsDlg::OnBnClickedSetFontButton()
{
    // TODO: 在此添加控件通知处理程序代码
    LOGFONT lf{};             //LOGFONT变量
    theApp.m_font_set.lyric.GetFont().GetLogFont(&lf);
    CCommon::NormalizeFont(lf);
    CFontDialog fontDlg(&lf);	//构造字体对话框，初始选择字体为之前字体
    if (IDOK == fontDlg.DoModal())     // 显示字体对话框
    {
        //获取字体信息
        m_data.lyric_font.name = fontDlg.GetFaceName();
        m_data.lyric_font.size = fontDlg.GetSize() / 10;
        m_data.lyric_font.style.bold = (fontDlg.IsBold() != FALSE);
        m_data.lyric_font.style.italic = (fontDlg.IsItalic() != FALSE);
        m_data.lyric_font.style.underline = (fontDlg.IsUnderline() != FALSE);
        m_data.lyric_font.style.strike_out = (fontDlg.IsStrikeOut() != FALSE);
        //将字体已更改flag置为true
        m_font_changed = true;
    }
}


void CLyricSettingsDlg::OnCbnSelchangeAlignmentCombo2()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.lyric_align = static_cast<Alignment>(m_lyric_alignment_combo.GetCurSel());
}


void CLyricSettingsDlg::OnCbnSelchangeDesktopLyricAlignmentCombo()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.desktop_lyric_data.lyric_align = static_cast<Alignment>(m_desktop_lyric_alignment_combo.GetCurSel());
}


void CLyricSettingsDlg::OnBnClickedShowLyricTranslateCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.show_translate = (IsDlgButtonChecked(IDC_SHOW_LYRIC_TRANSLATE_CHECK) != 0);
}
