#pragma once
#include "CListBoxEnhanced.h"
#include "BaseDialog.h"

// CEditStringListDlg 对话框

class CEditStringListDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CEditStringListDlg)

public:
	CEditStringListDlg(vector<wstring>& items, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CEditStringListDlg();

    void SetTitle(LPCTSTR title);
    void SetDlgIcon(HICON icon);

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SELECT_ITEM_DIALOG };
#endif

private:
    CListBoxEnhanced m_list_ctrl;
    CString m_title;
    vector<wstring>& m_items;
    HICON m_icon{};
    CEdit m_item_edit;
    int m_edit_row{};
    bool m_editing{};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

    afx_msg void OnEnKillfocusEdit1();
    afx_msg void OnNMDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnLvnBeginScrollList1(NMHDR* pNMHDR, LRESULT* pResult);

    virtual CString GetDialogName() const override;

    void Edit(int row);     //编辑指定行
    void EndEdit();         //结束编辑

public:
    virtual BOOL OnInitDialog();
    virtual void OnOK();
};
