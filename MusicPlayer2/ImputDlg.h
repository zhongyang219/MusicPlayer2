#pragma once


// CInputDlg 对话框

class CImputDlg : public CDialog
{
	DECLARE_DYNAMIC(CImputDlg)

public:
	CImputDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CImputDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INPUT_DLG };
#endif

    void SetTitle(LPCTSTR strTitle);
    void SetInfoText(LPCTSTR strInfo);
    CString GetEditText() const;

protected:
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
