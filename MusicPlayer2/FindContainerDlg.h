#pragma once
#include "afxdialogex.h"
#include "CTabCtrlEx.h"
#include "FindDlg.h"

// CFindContainerDlg 对话框

class CFindContainerDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CFindContainerDlg)

public:
	CFindContainerDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CFindContainerDlg();

	void LoadChildrenConfig();
	void SaveChildrenConfig();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FIND_CONTAINER_DIALOG };
#endif

private:
	CTabCtrlEx m_tab_ctrl;
	CFindDlg m_find_song_dlg;
	int m_tab_selected{};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual CString GetDialogName() const override;
	virtual bool InitializeControls() override;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
protected:
	afx_msg LRESULT OnPlaySelectedBtnEnable(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnDestroy();
};
