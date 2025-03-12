#pragma once
#include "SimplePropertiesDlg.h"
#include "ListItem.h"

// CMediaLibItemPropertiesDlg 对话框

class CMediaLibItemPropertiesDlg : public CSimplePropertiesDlg
{
    DECLARE_DYNAMIC(CMediaLibItemPropertiesDlg)

public:
    CMediaLibItemPropertiesDlg(const ListItem& item_info, CWnd* pParent = nullptr);   // 标准构造函数
    virtual ~CMediaLibItemPropertiesDlg();

private:
    ListItem m_item_info;

protected:
    virtual CString GetDialogName() const override;

    DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();

protected:
    virtual void InitData() override;
};
