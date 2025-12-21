// ClosseMainWindowInqueryDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "ClosseMainWindowInqueryDlg.h"


// CClosseMainWindowInqueryDlg 对话框

IMPLEMENT_DYNAMIC(CClosseMainWindowInqueryDlg, CBaseDialog)

CClosseMainWindowInqueryDlg::CClosseMainWindowInqueryDlg(CWnd* pParent /*=nullptr*/)
	: CBaseDialog(IDD_CLOSE_WINDOW_INQUERY_DIALOG, pParent)
{

}

CClosseMainWindowInqueryDlg::~CClosseMainWindowInqueryDlg()
{
}

void CClosseMainWindowInqueryDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
}

CString CClosseMainWindowInqueryDlg::GetDialogName() const
{
	return _T("ClosseMainWindowInqueryDlg");
}

bool CClosseMainWindowInqueryDlg::InitializeControls()
{
	SetWindowText(theApp.m_str_table.LoadText(L"TITLE_CLOSE_MAIN_WINDOW_INQUERY_DLG").c_str());
	SetDlgControlText(IDC_CLOSE_MAIN_WINDOW_INQUERY_INFO_STATIC, L"TXT_CLOSE_MAIN_WINDOW_DLG_INFO");
	SetDlgControlText(IDC_MINIMIZE_TO_NOTIFY_RADIO, L"TXT_OPT_DATA_CLOSE_MAIN_WINDOW_MINIMIZE_NOTIFY_AREA");
	SetDlgControlText(IDC_EXIT_PROGRAM_RADIO, L"TXT_OPT_DATA_CLOSE_MAIN_WINDOW_EXIT");
	SetDlgControlText(IDC_NO_LONGER_ASK_CHECK, L"TXT_CLOSE_MAIN_WINDOW_DLG_NO_LONGER_ASK");
	return true;
}


BEGIN_MESSAGE_MAP(CClosseMainWindowInqueryDlg, CBaseDialog)
END_MESSAGE_MAP()


// CClosseMainWindowInqueryDlg 消息处理程序


BOOL CClosseMainWindowInqueryDlg::OnInitDialog()
{
	CBaseDialog::OnInitDialog();

	SetIcon(IconMgr::IconType::IT_App, FALSE);

	//初始化控件
	if (theApp.m_general_setting_data.minimize_to_notify_icon)
		CheckDlgButton(IDC_MINIMIZE_TO_NOTIFY_RADIO, TRUE);
	else
		CheckDlgButton(IDC_EXIT_PROGRAM_RADIO, TRUE);
	CheckDlgButton(IDC_NO_LONGER_ASK_CHECK, !theApp.m_nc_setting_data.show_close_main_window_inquery);


	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CClosseMainWindowInqueryDlg::OnOK()
{
	theApp.m_general_setting_data.minimize_to_notify_icon = IsDlgButtonChecked(IDC_MINIMIZE_TO_NOTIFY_RADIO) != FALSE;
	theApp.m_nc_setting_data.show_close_main_window_inquery = IsDlgButtonChecked(IDC_NO_LONGER_ASK_CHECK) == FALSE;

	CBaseDialog::OnOK();
}
