#pragma once
#include "ListCtrlEx.h"
#include "BaseDialog.h"


// CMediaLibStatisticsDlg 对话框

class CMediaLibStatisticsDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CMediaLibStatisticsDlg)

public:
	CMediaLibStatisticsDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CMediaLibStatisticsDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SELECT_ITEM_DIALOG };
#endif

private:
    CListCtrlEx m_list_ctrl;

protected:
    enum RowIndex
    {
        RI_ARTIST,  //艺术家总数
        RI_ALBUM,   //唱片集总数
        RI_GENRE,   //流派总数
        RI_TOTAL,   //曲目总数
        RI_PLAYED,  //播放过曲目数
        RI_MAX
    };

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

    virtual CString GetDialogName() const override;

public:
    virtual BOOL OnInitDialog();
};
