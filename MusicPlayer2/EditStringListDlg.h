﻿#pragma once
#include "EditableListBox.h"
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
    CEditableListBox m_list_ctrl;
    CString m_title;
    vector<wstring>& m_items;
    HICON m_icon{};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

    virtual CString GetDialogName() const override;

public:
    virtual BOOL OnInitDialog();
    virtual void OnOK();
};
