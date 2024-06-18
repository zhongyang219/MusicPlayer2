#pragma once
#include "BaseDialog.h"
#include "EditableListBox.h"

// CEditStringListDlg 对话框

class CEditStringListDlg : public CBaseDialog
{
    DECLARE_DYNAMIC(CEditStringListDlg)

public:
    CEditStringListDlg(vector<wstring>& items, CWnd* pParent = nullptr);   // 标准构造函数
    virtual ~CEditStringListDlg();

    void SetTitle(LPCTSTR title);
    void SetDlgIcon(IconMgr::IconType icon_type);

// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_SELECT_ITEM_DIALOG };
#endif

private:
    CEditableListBox m_list_ctrl;
    CString m_title;
    vector<wstring>& m_items;
    IconMgr::IconType m_icon_type{ IconMgr::IconType::IT_NO_ICON };

protected:
    virtual CString GetDialogName() const override;
    virtual bool InitializeControls() override;
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    virtual void OnOK();
};
