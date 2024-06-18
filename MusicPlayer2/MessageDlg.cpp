// MessageDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "MessageDlg.h"


// CMessageDlg 对话框

#define MESSAGE_DLG_ICON_SIZE (theApp.DPI(32))

IMPLEMENT_DYNAMIC(CMessageDlg, CBaseDialog)

CMessageDlg::CMessageDlg(const wstring& dialog_name, CWnd* pParent)
    : CBaseDialog(IDD_MESSAGE_DIALOG, pParent)
{
    ASSERT(!dialog_name.empty());
    m_dialog_name = dialog_name;
}

CMessageDlg::~CMessageDlg()
{
}

bool CMessageDlg::InitializeControls()
{
    SetWindowTextW(m_title.c_str());
    SetDlgItemTextW(IDC_INFO_STATIC, m_info.c_str());
    SetDlgItemTextW(IDC_HELP_EDIT, m_message.c_str());
    SetDlgItemTextW(IDC_SYSLINK1, (L"<a>" + m_link_text + L"</a>").c_str());

    RepositionTextBasedControls({
        { CtrlTextInfo::L1, IDC_SYSLINK1, CtrlTextInfo::W_50 },
        { CtrlTextInfo::R1, IDOK, CtrlTextInfo::W32 }
        });
    return true;
}

void CMessageDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMessageDlg, CBaseDialog)
    ON_NOTIFY(NM_CLICK, IDC_SYSLINK1, &CMessageDlg::OnNMClickSyslink1)
END_MESSAGE_MAP()


// CMessageDlg 消息处理程序


BOOL CMessageDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    SetIcon(IconMgr::IconType::IT_App, FALSE);      // 设置小图标

    ShowDlgCtrl(IDC_SYSLINK1, !m_link_text.empty());

    CWnd* pWnd = GetDlgItem(IDOK);
    if (pWnd != nullptr)
        pWnd->SetFocus();

    return FALSE; // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}

void CMessageDlg::OnNMClickSyslink1(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: 在此添加控件通知处理程序代码
    if(!m_link_url.empty())
        ShellExecute(NULL, _T("open"), m_link_url.c_str(), NULL, NULL, SW_SHOW);    //打开超链接

    *pResult = 0;
}
