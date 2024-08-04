#pragma once
#include "BaseDialog.h"
#include "ListCtrlEx.h"


// CSimplePropertiesDlg 对话框

class CSimplePropertiesDlg : public CBaseDialog
{
    DECLARE_DYNAMIC(CSimplePropertiesDlg)

public:
    CSimplePropertiesDlg(CWnd* pParent = nullptr);   // 标准构造函数
    virtual ~CSimplePropertiesDlg();

    // 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_SELECT_ITEM_DIALOG };
#endif

protected:
    //对话框中每一行的内容
    struct Item
    {
        std::wstring label;     //标签的文本
        std::wstring value;     //值的文本

        Item() {}
        Item(const std::wstring& _label, const std::wstring& _value)
            : label(_label), value(_value)
        {}
    };

    std::vector<Item> m_items;

    //派生类中需要实现此函数，并向m_items中添加数据
    virtual void InitData() = 0;

private:
    CListCtrlEx m_list_ctrl;

    virtual bool InitializeControls() override;
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
};
