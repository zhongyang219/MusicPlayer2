#pragma once
#include "BaseDialog.h"

// CCoverPreviewDlg 对话框

class CCoverPreviewDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CCoverPreviewDlg)

public:
	CCoverPreviewDlg(std::wstring& file_path, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CCoverPreviewDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_COVER_PREVIEW_DIALOG };
#endif

protected:
	CImage m_cover_img;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();

	// 通过 CBaseDialog 继承
	CString GetDialogName() const override;
	virtual bool InitializeControls() override;
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
