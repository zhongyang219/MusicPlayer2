#pragma once
#include "ListCtrlEx.h"
#include "BaseDialog.h"

// CSupportedFormatDlg 对话框

class CSupportedFormatDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CSupportedFormatDlg)

public:
	CSupportedFormatDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CSupportedFormatDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SUPPORT_FORMAT_DIALOG };
#endif

protected:
	CListCtrlEx m_format_list;

    virtual CString GetDialogName() const override;
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
};
