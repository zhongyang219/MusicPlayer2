#pragma once
#include "TabDlg.h"


// CSelectPlaylist 对话框

class CSelectPlaylist : public CTabDlg
{
	DECLARE_DYNAMIC(CSelectPlaylist)

public:
	CSelectPlaylist(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CSelectPlaylist();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SELECT_PLAYLIST_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
