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

	GetHelpString();
	wstring help_info{ m_help_info.GetString() };

	//在帮助文档中添加支持的音频格式
	wstring str_support_format;
	str_support_format += L"【支持的音频格式】\r\n";
	str_support_format += L"更多音频格式依赖于BASS引擎插件（位于\".\\Plugins\"目录下）。\r\n";
	for (const auto support_format : CAudioCommon::m_surpported_format)
	{
		str_support_format += support_format.description;
		str_support_format += L": ";
		str_support_format += support_format.extensions_list;
		str_support_format += L"\r\n";
	}
	str_support_format += L"\r\n";
	size_t index = help_info.find(L"【操作*说明】");		//添加在【操作说明】之前
	if (index != wstring::npos)
	{
		help_info.insert(index, str_support_format);
	}
	else
	{
		help_info += L"\r\n";
		help_info += str_support_format;
	}

	m_help_edit.SetWindowText(help_info.c_str());
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CHelpDlg::GetHelpString()
{
	HRSRC hRes = FindResource(NULL,MAKEINTRESOURCE(IDR_TEXT1), _T("Text"));
	if (hRes != NULL)
	{
		HGLOBAL hglobal = LoadResource(NULL, hRes);
		if (hglobal != NULL)
			m_help_info.Format(_T("%s"), (LPVOID)hglobal);
	}
}

void CHelpDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//限制窗口最小大小
	lpMMI->ptMinTrackSize.x = m_min_size.cx;		//设置最小宽度
	lpMMI->ptMinTrackSize.y = m_min_size.cy;		//设置最小高度

	CDialog::OnGetMinMaxInfo(lpMMI);
}
