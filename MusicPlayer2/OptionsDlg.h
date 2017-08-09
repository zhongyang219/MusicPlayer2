#pragma once
#include "afxcmn.h"
#include "PlaySettingsDlg.h"
#include "AppearanceSettingDlg.h"
#include "DataSettingsDlg.h"

// COptionsDlg 对话框
#define TIMER_ID2 1122

class COptionsDlg : public CDialog
{
	DECLARE_DYNAMIC(COptionsDlg)

public:
	COptionsDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~COptionsDlg();

	CPlaySettingsDlg m_tab1_dlg;	//“播放选项”子对话框
	CAppearanceSettingDlg m_tab2_dlg;	//“外观设置”子对话框
	CDataSettingsDlg m_tab3_dlg;		//“数据文件”子对话框

	int m_tab_selected{};

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OPTIONS_DIALOG };
#endif

protected:

	CTabCtrl m_tab;		//选项卡控件

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTcnSelchangeOptionsTab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
