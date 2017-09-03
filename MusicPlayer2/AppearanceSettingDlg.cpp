// AppearanceSettingDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "AppearanceSettingDlg.h"
#include "afxdialogex.h"


// CAppearanceSettingDlg 对话框

IMPLEMENT_DYNAMIC(CAppearanceSettingDlg, CDialogEx)

CAppearanceSettingDlg::CAppearanceSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_APPEREANCE_SETTING_DLG, pParent)
	, m_line_space(0)
{

}

CAppearanceSettingDlg::~CAppearanceSettingDlg()
{
}

void CAppearanceSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TRANSPARENT_SLIDER, m_transparency_slid);
	DDX_Control(pDX, IDC_COLOR_STATIC, m_color_static);
	DDX_Control(pDX, IDC_COLOR_STATIC2, m_color_static1);
	DDX_Control(pDX, IDC_COLOR_STATIC3, m_color_static2);
	DDX_Control(pDX, IDC_COLOR_STATIC4, m_color_static3);
	DDX_Control(pDX, IDC_COLOR_STATIC5, m_color_static4);
	DDX_Control(pDX, IDC_COLOR_STATIC6, m_color_static5);
	DDX_Control(pDX, IDC_COLOR_STATIC7, m_color_static6);
	DDX_Control(pDX, IDC_FOLLOW_SYSTEM_COLOR_CHECK, m_follow_system_color_check);
	DDX_Text(pDX, IDC_FONT_SIZE_EDIT, m_line_space);
	DDV_MinMaxInt(pDX, m_line_space, 0, MAX_LINE_SPACE);
	DDX_Control(pDX, IDC_SPECTRUM_HEIGHT_SLIDER, m_spectrum_height_slid);
}

void CAppearanceSettingDlg::SetTransparency()
{
	::SetWindowLong(m_hMainWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	::SetLayeredWindowAttributes(m_hMainWnd, 0, m_transparency * 255 / 100, LWA_ALPHA);  //透明度取值范围为0~255
}

void CAppearanceSettingDlg::ClickColor()
{
	//点击了预置颜色中的其中一个时，取消“跟随系统主题色”复选按钮的选中
	CCommon::FillStaticColor(m_color_static, m_theme_color);
	m_theme_color_follow_system = false;
	m_follow_system_color_check.SetCheck(FALSE);
	//GetDlgItem(IDC_SET_PROGRESS_COLOR_BUTTON)->EnableWindow();
}

void CAppearanceSettingDlg::DrawColor()
{
	CCommon::FillStaticColor(m_color_static, m_theme_color);
	CCommon::FillStaticColor(m_color_static1, m_color1);
	CCommon::FillStaticColor(m_color_static2, m_color2);
	CCommon::FillStaticColor(m_color_static3, m_color3);
	CCommon::FillStaticColor(m_color_static4, m_color4);
	CCommon::FillStaticColor(m_color_static5, m_color5);
	CCommon::FillStaticColor(m_color_static6, m_color6);
}


BEGIN_MESSAGE_MAP(CAppearanceSettingDlg, CDialogEx)
	ON_BN_CLICKED(IDC_SET_FONT_BUTTON, &CAppearanceSettingDlg::OnBnClickedSetFontButton)
//	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_TRANSPARENT_SLIDER, &CAppearanceSettingDlg::OnNMReleasedcaptureTransparentSlider)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_SET_PROGRESS_COLOR_BUTTON, &CAppearanceSettingDlg::OnBnClickedSetThemeButton)
	ON_STN_CLICKED(IDC_COLOR_STATIC2, &CAppearanceSettingDlg::OnStnClickedColorStatic2)
	ON_STN_CLICKED(IDC_COLOR_STATIC3, &CAppearanceSettingDlg::OnStnClickedColorStatic3)
	ON_STN_CLICKED(IDC_COLOR_STATIC4, &CAppearanceSettingDlg::OnStnClickedColorStatic4)
	ON_STN_CLICKED(IDC_COLOR_STATIC5, &CAppearanceSettingDlg::OnStnClickedColorStatic5)
	ON_STN_CLICKED(IDC_COLOR_STATIC6, &CAppearanceSettingDlg::OnStnClickedColorStatic6)
	ON_STN_CLICKED(IDC_COLOR_STATIC7, &CAppearanceSettingDlg::OnStnClickedColorStatic7)
	ON_BN_CLICKED(IDC_FOLLOW_SYSTEM_COLOR_CHECK, &CAppearanceSettingDlg::OnBnClickedFollowSystemColorCheck)
	//ON_EN_CHANGE(IDC_FONT_NAME_EDIT, &CAppearanceSettingDlg::OnEnChangeFontNameEdit)
	ON_EN_CHANGE(IDC_FONT_SIZE_EDIT, &CAppearanceSettingDlg::OnEnChangeLineSpaceEdit)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, &CAppearanceSettingDlg::OnDeltaposSpin1)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CAppearanceSettingDlg 消息处理程序


