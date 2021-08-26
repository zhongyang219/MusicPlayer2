#include "stdafx.h"
#include "MusicPlayer2.h"
#include "AboutDlg.h"
#include "CDonateDlg.h"
#include "MessageDlg.h"
#include "GdiPlusTool.h"

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
    ON_NOTIFY(NM_CLICK, IDC_LICENSE_SYSLINK, &CAboutDlg::OnNMClickLicenseSyslink)
    ON_WM_ERASEBKGND()
    ON_WM_CTLCOLOR()
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
    CString compile_time = CCommon::GetLastCompileTime();
	temp_str.Replace(_T("<compile_date>"), compile_time);
	temp_str.Replace(_T("<year>"), COPY_RIGHT_YEAR);
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

    //载入图片
    m_about_pic.LoadFromResource(AfxGetResourceHandle(), IDB_DEFAULT_COVER);

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
	theApp.CheckUpdateInThread(true);
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
    //填充背景
    draw.GetDC()->FillSolidRect(m_rc_pic, RGB(212, 230, 255));

    //画背景图
    CSize img_size{ m_about_pic.GetWidth(), m_about_pic.GetHeight() };
    draw.DrawBitmap(m_about_pic, m_rc_pic.TopLeft(), m_rc_pic.Size(), CDrawCommon::StretchMode::FILL);
}


void CAboutDlg::OnNMClickLicenseSyslink(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: 在此添加控件通知处理程序代码
    CMessageDlg dlg;
    dlg.SetWindowTitle(CCommon::LoadText(IDS_LICENSE));
    dlg.SetInfoText(CCommon::LoadText(IDS_LICENSE_EXPLAIN));
    dlg.SetMessageText(CCommon::GetTextResource(IDR_LICENSE, CodeType::UTF8_NO_BOM));
    dlg.DoModal();
    *pResult = 0;
}


BOOL CAboutDlg::OnEraseBkgnd(CDC* pDC)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    CRect draw_rect;
    GetClientRect(draw_rect);
    pDC->FillSolidRect(draw_rect, GetSysColor(COLOR_WINDOW));

    //绘制白色背景
    int white_height;       //白色区域的高度
    CRect rc_copyright{};
    ::GetWindowRect(GetDlgItem(IDC_STATIC_COPYRIGHT)->GetSafeHwnd(), rc_copyright);
    ScreenToClient(rc_copyright);
    white_height = rc_copyright.bottom + theApp.DPI(4);

    //绘制“确定”按钮上方的分割线
    CRect rc_line{ draw_rect };
    rc_line.top = white_height;
    rc_line.bottom = white_height + theApp.DPI(1);
    pDC->FillSolidRect(rc_line, RGB(210, 210, 210));

    //绘制灰色背景
    CRect rc_gray{ rc_line };
    rc_gray.top = rc_line.bottom;
    rc_gray.bottom = draw_rect.bottom;
    pDC->FillSolidRect(rc_gray, GetSysColor(COLOR_BTNFACE));

    return TRUE;
    //return CDialog::OnEraseBkgnd(pDC);
}


HBRUSH CAboutDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

    // TODO:  在此更改 DC 的任何特性
    UINT ctrl_id = pWnd->GetDlgCtrlID();
    if (ctrl_id == IDC_STATIC_VERSION || ctrl_id == IDC_STATIC_COPYRIGHT)
    {
        static HBRUSH hBackBrush{};
        if (hBackBrush == NULL)
            hBackBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
        pDC->SetBkColor(GetSysColor(COLOR_WINDOW));
        return hBackBrush;
    }


    // TODO:  如果默认的不是所需画笔，则返回另一个画笔
    return hbr;
}
