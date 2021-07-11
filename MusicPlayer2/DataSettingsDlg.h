#pragma once
#include"Common.h"
#include "TabDlg.h"
#include "BrowseEdit.h"
#include "MyComboBox.h"

// CDataSettingsDlg 对话框

class CDataSettingsDlg : public CTabDlg
{
	DECLARE_DYNAMIC(CDataSettingsDlg)

public:
	CDataSettingsDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDataSettingsDlg();

    bool IsAutoRunModified() const;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DATA_SETTINGS_DIALOG };
#endif

	GeneralSettingData m_data;
    bool m_auto_run{};

private:
	CToolTipCtrl m_toolTip;
	CBrowseEdit m_sf2_path_edit;
	CMyComboBox m_language_combo;
    bool m_auto_run_modified{};

    void EnableControl();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	//afx_msg void OnBnClickedId3v2FirstCheck();
	afx_msg void OnBnClickedCoverAutoDownloadCheck();
	afx_msg void OnBnClickedLyricAutoDownloadCheck();
	afx_msg void OnBnClickedCheckUpdateCheck();
	//afx_msg void OnBnClickedBrowseButton();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedMidiUseInnerLyricCheck();
	afx_msg void OnBnClickedDownloadWhenTagFullCheck();
	afx_msg void OnEnChangeSf2PathEdit();
	virtual void OnOK();
protected:
    afx_msg LRESULT OnEditBrowseChanged(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnBnClickedAutoRunCheck();
    afx_msg void OnBnClickedGithubRadio();
    afx_msg void OnBnClickedGiteeRadio();
};
