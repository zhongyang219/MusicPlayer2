#pragma once
#include "UserUi.h"


// CUIDialog 对话框

class CUIDialog : public CDialog
{
    DECLARE_DYNAMIC(CUIDialog)

public:
    CUIDialog(UINT ui_res_id, CWnd* pParent = nullptr);   // 标准构造函数
    virtual ~CUIDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_UI_DIALOG };
#endif

    void RePaintUi();

protected:
    CUserUi m_ui;
    UIData m_ui_data;
    CDC* m_pDC{};
    CSize m_min_size{};
    CEdit m_ui_edit;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnPaint();
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnMouseLeave();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnDestroy();
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnEnChangeUiEdit();
};
