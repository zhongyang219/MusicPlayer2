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
}


BEGIN_MESSAGE_MAP(CTestDlg, CDialog)
END_MESSAGE_MAP()


// CTestDlg 消息处理程序


BOOL CTestDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    m_toolbar.SetIconSize(theApp.DPI(20));
    m_toolbar.AddToolButton(theApp.m_icon_set.media_lib, _T("添加"), _T("打开文件"), ID_FILE_OPEN, false);
    m_toolbar.AddToolButton(theApp.m_icon_set.setting, _T("添加"), _T("测试文本"), (UINT)0, true);
    m_toolbar.AddToolButton(theApp.m_icon_set.eq, _T("删除"), _T("测试文本1"), (UINT)0, true);
    m_toolbar.AddToolButton(theApp.m_icon_set.menu, _T("菜单"), _T("显示菜单"), theApp.m_menu_set.m_main_menu.GetSubMenu(0), true);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}
