#pragma once
#include "TabDlg.h"
#include "CListBoxEnhanced.h"


// CMediaLibSettingDlg 对话框

class CMediaLibSettingDlg : public CTabDlg
{
	DECLARE_DYNAMIC(CMediaLibSettingDlg)

public:
	CMediaLibSettingDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CMediaLibSettingDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MEDIA_LIB_SETTING_DIALOG };
#endif

    MediaLibSettingData m_data;

private:
    CButton m_classify_other_chk;
    CButton m_show_tree_tool_tips_chk;
    CListBoxEnhanced m_dir_list_ctrl;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedClassifyOtherCheck();
    afx_msg void OnBnClickedShowTreeToolTipsCheck();
    afx_msg void OnBnClickedAddButton();
    afx_msg void OnBnClickedDeleteButton();
};
