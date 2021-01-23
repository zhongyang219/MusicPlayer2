#pragma once
#include "CListBoxEnhanced.h"
#include "BaseDialog.h"

// CSelectItemDlg 对话框

class CSelectItemDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CSelectItemDlg)

public:
	CSelectItemDlg(const vector<wstring>& items, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CSelectItemDlg();

    void SetTitle(LPCTSTR title);
    void SetDlgIcon(HICON icon);

    wstring GetSelectedItem() const;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SELECT_ITEM_DIALOG };
#endif

private:
    CListBoxEnhanced m_list_ctrl;
    CString m_title;
    const vector<wstring>& m_items;
    int m_item_selected{ -1 };
    HICON m_icon{};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

    virtual CString GetDialogName() const override;

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
};
