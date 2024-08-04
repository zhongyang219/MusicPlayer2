#pragma once
#include "SimplePropertiesDlg.h"
#include "ListCtrlEx.h"
#include "RecentFolderMgr.h"

// CFolderPropertiesDlg 对话框

class CFolderPropertiesDlg : public CSimplePropertiesDlg
{
    DECLARE_DYNAMIC(CFolderPropertiesDlg)

public:
    CFolderPropertiesDlg(const PathInfo& folder_info, CWnd* pParent = nullptr);   // 标准构造函数
    virtual ~CFolderPropertiesDlg();

private:
    PathInfo m_folder_info;

protected:
    virtual CString GetDialogName() const override;

    DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();

protected:
    virtual void InitData() override;
};
