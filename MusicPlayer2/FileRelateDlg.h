#pragma once
#include "BaseDialog.h"


// CFileRelateDlg 对话框

class CFileRelateDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CFileRelateDlg)

public:
	CFileRelateDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CFileRelateDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILE_RELATE_DIALOG };
#endif

protected:
    virtual CString GetDialogName() const override;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    void RefreshList();

	DECLARE_MESSAGE_MAP()
public:
    CListCtrlEx m_list_ctrl;
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnBnClickedSelectAllCheck();
    afx_msg void OnBnClickedDefaultButton();
};
