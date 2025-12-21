#pragma once
#include "BaseDialog.h"


// CClosseMainWindowInqueryDlg 对话框

class CClosseMainWindowInqueryDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CClosseMainWindowInqueryDlg)

public:
	CClosseMainWindowInqueryDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CClosseMainWindowInqueryDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLOSE_WINDOW_INQUERY_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	// 通过 CBaseDialog 继承
	virtual CString GetDialogName() const override;
	virtual bool InitializeControls() override;

public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};
