#pragma once
#include "TagSelBaseDlg.h"

// CFileNameFormDlg 对话框

class CFileNameFormDlg : public CTagSelBaseDlg
{
    DECLARE_DYNAMIC(CFileNameFormDlg)
public:
    CFileNameFormDlg(CWnd* pParent = nullptr);   // 标准构造函数
    virtual ~CFileNameFormDlg();

protected:
    virtual CString GetDialogName() const;
    virtual bool InitializeControls() override;
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
};


