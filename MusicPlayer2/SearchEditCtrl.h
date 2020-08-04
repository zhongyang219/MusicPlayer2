#pragma once
#include "ColorConvert.h"


// CSearchEditCtrl
#define WM_SEARCH_EDIT_BTN_CLICKED (WM_USER+125)

class CSearchEditCtrl : public CMFCEditBrowseCtrl
{
	DECLARE_DYNAMIC(CSearchEditCtrl)

public:
	CSearchEditCtrl();
	virtual ~CSearchEditCtrl();

    virtual void OnBrowse() override;
    virtual void OnDrawBrowseButton(CDC* pDC, CRect rect, BOOL bIsButtonPressed, BOOL bIsButtonHot) override;
    virtual void OnChangeLayout()override;

protected:
    void UpdateToolTipPosition();        //更新鼠标提示的位置

	DECLARE_MESSAGE_MAP()

private:
    ColorTable& m_theme_color;
    CToolTipCtrl m_tool_tip;
    bool m_draw_clear_btn{};        //如果为true，则显示“清除搜索结果”按钮，否则，显示“搜索”按钮

private:
    virtual void PreSubclassWindow();
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg BOOL OnEnChange();
};


