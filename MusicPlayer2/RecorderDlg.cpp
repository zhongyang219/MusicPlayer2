// CRecorderDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "RecorderDlg.h"
#include "afxdialogex.h"


// CRecorderDlg 对话框

IMPLEMENT_DYNAMIC(CRecorderDlg, CDialog)

CRecorderDlg::CRecorderDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_RECORDER_DIALOG, pParent)
{

}

CRecorderDlg::~CRecorderDlg()
{
}

void CRecorderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRecorderDlg, CDialog)
END_MESSAGE_MAP()


// CRecorderDlg 消息处理程序


BOOL CRecorderDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
