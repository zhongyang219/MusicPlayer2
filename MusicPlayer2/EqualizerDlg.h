#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "CListBoxEnhanced.h"


// CEqualizerDlg 对话框

class CEqualizerDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CEqualizerDlg)

public:
	CEqualizerDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CEqualizerDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EQUALIZER_DIALOG };
#endif

protected:
	int m_user_defined_gain[EQU_CH_NUM]{};		//用户自定义的均衡器设置

	CSliderCtrl m_sliders[EQU_CH_NUM];
	CListBoxEnhanced m_equ_style_list;
	CButton m_enable_equ_check;
	CToolTipCtrl m_Mytip;		//鼠标提示
	int m_equ_style_selected{};		//选中的均衡器预设

	void EnableControls(bool enable);		//启用或禁用控件
	void SaveConfig() const;
	void LoadConfig();
	void UpdateChannelTip(int channel, int gain);		//更新通道滑动条的鼠标提示

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedEnableEquCheck();
	afx_msg LRESULT OnListboxSelChanged(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
