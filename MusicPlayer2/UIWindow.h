#pragma once
#include <afxwin.h>
#include "IPlayerUI.h"
class CUIWindow :
    public CStatic
{
public:
    CUIWindow(IPlayerUI*& pUI)
        : m_pUI(pUI)
    {}

    ~CUIWindow()
    {}

protected:
    IPlayerUI*& m_pUI;
    bool m_bTitlebarLButtonDown{};
    CPoint m_ptLButtonDown{};

    virtual void PreSubclassWindow();
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    DECLARE_MESSAGE_MAP()
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
//    afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
public:
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnPaint();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnMouseLeave();
};

