#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CEqualizerDlg 对话框

class CEqualizerDlg : public CDialog
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
	int m_user_defined_gain[FX_CH_NUM]{};		//用户自定义的均衡器设置

	CSliderCtrl m_sliders[FX_CH_NUM];
	CListBox m_equ_style_list;
	CButton m_enable_equ_check;
	CToolTipCtrl m_Mytip;		//鼠标提示
	int m_equ_style_selected{};		//选中的均衡器预设

	void EnableControls(bool enable);		//启用或禁用控件
	void SaveConfig() const;
	void LoadConfig();
	void UpdateChannelTip(int channel, int gain);		//更新通道滑动条的鼠标提示

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedEnableEquCheck();
	afx_msg void OnLbnSelchangeEquStylesList();
	afx_msg void OnDestroy();
};
