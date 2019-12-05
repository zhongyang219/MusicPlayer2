// TestDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "TestDlg.h"
#include "afxdialogex.h"

// CTestDlg 对话框

IMPLEMENT_DYNAMIC(CTestDlg, CDialog)

CTestDlg::CTestDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_TEST_DIALOG, pParent)
{

}

CTestDlg::~CTestDlg()
{
}

void CTestDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TEST_TOOLBAR, m_toolbar);
    DDX_Control(pDX, IDC_TEST_PROGRESS_BAR, m_progress_bar);
    DDX_Control(pDX, IDC_MFCEDITBROWSE1, m_browse_edit);
}


BEGIN_MESSAGE_MAP(CTestDlg, CDialog)
    ON_WM_TIMER()
END_MESSAGE_MAP()


// CTestDlg 消息处理程序


BOOL CTestDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    m_toolbar.SetIconSize(theApp.DPI(20));
    m_toolbar.AddToolButton(theApp.m_icon_set.media_lib, _T("添加"), _T("打开文件"), ID_SET_PATH, false);
    m_toolbar.AddToolButton(theApp.m_icon_set.setting, _T("添加"), _T("测试文本"), (UINT)0, true);
    m_toolbar.AddToolButton(theApp.m_icon_set.eq, _T("删除"), _T("测试文本1"), (UINT)0, true);
    m_toolbar.AddToolButton(theApp.m_icon_set.menu, _T("菜单"), _T("显示菜单"), theApp.m_menu_set.m_main_menu.GetSubMenu(0), true);
    m_toolbar.AddToolButton(theApp.m_icon_set.edit, nullptr, _T("显示菜单"), nullptr, true);

    //进度条
    m_progress_bar.SetProgress(18);
    m_progress_bar.SetBarCount(10);

    SetTimer(82373, 80, NULL);

    CString szFilter = CCommon::LoadText(IDS_SOUND_FONT_FILTER);
    m_browse_edit.EnableFileBrowseButton(_T("SF2"), szFilter);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CTestDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    if (nIDEvent == 82373)
    {
        m_timer_cnt+=4;
        m_progress_bar.SetProgress(m_timer_cnt % 100);
    }

    CDialog::OnTimer(nIDEvent);
}
