#pragma once
#include "TagSelBaseDlg.h"

// CTagFromNameDlg 对话框

class CTagFromNameDlg : public CTagSelBaseDlg
{
    DECLARE_DYNAMIC(CTagFromNameDlg)
public:
    CTagFromNameDlg(CWnd* pParent = nullptr);   // 标准构造函数
    virtual ~CTagFromNameDlg();

protected:
    virtual CString GetDialogName() const;
    virtual bool InitializeControls() override;
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
};

