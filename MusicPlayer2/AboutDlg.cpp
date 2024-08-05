#include "stdafx.h"
#include "MusicPlayer2.h"
#include "AboutDlg.h"
#include "CDonateDlg.h"
#include "MessageDlg.h"

CAboutDlg::CAboutDlg()
    : CBaseDialog(IDD_ABOUTBOX)
{
}

CAboutDlg::~CAboutDlg()
{
}

CString CAboutDlg::GetDialogName() const
{
    return L"AboutDlg";
}

bool CAboutDlg::InitializeControls()
{
    wstring temp;
    temp = theApp.m_str_table.LoadText(L"TITLE_ABOUTBOX");
    SetWindowTextW(temp.c_str());
    temp = L"MusicPlayer2，V" APP_VERSION;

#ifdef COMPILE_IN_WIN_XP
    temp += L" (For WinXP)";
#endif // COMPILE_FOR_WINXP

#ifdef _M_X64
    temp += L" (x64)";
#endif

#ifdef _DEBUG
    temp += L" (Debug)";
#endif

    wstring time_str, hash_str;
    CCommon::GetLastCompileTime(time_str, hash_str);
    if (!hash_str.empty())
        temp += L"   (" + hash_str + L")";

    SetDlgItemTextW(IDC_STATIC_VERSION, temp.c_str());
    temp = L"Copyright (C) 2017-" COPY_RIGHT_YEAR L" By ZhongYang\r\n";
    temp += theApp.m_str_table.LoadTextFormat(L"TXT_ABOUTBOX_LAST_BUILD_DATE", { time_str });
    SetDlgItemTextW(IDC_STATIC_COPYRIGHT, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_ABOUTBOX_THIRD_PARTY_LIB");
    SetDlgItemTextW(IDC_STATIC_THIRD_PARTY_LIB, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_ABOUTBOX_OTHER_SOFTWARE");
    SetDlgItemTextW(IDC_STATIC_OTHER_SOFTWARE, temp.c_str());
    temp = L"<a>" + theApp.m_str_table.LoadText(L"TXT_ABOUTBOX_CONTACT_AUTHOR") + L"</a>";
    SetDlgItemTextW(IDC_SYSLINK1, temp.c_str());
    temp = L"<a>" + theApp.m_str_table.LoadText(L"TXT_ABOUTBOX_CHECK_UPDATE") + L"</a>";
    SetDlgItemTextW(IDC_SYSLINK2, temp.c_str());
    temp = L"<a>" + theApp.m_str_table.LoadText(L"TXT_ABOUTBOX_GITHUB") + L"</a>";
    SetDlgItemTextW(IDC_GITHUB_SYSLINK, temp.c_str());
    temp = L"<a>" + theApp.m_str_table.LoadText(L"TXT_ABOUTBOX_LICENSE") + L"</a>";
    SetDlgItemTextW(IDC_LICENSE_SYSLINK, temp.c_str());
    temp = L"<a>" + theApp.m_str_table.LoadText(L"TXT_ABOUTBOX_DONATE") + L"</a>";
    SetDlgItemTextW(IDC_DONATE_SYSLINK, temp.c_str());
    temp = L"<a>" + theApp.m_str_table.LoadText(L"TXT_ABOUTBOX_ACKNOWLEDGEMENT") + L"</a>";
    SetDlgItemTextW(IDC_ACKNOWLEDGEMENT_SYSLINK, temp.c_str());
    temp = theApp.m_str_table.LoadText(L"TXT_OK");
    SetDlgItemTextW(IDOK, temp.c_str());

    RepositionTextBasedControls({
        { CtrlTextInfo::L3, IDC_SYSLINK1, CtrlTextInfo::W_50 },
        { CtrlTextInfo::L2, IDC_SYSLINK2, CtrlTextInfo::W_50 },
        { CtrlTextInfo::L1, IDC_GITHUB_SYSLINK, CtrlTextInfo::W_50 },
        { CtrlTextInfo::L3, IDC_LICENSE_SYSLINK, CtrlTextInfo::W_50 },
        { CtrlTextInfo::L2, IDC_DONATE_SYSLINK, CtrlTextInfo::W_50 },
        { CtrlTextInfo::L1, IDC_ACKNOWLEDGEMENT_SYSLINK, CtrlTextInfo::W_50 }
        });

    return true;
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CBaseDialog)
    ON_NOTIFY(NM_CLICK, IDC_SYSLINK1, &CAboutDlg::OnNMClickSyslink1)
    ON_NOTIFY(NM_CLICK, IDC_SYSLINK2, &CAboutDlg::OnNMClickSyslink2)
    ON_NOTIFY(NM_CLICK, IDC_GITHUB_SYSLINK, &CAboutDlg::OnNMClickGithubSyslink)
    ON_NOTIFY(NM_CLICK, IDC_DONATE_SYSLINK, &CAboutDlg::OnNMClickDonateSyslink)
    ON_WM_PAINT()
    ON_NOTIFY(NM_CLICK, IDC_LICENSE_SYSLINK, &CAboutDlg::OnNMClickLicenseSyslink)
    ON_WM_ERASEBKGND()
    ON_WM_CTLCOLOR()
    ON_NOTIFY(NM_CLICK, IDC_SYSLINK_BASS, &CAboutDlg::OnNMClickSyslinkBass)
    ON_NOTIFY(NM_CLICK, IDC_SYSLINK_TAGLIB, &CAboutDlg::OnNMClickSyslinkTaglib)
    ON_NOTIFY(NM_CLICK, IDC_SYSLINK_TINYXML2, &CAboutDlg::OnNMClickSyslinkTinyxml2)
    ON_NOTIFY(NM_CLICK, IDC_SYSLINK_SCINTILLA, &CAboutDlg::OnNMClickSyslinkScintilla)
    ON_NOTIFY(NM_CLICK, IDC_SYSLINK_TRAFFICMONITOR, &CAboutDlg::OnNMClickSyslinkTrafficmonitor)
    ON_NOTIFY(NM_CLICK, IDC_SYSLINK_SIMPLENOTEPAD, &CAboutDlg::OnNMClickSyslinkSimplenotepad)
    ON_NOTIFY(NM_CLICK, IDC_ACKNOWLEDGEMENT_SYSLINK, &CAboutDlg::OnNMClickAcknowledgementSyslink)
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // 设置背景色为白色，为使用CBaseDialog::OnCtlColor需要此初始化
    SetBackgroundColor(GetSysColor(COLOR_WINDOW));

    m_tool_tip.Create(this);
    m_tool_tip.AddTool(GetDlgItem(IDC_SYSLINK1), (theApp.m_str_table.LoadText(L"TIP_ABOUTBOX_SEND_EMAIL_TO_AUTHOR") + L"\r\nmailto:zhongyang219@hotmail.com").c_str());
    m_tool_tip.AddTool(GetDlgItem(IDC_GITHUB_SYSLINK), (theApp.m_str_table.LoadText(L"TIP_ABOUTBOX_GOTO_GITHUB") + L"\r\nhttps://github.com/zhongyang219/MusicPlayer2").c_str());
    m_tool_tip.AddTool(GetDlgItem(IDC_SYSLINK_BASS), L"http://www.un4seen.com/bass.html");
    m_tool_tip.AddTool(GetDlgItem(IDC_SYSLINK_TAGLIB), L"http://taglib.org/");
    m_tool_tip.AddTool(GetDlgItem(IDC_SYSLINK_TINYXML2), L"https://github.com/leethomason/tinyxml2");
    m_tool_tip.AddTool(GetDlgItem(IDC_SYSLINK_SCINTILLA), L"https://www.scintilla.org/index.html");
    m_tool_tip.AddTool(GetDlgItem(IDC_SYSLINK_TRAFFICMONITOR), (theApp.m_str_table.LoadText(L"TIP_ABOUTBOX_TRAFFICMONITOR_DESCRIPTION") + L"\r\nhttps://github.com/zhongyang219/TrafficMonitor").c_str());
    m_tool_tip.AddTool(GetDlgItem(IDC_SYSLINK_SIMPLENOTEPAD), (theApp.m_str_table.LoadText(L"TIP_ABOUTBOX_SIMPLENOTEPAD_DESCRIPTION") + L"\r\nhttps://github.com/zhongyang219/SimpleNotePad").c_str());
    m_tool_tip.SetDelayTime(300);   //设置延迟
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
    // 计算背景白色区域高度
    ::GetWindowRect(GetDlgItem(IDOK)->GetSafeHwnd(), rect);
    ScreenToClient(rect);
    m_white_height = rect.top - theApp.DPI(8);

    //载入图片
    m_about_pic.LoadFromResource(AfxGetResourceHandle(), IDB_DEFAULT_COVER);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}

void CAboutDlg::OnNMClickSyslink1(NMHDR* pNMHDR, LRESULT* pResult)
{
    // TODO: 在此添加控件通知处理程序代码
    //点击了“联系作者”
    ShellExecute(NULL, _T("open"), _T("mailto:zhongyang219@hotmail.com"), NULL, NULL, SW_SHOW); //打开超链接
    *pResult = 0;
}

void CAboutDlg::OnNMClickSyslink2(NMHDR* pNMHDR, LRESULT* pResult)
{
    // TODO: 在此添加控件通知处理程序代码
    //点击了“检查更新”
    theApp.CheckUpdateInThread(true);
    *pResult = 0;
}

void CAboutDlg::OnNMClickGithubSyslink(NMHDR* pNMHDR, LRESULT* pResult)
{
    // TODO: 在此添加控件通知处理程序代码
    ShellExecute(NULL, _T("open"), _T("https://github.com/zhongyang219/MusicPlayer2/"), NULL, NULL, SW_SHOW);   //打开超链接
    *pResult = 0;
}

BOOL CAboutDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if (pMsg->message == WM_MOUSEMOVE)
        m_tool_tip.RelayEvent(pMsg);


    //if (pMsg->message == WM_KEYDOWN)
    //{
    //  if ((GetKeyState(VK_CONTROL) & 0x80) && (GetKeyState(VK_SHIFT) & 0x8000) && pMsg->wParam == 'Z')
    //  {
    //      CTest::Test();
    //  }
    //}


    return CBaseDialog::PreTranslateMessage(pMsg);
}



