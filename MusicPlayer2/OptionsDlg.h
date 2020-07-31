#pragma once
#include "afxcmn.h"
#include "LyricSettingsDlg.h"
#include "AppearanceSettingDlg.h"
#include "DataSettingsDlg.h"
#include "PlaySettingsDlg.h"
#include "CTabCtrlEx.h"
#include "CHotKeySettingDlg.h"
#include "MediaLibSettingDlg.h"
#include "BaseDialog.h"

// COptionsDlg 对话框
#define TIMER_ID2 1122
#define WM_SETTINGS_APPLIED (WM_USER+113)

class COptionsDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(COptionsDlg)

public:
	COptionsDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~COptionsDlg();

	CLyricSettingsDlg m_tab1_dlg{ this };		//“歌词设置”子对话框
	CAppearanceSettingDlg m_tab2_dlg{ this };	//“外观设置”子对话框
	CDataSettingsDlg m_tab3_dlg{ this };		//“常规设置”子对话框
	CPlaySettingsDlg m_tab4_dlg{ this };		//“播放选项”子对话框
	CHotKeySettingDlg m_tab5_dlg{ this };		//“全局快捷键”子对话框
	CMediaLibSettingDlg m_media_lib_dlg{ this };		//“媒体库”子对话框

	int m_tab_selected{};

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OPTIONS_DIALOG };
#endif

protected:

	CTabCtrlEx m_tab;		//选项卡控件
    std::vector<CTabDlg*> m_tab_vect;
    std::vector<int> m_tab_height;

    virtual CString GetDialogName() const override;
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBnClickedApplyButton();
	afx_msg void OnDestroy();
    afx_msg void OnSize(UINT nType, int cx, int cy);
};
