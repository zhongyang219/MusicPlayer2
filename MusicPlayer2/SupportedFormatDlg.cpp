// SupportedFormatDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "SupportedFormatDlg.h"
#include "afxdialogex.h"
#include "AudioCommon.h"


// CSupportedFormatDlg 对话框

IMPLEMENT_DYNAMIC(CSupportedFormatDlg, CBaseDialog)

CSupportedFormatDlg::CSupportedFormatDlg(CWnd* pParent /*=nullptr*/)
	: CBaseDialog(IDD_SUPPORT_FORMAT_DIALOG, pParent)
{

}

CSupportedFormatDlg::~CSupportedFormatDlg()
{
}

CString CSupportedFormatDlg::GetDialogName() const
{
    return _T("SupportedFormatDlg");
}

void CSupportedFormatDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FORMAT_LIST, m_format_list);
}


BEGIN_MESSAGE_MAP(CSupportedFormatDlg, CBaseDialog)
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()


// CSupportedFormatDlg 消息处理程序


BOOL CSupportedFormatDlg::OnInitDialog()
{
	CBaseDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);		// 设置小图标

	//初始化列表
	//m_format_list.SetColor(theApp.m_app_setting_data.theme_color);
	CRect rect;
	m_format_list.GetWindowRect(rect);
	int width0, width1, width2;
	width0 = theApp.DPI(100);
	width1 = rect.Width() / 3;
	width2 = rect.Width() - width1 - width0 - theApp.DPI(20) - 1;

    m_format_list.SetExtendedStyle(m_format_list.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_LABELTIP);
	m_format_list.InsertColumn(0, CCommon::LoadText(IDS_PLUGIN_FILE_NAME), LVCFMT_LEFT, width0);		//插入第1列
	m_format_list.InsertColumn(1, CCommon::LoadText(IDS_FORMAT_SUPPORTED), LVCFMT_LEFT, width1);		//插入第2列
	m_format_list.InsertColumn(2, CCommon::LoadText(IDS_FILE_EXTENSION), LVCFMT_LEFT, width2);		//插入第2列

	int index = 0;
	for (const auto support_format : CAudioCommon::m_surpported_format)
	{
		if (!support_format.file_name.empty())
		{
			m_format_list.InsertItem(index, support_format.file_name.c_str());
			m_format_list.SetItemText(index, 1, support_format.description.c_str());
			m_format_list.SetItemText(index, 2, support_format.extensions_list.c_str());

			index++;
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