void CAboutDlg::OnNMClickDonateSyslink(NMHDR* pNMHDR, LRESULT* pResult)
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
                       // 不为绘图消息调用 CBaseDialog::OnPaint()
    CDrawCommon draw;
    draw.Create(&dc);
    //填充背景
    draw.GetDC()->FillSolidRect(m_rc_pic, RGB(212, 230, 255));

    //画背景图
    CSize img_size{ m_about_pic.GetWidth(), m_about_pic.GetHeight() };
    draw.DrawImage(m_about_pic, m_rc_pic.TopLeft(), m_rc_pic.Size(), CDrawCommon::StretchMode::FILL);
}


void CAboutDlg::OnNMClickLicenseSyslink(NMHDR* pNMHDR, LRESULT* pResult)
{
    // TODO: 在此添加控件通知处理程序代码
    CMessageDlg dlg(L"LicenseDlg");
    dlg.SetWindowTitle(theApp.m_str_table.LoadText(L"TITLE_LICENSE"));
    dlg.SetInfoText(theApp.m_str_table.LoadText(L"TXT_LICENSE_EXPLAIN"));
    dlg.SetMessageText(CCommon::GetTextResource(IDR_LICENSE, CodeType::UTF8_NO_BOM));
    dlg.DoModal();
    *pResult = 0;
}


