#pragma once
#include "ListCtrlEx.h"


// CListenTimeStatisticsDlg 对话框

class CListenTimeStatisticsDlg : public CDialog
{
	DECLARE_DYNAMIC(CListenTimeStatisticsDlg)

public:
	CListenTimeStatisticsDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CListenTimeStatisticsDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LISTEN_TIME_STATISTICS_DLG };
#endif

protected:
	CListCtrlEx m_list_ctrl;

	CSize m_min_size;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	void ShowData();

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedExportButton();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
    afx_msg void OnBnClickedClearButton();
};
