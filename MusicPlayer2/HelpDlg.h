#pragma once
#include "afxwin.h"


// CHelpDlg 对话框

class CHelpDlg : public CDialog
{
	DECLARE_DYNAMIC(CHelpDlg)

public:
	CHelpDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CHelpDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HELP_DIALOG };
#endif

protected:
	CEdit m_help_edit;
	CString m_help_info;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	void GetHelpString();
};
