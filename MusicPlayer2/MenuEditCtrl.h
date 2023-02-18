#pragma once
#include "ColorConvert.h"


// CMenuEditCtrl

class CMenuEditCtrl : public CMFCEditBrowseCtrl
{
    DECLARE_DYNAMIC(CMenuEditCtrl)

public:
    CMenuEditCtrl();
    virtual ~CMenuEditCtrl();
    void SetTooltopText(const CString& tooltip_text);       //设置按钮上鼠标提示的文本

    virtual void OnBrowse() override;
    virtual void OnDrawBrowseButton(CDC* pDC, CRect rect, BOOL bIsButtonPressed, BOOL bIsButtonHot) override;
    virtual void OnChangeLayout()override;

protected:
    void UpdateToolTipPosition();        //更新鼠标提示的位置
    
    DECLARE_MESSAGE_MAP()

private:
    ColorTable& m_theme_color;
    CToolTipCtrl m_tool_tip;
    CString m_tooltip_text;     //按钮上鼠标提示的文本
    CRect m_btn_rect;           //按钮的矩形区域

public:
    afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
protected:
    afx_msg LRESULT OnTabletQuerysystemgesturestatus(WPARAM wParam, LPARAM lParam);
    virtual void PreSubclassWindow();
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
};
