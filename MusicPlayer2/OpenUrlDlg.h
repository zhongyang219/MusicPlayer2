#pragma once
#include "BaseDialog.h"

// COpenUrlDlg 对话框

class COpenUrlDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(COpenUrlDlg)

public:
	COpenUrlDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~COpenUrlDlg();

	const CString& GetUrl() const;
	const CString& GetName() const;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OPEN_URL_DLG };
#endif

private:
	CString m_str_url;
	CString m_str_name;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	// 通过 CBaseDialog 继承
	CString GetDialogName() const override;
	virtual void OnOK();
};
