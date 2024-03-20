#pragma once
#include "BaseDialog.h"

// CInputDlg 对话框

class CInputDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CInputDlg)

public:
	CInputDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CInputDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INPUT_DLG };
#endif

    void SetTitle(LPCTSTR strTitle);
    void SetInfoText(LPCTSTR strInfo);
    void SetEditText(LPCTSTR strEdit);
    CString GetEditText() const;

protected:
    virtual CString GetDialogName() const override;
    virtual bool InitializeControls() override;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

private:
    CString m_strTitle;
    CString m_strInfo;
    CString m_strEdit;

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    virtual void OnOK();
};
