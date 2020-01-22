#include "stdafx.h"
#include "MusicPlayer2.h"
#include "AboutDlg.h"
#include "CDonateDlg.h"

CAboutDlg::CAboutDlg() : CDialog(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK1, &CAboutDlg::OnNMClickSyslink1)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK2, &CAboutDlg::OnNMClickSyslink2)
	ON_NOTIFY(NM_CLICK, IDC_GITHUB_SYSLINK, &CAboutDlg::OnNMClickGithubSyslink)
	ON_NOTIFY(NM_CLICK, IDC_DONATE_SYSLINK, &CAboutDlg::OnNMClickDonateSyslink)
    ON_WM_PAINT()
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	CString version_info;
	GetDlgItemText(IDC_STATIC_VERSION, version_info);
	version_info.Replace(_T("<version>"), VERSION);
#ifdef COMPILE_IN_WIN_XP
	version_info += _T(" (For WinXP)");
#endif // COMPILE_FOR_WINXP

#ifdef _M_X64
	version_info += _T(" (x64)");
#endif

#ifdef _DEBUG
	version_info += _T(" (Debug)");
#endif

	SetDlgItemText(IDC_STATIC_VERSION, version_info);

	//设置最后编译日期
	CString temp_str;
	GetDlgItemText(IDC_STATIC_COPYRIGHT, temp_str);
	temp_str.Replace(_T("<compile_date>"), COMPILE_DATE);
	CString year{ COMPILE_DATE };
	year = year.Left(4);
	temp_str.Replace(_T("<year>"), year);
	SetDlgItemText(IDC_STATIC_COPYRIGHT, temp_str);

	m_tool_tip.Create(this);
	m_tool_tip.AddTool(GetDlgItem(IDC_SYSLINK1), CCommon::LoadText(IDS_SEND_EMAIL_TO_ATHOUR, _T("\r\nmailto:zhongyang219@hotmail.com")));
	m_tool_tip.AddTool(GetDlgItem(IDC_GITHUB_SYSLINK), CCommon::LoadText(IDS_GOTO_GITHUB, _T("\r\nhttps://github.com/zhongyang219/MusicPlayer2")));
	m_tool_tip.SetDelayTime(300);	//设置延迟
	m_tool_tip.SetMaxTipWidth(theApp.DPI(400));

    //设置图片的位置
    CRect rect;
    GetClientRect(rect);
    m_rc_pic = rect;
    ::GetWindowRect(GetDlgItem(IDC_STATIC_VERSION)->GetSafeHwnd(), rect);
    ScreenToClient(rect);
    m_rc_pic.bottom = rect.top - theApp.DPI(6);
    if (m_rc_pic.Height() <= 0)
        m_rc_pic.bottom = m_rc_pic.top + theApp.DPI(50);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CAboutDlg::OnNMClickSyslink1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	//点击了“联系作者”
	ShellExecute(NULL, _T("open"), _T("mailto:zhongyang219@hotmail.com"), NULL, NULL, SW_SHOW);	//打开超链接
	*pResult = 0;
}

void CAboutDlg::OnNMClickSyslink2(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	//点击了“检查更新”
	theApp.CheckUpdate(true);
	*pResult = 0;
}

void CAboutDlg::OnNMClickGithubSyslink(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	ShellExecute(NULL, _T("open"), _T("https://github.com/zhongyang219/MusicPlayer2/"), NULL, NULL, SW_SHOW);	//打开超链接
	*pResult = 0;
}

BOOL CAboutDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_MOUSEMOVE)
		m_tool_tip.RelayEvent(pMsg);


	//if (pMsg->message == WM_KEYDOWN)
	//{
	//	if ((GetKeyState(VK_CONTROL) & 0x80) && (GetKeyState(VK_SHIFT) & 0x8000) && pMsg->wParam == 'Z')
	//	{
	//		CTest::Test();
	//	}
	//}


	return CDialog::PreTranslateMessage(pMsg);
}



void CAboutDlg::OnNMClickDonateSyslink(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	CDonateDlg dlg;
	dlg.DoModal();

	*pResult = 0;
}


void CAboutDlg::OnPaint()
{
    CPaintDC dc(this); // device context for painting
                       // TODO: 在此处添加消息处理程序代码
                       // 不为绘图消息调用 CDialog::OnPaint()
    CDrawCommon draw;
    draw.Create(&dc, this);
    draw.GetDC()->FillSolidRect(m_rc_pic, RGB(161, 200, 255));
    draw.DrawBitmap(IDB_ABOUT_BITMAP, m_rc_pic.TopLeft(), m_rc_pic.Size(), CDrawCommon::StretchMode::FIT);

}
