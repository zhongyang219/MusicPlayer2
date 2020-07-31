#pragma once
#include "BaseDialog.h"
#include "ListCtrlEx.h"


// CAlbumCoverInfoDlg 对话框

class CAlbumCoverInfoDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CAlbumCoverInfoDlg)

public:
	CAlbumCoverInfoDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CAlbumCoverInfoDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ADD_TO_PLAYLIST_DIALOG };
#endif

private:
    CListCtrlEx m_list_ctrl;

protected:
    virtual CString GetDialogName() const override;
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
};
