#pragma once


// CRecorderDlg 对话框

class CRecorderDlg : public CDialog
{
	DECLARE_DYNAMIC(CRecorderDlg)

public:
	CRecorderDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CRecorderDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RECORDER_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
