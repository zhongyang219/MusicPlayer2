#pragma once
#include "afxwin.h"

// CMessageDlg 对话框

class CMessageDlg : public CDialog
{
	DECLARE_DYNAMIC(CMessageDlg)

public:
	CMessageDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMessageDlg();

	void SetWindowTitle(LPCTSTR str);
	void SetInfoText(LPCTSTR str);
	void SetMessageText(LPCTSTR str);
	void ShowLinkStatic(bool show = true) { m_show_link_ctrl = show; }
	void SetLinkInfo(LPCTSTR text, LPCTSTR url);

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MESSAGE_DIALOG };
#endif

protected:
	CEdit m_message_edit;
	CSize m_min_size;		//窗口的最小大小
	CStatic m_info_static;

	CString m_title;
	CString m_info;
	CString m_message;

	CString m_link_text;
	CString m_link_url;

	bool m_show_link_ctrl{ false };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnNMClickSyslink1(NMHDR *pNMHDR, LRESULT *pResult);
};
