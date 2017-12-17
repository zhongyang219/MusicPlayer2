#pragma once
#include "afxcmn.h"
#include "ColorConvert.h"
class CListCtrlEx :
	public CListCtrl
{
	DECLARE_DYNAMIC(CListCtrlEx)
public:
	CListCtrlEx();
	~CListCtrlEx();

	void SetColor(const ColorTable& colors);
	void SetHightItem(int item) { m_highlight_item = item; }					//设置高亮的项目，即播放列表中正在播放的项目

protected:
	ColorTable m_theme_color;
	COLORREF m_background_color{ GRAY(255) };	//列表控件的背景色
	int m_highlight_item{};			//高亮的项目

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	virtual void PreSubclassWindow();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};

