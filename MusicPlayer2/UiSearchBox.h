#pragma once
#include "afxdialogex.h"
#include "SearchEditCtrl.h"

// CUiSearchBox 对话框

class CUiSearchBox : public CDialog
{
	DECLARE_DYNAMIC(CUiSearchBox)

public:
	CUiSearchBox(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CUiSearchBox();
    void MainWindowMoved(CWnd* pWnd);
    void SetRelativePos(CPoint pos);

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UI_SEARCH_BOX_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    CSearchEditCtrl m_search_box;
    virtual void OnCancel();

private:
    CPoint m_relative_pos;         //搜索框以主窗口左上角为原点的相对位置
public:
    afx_msg void OnEnKillfocusUiSearchBoxEdit();
};
