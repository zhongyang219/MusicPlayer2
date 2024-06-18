#pragma once
#include "BaseDialog.h"
#include "CListBoxEnhanced.h"

// CSelectItemDlg 对话框

class CSelectItemDlg : public CBaseDialog
{
    DECLARE_DYNAMIC(CSelectItemDlg)

public:
    CSelectItemDlg(const vector<wstring>& items, CWnd* pParent = nullptr);   // 标准构造函数
    virtual ~CSelectItemDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_SELECT_ITEM_DIALOG };
#endif

public:
    void SetTitle(LPCTSTR title);
    void SetDlgIcon(IconMgr::IconType icon_type);
    wstring GetSelectedItem() const;

private:
    CListBoxEnhanced m_list_ctrl;
    CString m_title;
    const vector<wstring>& m_items;
    int m_item_selected{ -1 };
    IconMgr::IconType m_icon_type{ IconMgr::IconType::IT_NO_ICON };

protected:
    virtual CString GetDialogName() const override;
    virtual bool InitializeControls() override;
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
};
