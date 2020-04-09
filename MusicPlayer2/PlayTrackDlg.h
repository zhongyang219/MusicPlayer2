#pragma once
#include "afxwin.h"


// CPlayTrackDlg 对话框

class CPlayTrackDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPlayTrackDlg)

public:
	CPlayTrackDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPlayTrackDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PLAY_TRACK_DIALOG };
#endif

protected:
	int m_track;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_track_edit;

	int GetTrack() const;
	afx_msg void OnEnChangeEditTrack();
	virtual BOOL OnInitDialog();
};
