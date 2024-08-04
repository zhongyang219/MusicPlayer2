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
    CString GetItemText(int index);
    virtual bool DeleteItem(int nItem) override;

protected:
	virtual void PreSubclassWindow();
    int CalculateColumnWidth();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult);       //这里函数的返回值必须为BOOL，消息映射的宏使用ON_NOTIFY_REFLECT_EX，函数返回FALSE，这样派生类才能正常响应此消息
    afx_msg void OnSize(UINT nType, int cx, int cy);
};