BOOL CAboutDlg::OnEraseBkgnd(CDC* pDC)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    CRect draw_rect;
    GetClientRect(draw_rect);
    int client_bottom = draw_rect.bottom;
    //绘制白色背景
    draw_rect.bottom = m_white_height;
    pDC->FillSolidRect(draw_rect, GetSysColor(COLOR_WINDOW));

    //绘制“确定”按钮上方的分割线
    draw_rect.top = draw_rect.bottom;
    draw_rect.bottom = draw_rect.top + max(theApp.DPI(1), 1);
    pDC->FillSolidRect(draw_rect, RGB(210, 210, 210));

    //绘制灰色背景
    draw_rect.top = draw_rect.bottom;
    draw_rect.bottom = client_bottom;
    pDC->FillSolidRect(draw_rect, GetSysColor(COLOR_BTNFACE));

    return TRUE;
}


HBRUSH CAboutDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    CRect rect;
    ::GetWindowRect(pWnd->GetSafeHwnd(), rect);
    ScreenToClient(rect);
    // 如果控件在白色区域那么使用CBaseDialog的方法修改控件背景色，灰色区域使用原版CDialog的方法
    if (rect.top < m_white_height)
        return CBaseDialog::OnCtlColor(pDC, pWnd, nCtlColor);
    else
        return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}


