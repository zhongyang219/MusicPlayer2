#pragma once
#include <afxwin.h>
class CMainDialogBase :
	public CDialog
{
	DECLARE_DYNAMIC(CMainDialogBase)
public:
	CMainDialogBase(UINT nIDTemplate, CWnd *pParent = NULL);
	~CMainDialogBase();

protected:
	void SetFullScreen(bool full_screen);		//设置窗口全屏(来自:https://www.cnblogs.com/weixinhum/p/3916673.html)

private:
	bool m_bFullScreen{ false };
	CRect m_rectFullScreen;
	WINDOWPLACEMENT m_struOldWndpl;//结构中包含了有关窗口在屏幕上位置的信息

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
};

