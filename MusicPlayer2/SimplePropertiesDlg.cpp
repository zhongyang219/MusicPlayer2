// SimplePropertiesDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "MusicPlayer2.h"
#include "SimplePropertiesDlg.h"

// CSimplePropertiesDlg 对话框

IMPLEMENT_DYNAMIC(CSimplePropertiesDlg, CBaseDialog)

CSimplePropertiesDlg::CSimplePropertiesDlg(CWnd* pParent /*=nullptr*/)
    : CBaseDialog(IDD_SELECT_ITEM_DIALOG, pParent)
{

}

CSimplePropertiesDlg::~CSimplePropertiesDlg()
{
}

bool CSimplePropertiesDlg::InitializeControls()
{
    // IDC_LIST1
    // IDOK
    SetDlgControlText(IDCANCEL, L"TXT_CLOSE");

    RepositionTextBasedControls({
        { CtrlTextInfo::R1, IDOK, CtrlTextInfo::W32 },
        { CtrlTextInfo::R2, IDCANCEL, CtrlTextInfo::W32 }
        });
    return true;
}

void CSimplePropertiesDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_list_ctrl);
}

BEGIN_MESSAGE_MAP(CSimplePropertiesDlg, CBaseDialog)
END_MESSAGE_MAP()


// CSimplePropertiesDlg 消息处理程序


BOOL CSimplePropertiesDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化

    //初始化控件
    ShowDlgCtrl(IDOK, false);
    if (auto pWnd = GetDlgItem(IDCANCEL))
        pWnd->SetFocus();

    //初始化列表
    CRect rect;
    m_list_ctrl.GetWindowRect(rect);
    m_list_ctrl.SetExtendedStyle(m_list_ctrl.GetExtendedStyle() | LVS_EX_GRIDLINES);
    int width0 = (std::min)(rect.Width() / 2, theApp.DPI(160));
    int width1 = rect.Width() - width0 - theApp.DPI(20) - 1;
    m_list_ctrl.InsertColumn(0, theApp.m_str_table.LoadText(L"TXT_ITEM").c_str(), LVCFMT_LEFT, width0);
    m_list_ctrl.InsertColumn(1, theApp.m_str_table.LoadText(L"TXT_VALUE").c_str(), LVCFMT_LEFT, width1);

    //获取数据
    InitData();

    //插入行
    int row_count = static_cast<int>(m_items.size());
    for (int i{}; i < row_count; i++)
    {
        m_list_ctrl.InsertItem(i, m_items[i].label.c_str());
        m_list_ctrl.SetItemText(i, 1, m_items[i].value.c_str());
    }

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}
