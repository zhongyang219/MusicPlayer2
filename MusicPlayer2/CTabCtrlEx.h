#pragma once
#include "IconMgr.h"

// CTabCtrlEx

class CTabCtrlEx : public CTabCtrl
{
	DECLARE_DYNAMIC(CTabCtrlEx)

public:
	CTabCtrlEx();
	virtual ~CTabCtrlEx();

    void AddWindow(CWnd* pWnd, LPCTSTR lable_text, IconMgr::IconType icon_type);    //向当前tab控件添加一个子窗口
	void SetCurTab(int index);
    CWnd* GetCurrentTab();
    // 调整所有标签页大小兼执行设置图标
    void AdjustTabWindowSize();

protected:
    void CalSubWindowSize();

	DECLARE_MESSAGE_MAP()

protected:
	vector<CWnd*> m_tab_list;		//保存tab控件每个子窗口的指针
    vector<IconMgr::IconType> m_icon_list;
    int m_last_tab_index{ -1 };
public:
	afx_msg void OnTcnSelchange(NMHDR *pNMHDR, LRESULT *pResult);
	virtual void PreSubclassWindow();

	CRect m_tab_rect;
    afx_msg void OnSize(UINT nType, int cx, int cy);
};