void CAboutDlg::OnNMClickSyslinkBass(NMHDR* pNMHDR, LRESULT* pResult)
{
    // TODO: 在此添加控件通知处理程序代码
    ShellExecute(NULL, _T("open"), _T("http://www.un4seen.com/bass.html"), NULL, NULL, SW_SHOW);    //打开超链接
    *pResult = 0;
}


void CAboutDlg::OnNMClickSyslinkTaglib(NMHDR* pNMHDR, LRESULT* pResult)
{
    // TODO: 在此添加控件通知处理程序代码
    ShellExecute(NULL, _T("open"), _T("http://taglib.org/"), NULL, NULL, SW_SHOW);  //打开超链接
    *pResult = 0;
}


void CAboutDlg::OnNMClickSyslinkTinyxml2(NMHDR* pNMHDR, LRESULT* pResult)
{
    // TODO: 在此添加控件通知处理程序代码
    ShellExecute(NULL, _T("open"), _T("https://github.com/leethomason/tinyxml2"), NULL, NULL, SW_SHOW); //打开超链接
    *pResult = 0;
}


void CAboutDlg::OnNMClickSyslinkScintilla(NMHDR* pNMHDR, LRESULT* pResult)
{
    // TODO: 在此添加控件通知处理程序代码
    ShellExecute(NULL, _T("open"), _T("https://www.scintilla.org/index.html"), NULL, NULL, SW_SHOW);    //打开超链接
    *pResult = 0;
}


void CAboutDlg::OnNMClickSyslinkTrafficmonitor(NMHDR* pNMHDR, LRESULT* pResult)
{
    // TODO: 在此添加控件通知处理程序代码
    ShellExecute(NULL, _T("open"), _T("https://github.com/zhongyang219/TrafficMonitor"), NULL, NULL, SW_SHOW);  //打开超链接
    *pResult = 0;
}


void CAboutDlg::OnNMClickSyslinkSimplenotepad(NMHDR* pNMHDR, LRESULT* pResult)
{
    // TODO: 在此添加控件通知处理程序代码
    ShellExecute(NULL, _T("open"), _T("https://github.com/zhongyang219/SimpleNotePad"), NULL, NULL, SW_SHOW);   //打开超链接
    *pResult = 0;
}


void CAboutDlg::OnNMClickAcknowledgementSyslink(NMHDR* pNMHDR, LRESULT* pResult)
{
    // TODO: 在此添加控件通知处理程序代码
    CMessageDlg dlg(L"AcknowledgementDlg");
    dlg.SetWindowTitle(theApp.m_str_table.LoadText(L"TITLE_ACKNOWLEDGEMENT"));
    std::wstringstream wss;
    wss << theApp.m_str_table.LoadText(L"TXT_ACKNOWLEDGEMENT_INFO") << L"\r\n"
        << CCommon::GetTextResource(IDR_ACKNOWLEDGEMENT, CodeType::UTF8_NO_BOM);

    // 这里排版需要重做
    wss << L"\r\nTranslators\r\n------------------------";
    const auto& lang_list = theApp.m_str_table.GetLanguageList();
    // translator保证至少有一个元素
    for (const auto& lang : lang_list)
    {
        wss << L"\r\n" << lang.display_name + L"(" + lang.file_name + L")    " << lang.translator.front();
        for (auto iter = lang.translator.begin() + 1; iter != lang.translator.end(); ++iter)
            wss << L"; " << *iter;
    }

    dlg.SetMessageText(wss.str());
    dlg.DoModal();
    *pResult = 0;
}
