#pragma once
#include "TagSelBaseDlg.h"

// CRenameDlg 对话框

class CRenameDlg : public CTagSelBaseDlg
{
    DECLARE_DYNAMIC(CRenameDlg)
public:
    CRenameDlg(int file_num, CWnd* pParent = nullptr);   // 标准构造函数
    virtual ~CRenameDlg();

protected:
    virtual CString GetDialogName() const;
    virtual bool InitializeControls() override;
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

private:
    int m_file_num;     //要重命名的文件数，用于显示在标题栏中

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
};

