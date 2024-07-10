#pragma once
#include "BaseDialog.h"
#include "PlayerToolBar.h"
#include "PlayerProgressBar.h"
#include "BrowseEdit.h"
#include "ListCtrlEx.h"

// CTestDlg 对话框

class CTestDlg : public CBaseDialog
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
    virtual CString GetDialogName() const override;
    virtual bool InitializeControls() override;
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

    CListCtrlEx m_list_ctrl;

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    CBrowseEdit m_browse_edit;
    afx_msg void OnPaint();
};
