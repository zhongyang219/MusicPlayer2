// InputDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "InputDlg.h"
#include "afxdialogex.h"


// CInputDlg 对话框

IMPLEMENT_DYNAMIC(CInputDlg, CDialog)

CInputDlg::CInputDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_INPUT_DLG, pParent)
{

}

CInputDlg::~CInputDlg()
{
}

void CInputDlg::SetTitle(LPCTSTR strTitle)
{
    m_strTitle = strTitle;
}

void CInputDlg::SetInfoText(LPCTSTR strInfo)
{
    m_strInfo = strInfo;
}

void CInputDlg::SetEditText(LPCTSTR strEdit)
{
    m_strEdit = strEdit;
}

CString CInputDlg::GetEditText() const
{
    return m_strEdit;
}

void CInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CInputDlg, CDialog)
END_MESSAGE_MAP()


// CInputDlg 消息处理程序


BOOL CInputDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    SetWindowText(m_strTitle);
    SetDlgItemText(IDC_INFO_STATIC, m_strInfo);

    CEdit* pEdit = (CEdit*)GetDlgItem(IDC_INPUT_EDIT);
    if(pEdit!=nullptr)
    {
        pEdit->SetWindowText(m_strEdit);
        pEdit->SetFocus();		//初始时将焦点设置到输入框
        pEdit->SetSel(0, -1);
    }

    return FALSE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CInputDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类
    GetDlgItemText(IDC_INPUT_EDIT, m_strEdit);

    CDialog::OnOK();
}