BOOL CAppearanceSettingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetBackgroundColor(RGB(255, 255, 255));

	//SetDlgItemText(IDC_FONT_NAME_EDIT, m_font.c_str());
	//CString font_size_str;
	//font_size_str.Format(_T("%d"), m_line_space);
	//SetDlgItemText(IDC_FONT_SIZE_EDIT, font_size_str);

	m_transparency_slid.SetRange(20, 100);
	m_transparency_slid.SetPos(m_transparency);
	CString str;
	str.Format(_T("%d%%"), m_transparency);
	SetDlgItemText(IDC_TRANSPARENT_STATIC, str);

	m_spectrum_height_slid.SetRange(10, 300);
	m_spectrum_height_slid.SetPos(theApp.m_sprctrum_height);
	str.Format(_T("%d%%"), theApp.m_sprctrum_height);
	SetDlgItemText(IDC_SPECTRUM_HEIGHT_STATIC, str);

	//将焦点设置到“设置字体”按钮上
	//GetDlgItem(IDC_SET_FONT_BUTTON)->SetFocus();

	//为控件设置SS_NOTIFY属性，以允许响应消息
	DWORD dwStyle = m_color_static.GetStyle();
	::SetWindowLong(m_color_static.GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);
	::SetWindowLong(m_color_static1.GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);
	::SetWindowLong(m_color_static2.GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);
	::SetWindowLong(m_color_static3.GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);
	::SetWindowLong(m_color_static4.GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);
	::SetWindowLong(m_color_static5.GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);
	::SetWindowLong(m_color_static6.GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);

	m_toolTip.Create(this);
	m_toolTip.AddTool(&m_color_static, _T("当前的颜色"));
	m_toolTip.AddTool(&m_color_static1, _T("天蓝色"));
	m_toolTip.AddTool(&m_color_static2, _T("绿色"));
	m_toolTip.AddTool(&m_color_static3, _T("橙色"));
	m_toolTip.AddTool(&m_color_static4, _T("青绿色"));
	m_toolTip.AddTool(&m_color_static5, _T("浅红色"));
	m_toolTip.AddTool(&m_color_static6, _T("淡紫色"));

	////设置“更多颜色”按钮的可用状态
	//GetDlgItem(IDC_SET_PROGRESS_COLOR_BUTTON)->EnableWindow(!m_theme_color_follow_system);
	//根据是否跟随系统设置复选按钮的初始状态
#ifdef COMPILE_IN_WIN_XP
	m_follow_system_color_check.EnableWindow(FALSE);
#else
	if (IsWindows8OrGreater())
		m_follow_system_color_check.SetCheck(m_theme_color_follow_system);
	else
		m_follow_system_color_check.EnableWindow(FALSE);		//Win8以下系统禁用此复选按钮
#endif // !COMPILE_IN_WIN_XP


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CAppearanceSettingDlg::OnBnClickedSetFontButton()
{
	// TODO: 在此添加控件通知处理程序代码
	CFont font;
	font.CreatePointFont(m_font_size * 10, m_font.c_str());
	LOGFONT lf{};             //LOGFONT变量
	font.GetLogFont(&lf);
	CFontDialog fontDlg(&lf);	//构造字体对话框，初始选择字体为之前字体
	if (IDOK == fontDlg.DoModal())     // 显示字体对话框
	{
		//获取字体信息
		//m_font = fontDlg.m_cf.lpLogFont->lfFaceName;
		//m_font_size = fontDlg.m_cf.iPointSize / 10;
		m_font = fontDlg.GetFaceName();
		m_font_size = fontDlg.GetSize() / 10;
		//将字体已更改flag置为true
		m_font_changed = true;
	}
}


//void CAppearanceSettingDlg::OnNMReleasedcaptureTransparentSlider(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	// TODO: 在此添加控件通知处理程序代码
//	m_transparency = m_transparency_slid.GetPos();
//	CString str;
//	str.Format(_T("不透明度：%%%d"), m_transparency);
//	SetDlgItemText(IDC_TRANSPARENT_STATIC, str);
//	*pResult = 0;
//}


void CAppearanceSettingDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/
	//滑动水平滑动条时响应此函数
	if ((pScrollBar->GetDlgCtrlID() == IDC_TRANSPARENT_SLIDER))
	{
		m_transparency = m_transparency_slid.GetPos();
		CString str;
		str.Format(_T("%d%%"), m_transparency);
		SetDlgItemText(IDC_TRANSPARENT_STATIC, str);

		SetTransparency();		//实时设置窗口不透明度
	}
	if ((pScrollBar->GetDlgCtrlID() == IDC_SPECTRUM_HEIGHT_SLIDER))
	{
		theApp.m_sprctrum_height = m_spectrum_height_slid.GetPos();
		CString str;
		str.Format(_T("%d%%"), theApp.m_sprctrum_height);
		SetDlgItemText(IDC_SPECTRUM_HEIGHT_STATIC, str);
	}

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CAppearanceSettingDlg::OnBnClickedSetThemeButton()
{
	// TODO: 在此添加控件通知处理程序代码
	CColorDialog colorDlg(m_theme_color, 0, this);
	if (colorDlg.DoModal() == IDOK)
	{
		m_theme_color = colorDlg.GetColor();
		//if (m_theme_color == 0)
		//	MessageBox(_T("警告：将主题颜色设置成黑色会使播放列表中正在播放的项目看不见！"), NULL, MB_ICONWARNING);
		if(m_theme_color == RGB(255,255,255))
			MessageBox(_T("警告：将主题颜色设置成白色会使进度条完全看不见！"), NULL, MB_ICONWARNING);
		CCommon::FillStaticColor(m_color_static, m_theme_color);
		//设置了“更多颜色”之后，取消“跟随系统主题色”复选按钮的选中
		m_theme_color_follow_system = false;
		m_follow_system_color_check.SetCheck(FALSE);
	}
}


void CAppearanceSettingDlg::OnStnClickedColorStatic2()
{
	// TODO: 在此添加控件通知处理程序代码
	m_theme_color = m_color1;
	ClickColor();
}


void CAppearanceSettingDlg::OnStnClickedColorStatic3()
{
	// TODO: 在此添加控件通知处理程序代码
	m_theme_color = m_color2;
	ClickColor();
}


void CAppearanceSettingDlg::OnStnClickedColorStatic4()
{
	// TODO: 在此添加控件通知处理程序代码
	m_theme_color = m_color3;
	ClickColor();
}


void CAppearanceSettingDlg::OnStnClickedColorStatic5()
{
	// TODO: 在此添加控件通知处理程序代码
	m_theme_color = m_color4;
	ClickColor();
}


void CAppearanceSettingDlg::OnStnClickedColorStatic6()
{
	// TODO: 在此添加控件通知处理程序代码
	m_theme_color = m_color5;
	ClickColor();
}


void CAppearanceSettingDlg::OnStnClickedColorStatic7()
{
	// TODO: 在此添加控件通知处理程序代码
	m_theme_color = m_color6;
	ClickColor();
}


BOOL CAppearanceSettingDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_MOUSEMOVE)
		m_toolTip.RelayEvent(pMsg);

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CAppearanceSettingDlg::OnBnClickedFollowSystemColorCheck()
{
	// TODO: 在此添加控件通知处理程序代码
	m_theme_color_follow_system = (m_follow_system_color_check.GetCheck() != 0);
	//GetDlgItem(IDC_SET_PROGRESS_COLOR_BUTTON)->EnableWindow(!m_theme_color_follow_system);
}


void CAppearanceSettingDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialogEx::OnCancel();
}


void CAppearanceSettingDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialogEx::OnOK();
}


//void CAppearanceSettingDlg::OnEnChangeFontNameEdit()
//{
//	// TODO:  如果该控件是 RICHEDIT 控件，它将不
//	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
//	// 函数并调用 CRichEditCtrl().SetEventMask()，
//	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。
//
//	// TODO:  在此添加控件通知处理程序代码
//	CString font_name;
//	GetDlgItemText(IDC_FONT_NAME_EDIT, font_name);
//	m_font = font_name;
//	m_font_changed = true;
//}


void CAppearanceSettingDlg::OnEnChangeLineSpaceEdit()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	//CString font_size;
	//GetDlgItemText(IDC_FONT_SIZE_EDIT, font_size);
	//m_line_space = _wtoi(font_size);
	UpdateData(TRUE);
	if (m_line_space < 0) m_line_space = 0;
	if (m_line_space > MAX_LINE_SPACE) m_line_space = MAX_LINE_SPACE;
	UpdateData(FALSE);
	m_font_changed = true;
}


void CAppearanceSettingDlg::OnDeltaposSpin1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	if (pNMUpDown->iDelta == 1) // 如果此值为1 , 说明点击了Spin的往下箭头 
	{
		//减少歌词行间距的数值
		UpdateData(TRUE);
		if (m_line_space > 0)
			m_line_space--;
		UpdateData(FALSE);
	}
	else if (pNMUpDown->iDelta == -1) // 如果此值为-1 , 说明点击了Spin的往上箭头 
	{
		//增加歌词行间距的数值
		UpdateData(TRUE);
		if (m_line_space < MAX_LINE_SPACE)
			m_line_space++;
		UpdateData(FALSE);
	}
	UpdateData(TRUE);
	m_font_changed = true;

	*pResult = 0;
}



HBRUSH CAppearanceSettingDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	if (pWnd == &m_transparency_slid || pWnd == &m_spectrum_height_slid)		//设置滑动条控件的背景色为白色
	{
		return (HBRUSH)::GetStockObject(WHITE_BRUSH);
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}
