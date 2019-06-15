#pragma once
#include "CTabCtrlEx.h"
#include "CSelectPlaylist.h"


// CMediaLibDlg 对话框

class CMediaLibDlg : public CDialog
{
	DECLARE_DYNAMIC(CMediaLibDlg)

public:
	CMediaLibDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CMediaLibDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MEDIA_LIB_DIALOG };
#endif

public:
    CSetPathDlg m_path_dlg;
    CSelectPlaylist m_playlist_dlg;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
    CTabCtrlEx m_tab_ctrl;
    CSize m_min_size{};

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedPlaySelected();
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
};
