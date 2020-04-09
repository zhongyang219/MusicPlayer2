#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CReverbDlg 对话框

class CReverbDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CReverbDlg)

public:
	CReverbDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CReverbDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REVERB_DIALOG };
#endif

protected:
	CSliderCtrl m_reverb_mix_slider;
	CSliderCtrl m_reverb_time_slider;

	void EnableControls(bool enable);		//启用或禁用控件
	void UpdateStaticText(int mix, int time);		//更新静态控件上的文本

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CButton m_enable_reverb_chk;
	afx_msg void OnBnClickedEnableReverbCheck();
};
