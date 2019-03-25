#pragma once
#include "CHotkeyManager.h"
#include "TabDlg.h"


// CHotKeySettingDlg 对话框

class CHotKeySettingDlg : public CTabDlg
{
	DECLARE_DYNAMIC(CHotKeySettingDlg)

public:
	CHotKeySettingDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CHotKeySettingDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HOT_KEY_SETTINGS_DIALOG };
#endif

public:
	std::map<eHotKeyId, CHotKey> m_hotkey_group;
	GlobalHotKeySettingData m_data;

private:
	CListCtrlEx m_key_list;
	CButton m_hot_key_enable_check;
	CButton m_enable_global_multimedia_key_check;
	int m_item_selected{ -1 };
	CToolTipCtrl m_toolTip;

protected:
	void ShowKeyList();
	void EnableControl();
	void ListClicked();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CHotKeyCtrl m_hot_key_ctrl;
	afx_msg void OnBnClickedSetButton();
	afx_msg void OnNMClickHotKeyList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickHotKeyList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedHotKeyEnableCheck();
	afx_msg void OnBnClickedEnableGlabolMultimediaKeyCheck();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
