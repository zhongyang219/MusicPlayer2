// PlaySettingsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "LyricSettingsDlg.h"
#include "afxdialogex.h"


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
}


BEGIN_MESSAGE_MAP(CLyricSettingsDlg, CTabDlg)
	ON_BN_CLICKED(IDC_KARAOKE_DISP, &CLyricSettingsDlg::OnBnClickedKaraokeDisp)
	ON_BN_CLICKED(IDC_EXPLORE_LYRIC_BUTTON, &CLyricSettingsDlg::OnBnClickedExploreLyricButton)
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
END_MESSAGE_MAP()


// CLyricSettingsDlg 消息处理程序


BOOL CLyricSettingsDlg::OnInitDialog()
{
	CTabDlg::OnInitDialog();

	// TODO:  在此添加额外的初始化

	//初始化各控件的状态
	m_karaoke_disp_check.SetCheck(m_data.lyric_karaoke_disp);
	m_lyric_fuzzy_match_check.SetCheck(m_data.lyric_fuzzy_match);
	m_lyric_double_line_chk.SetCheck(m_data.cortana_lyric_double_line);
	m_show_album_cover_in_cortana_check.SetCheck(m_data.cortana_show_album_cover);
	m_cortana_icon_beat_check.SetCheck(m_data.cortana_icon_beat);
	//m_cortana_icon_beat_check.EnableWindow(!m_data.cortana_show_album_cover);
	m_lyric_compatible_mode_chk.SetCheck(m_data.cortana_lyric_compatible_mode);
	m_keep_display_chk.SetCheck(m_data.cortana_lyric_keep_display);
	m_show_spectrum_chk.SetCheck(m_data.cortana_show_spectrum);
    m_show_lyric_in_cortana_chk.SetCheck(m_data.cortana_show_lyric);
	if (CWinVersionHelper::IsWindows10OrLater())
	{
		m_cortana_info_enable_check.SetCheck(m_data.cortana_info_enable);
	}
	else
	{
		m_cortana_info_enable_check.EnableWindow(FALSE);		//Win10以下系统禁用此复选按钮
		m_data.cortana_info_enable = false;
	}

	EnableControl();

	SetDlgItemText(IDC_LYRIC_PATH_EDIT, m_data.lyric_path.c_str());

	m_tool_tip.Create(this);
	m_tool_tip.SetMaxTipWidth(300);
	m_tool_tip.AddTool(&m_lyric_fuzzy_match_check, CCommon::LoadText(IDS_LYRIC_FUZZY_MATHC_TIP_INFO));
	m_tool_tip.AddTool(GetDlgItem(IDC_LYRIC_PATH_EDIT), CCommon::LoadText(IDS_LYRIC_PATH_TIP_INFO));
	m_tool_tip.AddTool(GetDlgItem(IDC_SHOW_LYRIC_IN_CORTANA), CCommon::LoadText(IDS_CORTANA_SHOW_LYRIC_TIP_INFO));

	m_tool_tip.SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

	m_cortana_color_combo.AddString(CCommon::LoadText(IDS_FOLLOWING_SYSTEM));
	m_cortana_color_combo.AddString(CCommon::LoadText(IDS_BLACK));
	m_cortana_color_combo.AddString(CCommon::LoadText(IDS_WHITE));
	m_cortana_color_combo.SetCurSel(m_data.cortana_color);

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
	m_cortana_icon_beat_check.EnableWindow(enable);
	GetDlgItem(IDC_SET_FONT)->EnableWindow(enable);
	m_keep_display_chk.EnableWindow(enable);
	m_show_spectrum_chk.EnableWindow(enable);
	m_lyric_compatible_mode_chk.EnableWindow(m_data.cortana_info_enable);
}


void CLyricSettingsDlg::OnBnClickedKaraokeDisp()
{
	// TODO: 在此添加控件通知处理程序代码
	m_data.lyric_karaoke_disp = (m_karaoke_disp_check.GetCheck() != 0);
}


void CLyricSettingsDlg::OnBnClickedExploreLyricButton()
{
	// TODO: 在此添加控件通知处理程序代码
#ifdef COMPILE_IN_WIN_XP
	CFolderBrowserDlg folderPickerDlg(this->GetSafeHwnd());
	folderPickerDlg.SetInfo(CCommon::LoadText(IDS_SELECT_LYRIC_FOLDER));
#else
	CFolderPickerDialog folderPickerDlg(m_data.lyric_path.c_str());
	folderPickerDlg.m_ofn.lpstrTitle = CCommon::LoadText(IDS_SELECT_LYRIC_FOLDER);		//设置对话框标题
#endif // COMPILE_IN_WIN_XP
	if (folderPickerDlg.DoModal() == IDOK)
	{
		m_data.lyric_path = folderPickerDlg.GetPathName();
		if (m_data.lyric_path.back() != L'\\') m_data.lyric_path.push_back(L'\\');	//确保路径末尾有反斜杠
		SetDlgItemText(IDC_LYRIC_PATH_EDIT, m_data.lyric_path.c_str());
	}
}


void CLyricSettingsDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CTabDlg::OnCancel();
}


void CLyricSettingsDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

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
	//m_cortana_icon_beat_check.EnableWindow(!m_data.cortana_show_album_cover);
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
	LOGFONT lf{};             //LOGFONT变量
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
		m_font_changed = true;
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
