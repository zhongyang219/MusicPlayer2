#pragma once
#include "ListCtrlEx.h"

#define WM_LISTBOX_SEL_CHANGED (WM_USER+116)

class CListBoxEnhanced :
	public CListCtrlEx
{
public:
	CListBoxEnhanced();
	~CListBoxEnhanced();

	void AddString(LPCTSTR str);

protected:
	virtual void PreSubclassWindow();
    int CalculateColumnWidth();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnSize(UINT nType, int cx, int cy);
};

