// OpenUrlDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "afxdialogex.h"
#include "OpenUrlDlg.h"


// COpenUrlDlg 对话框

IMPLEMENT_DYNAMIC(COpenUrlDlg, CBaseDialog)

COpenUrlDlg::COpenUrlDlg(CWnd* pParent /*=nullptr*/)
	: CBaseDialog(IDD_OPEN_URL_DLG, pParent)
{

}

COpenUrlDlg::~COpenUrlDlg()
{
}

const CString& COpenUrlDlg::GetUrl() const
{
	return m_str_url;
}

const CString& COpenUrlDlg::GetName() const
{
	return m_str_name;
}

void COpenUrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
}

CString COpenUrlDlg::GetDialogName() const
{
	return _T("OpenUrlDlg");
}


BEGIN_MESSAGE_MAP(COpenUrlDlg, CBaseDialog)
END_MESSAGE_MAP()


// COpenUrlDlg 消息处理程序

void COpenUrlDlg::OnOK()
{
	GetDlgItemText(IDC_INPUT_NAME_EDIT, m_str_name);
	GetDlgItemText(IDC_INPUT_URL_EDIT, m_str_url);

	CBaseDialog::OnOK();
}
