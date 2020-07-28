#pragma once
#include "PlayerToolBar.h"
#include "PlayerProgressBar.h"
#include "BrowseEdit.h"

// CTestDlg 对话框

class CTestDlg : public CDialog
{
	DECLARE_DYNAMIC(CTestDlg)

public:
	CTestDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CTestDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TEST_DIALOG };
#endif


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
    CPlayerToolBar m_toolbar;
    CPlayerProgressBar m_progress_bar;

    CImage m_image;
    Gdiplus::Image* m_pImage;

    int m_timer_cnt{};

    CBitmap bitmap;
    CBitmap bitmap_copy;


public:
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    CBrowseEdit m_browse_edit;
    afx_msg void OnPaint();
};
