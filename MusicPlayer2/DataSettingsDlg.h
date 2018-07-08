#pragma once
#include"Common.h"

// CDataSettingsDlg 对话框

class CDataSettingsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDataSettingsDlg)

public:
	CDataSettingsDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDataSettingsDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DATA_SETTINGS_DIALOG };
#endif

	GeneralSettingData m_data;

private:
	size_t m_data_size;		//数据文件的大小

	void ShowDataSizeInfo();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCleanDataFileButton();
	afx_msg void OnBnClickedId3v2FirstCheck();
	afx_msg void OnBnClickedCoverAutoDownloadCheck();
	afx_msg void OnBnClickedLyricAutoDownloadCheck();
	afx_msg void OnBnClickedCheckUpdateCheck();
};
