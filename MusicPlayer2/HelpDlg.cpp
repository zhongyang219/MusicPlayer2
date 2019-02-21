// HelpDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "HelpDlg.h"
#include "afxdialogex.h"


// CHelpDlg 对话框

IMPLEMENT_DYNAMIC(CHelpDlg, CDialog)

CHelpDlg::CHelpDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_HELP_DIALOG, pParent)
{

}

CHelpDlg::~CHelpDlg()
{
}

void CHelpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HELP_EDIT, m_help_edit);
}


BEGIN_MESSAGE_MAP(CHelpDlg, CDialog)
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()


// CHelpDlg 消息处理程序


BOOL CHelpDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//help_info.LoadString(IDS_HELP_INFO);

	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);		// 设置小图标

	//获取初始时窗口的大小
	CRect rect;
	GetWindowRect(rect);
	m_min_size.cx = rect.Width();
	m_min_size.cy = rect.Height();

	m_help_edit.SetWindowText(GetHelpString());

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

CString CHelpDlg::GetHelpString()
{
	CString help_info;
	HRSRC hRes;
	if(theApp.m_general_setting_data.language == Language::FOLLOWING_SYSTEM)
		hRes = FindResource(NULL, MAKEINTRESOURCE(IDR_TEXT1), _T("TEXT"));
	else
		hRes = FindResourceEx(NULL, _T("TEXT"), MAKEINTRESOURCE(IDR_TEXT1), theApp.GetCurrentLanguage());
	if (hRes != NULL)
	{
		HGLOBAL hglobal = LoadResource(NULL, hRes);
		if (hglobal != NULL)
			help_info.Format(_T("%s"), (LPVOID)hglobal);
	}

	//在帮助信息后面增加系统信息
	help_info += _T("\r\n\r\nSystem Info:\r\n");

	CString strTmp;
	strTmp.Format(_T("Windows Version: %d.%d build %d\r\n"), CWinVersionHelper::GetMajorVersion(),
		CWinVersionHelper::GetMinorVersion(), CWinVersionHelper::GetBuildNumber());
	help_info += strTmp;

	strTmp.Format(_T("DPI: %d"), theApp.m_dpi);
	help_info += strTmp;
	help_info += _T("\r\n");

	return help_info;
}

void CHelpDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//限制窗口最小大小
	lpMMI->ptMinTrackSize.x = m_min_size.cx;		//设置最小宽度
	lpMMI->ptMinTrackSize.y = m_min_size.cy;		//设置最小高度

	CDialog::OnGetMinMaxInfo(lpMMI);
}
