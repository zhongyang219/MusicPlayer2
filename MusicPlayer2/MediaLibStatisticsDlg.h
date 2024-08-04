#pragma once
#include "SimplePropertiesDlg.h"
#include "ListCtrlEx.h"


// CMediaLibStatisticsDlg 对话框

class CMediaLibStatisticsDlg : public CSimplePropertiesDlg
{
    DECLARE_DYNAMIC(CMediaLibStatisticsDlg)

public:
    CMediaLibStatisticsDlg(CWnd* pParent = nullptr);   // 标准构造函数
    virtual ~CMediaLibStatisticsDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_SELECT_ITEM_DIALOG };
#endif

protected:
    virtual CString GetDialogName() const override;

    DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();

protected:
    virtual void InitData() override;
};
