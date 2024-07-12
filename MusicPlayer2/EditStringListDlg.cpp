// SelectItemDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "EditStringListDlg.h"


// CEditStringListDlg 对话框

IMPLEMENT_DYNAMIC(CEditStringListDlg, CBaseDialog)

CEditStringListDlg::CEditStringListDlg(vector<wstring>& items, CWnd* pParent /*=nullptr*/)
    : CBaseDialog(IDD_SELECT_ITEM_DIALOG, pParent), m_items(items)
{

}

CEditStringListDlg::~CEditStringListDlg()
{
}

void CEditStringListDlg::SetTitle(LPCTSTR title)
{
    m_title = title;
}

void CEditStringListDlg::SetDlgIcon(IconMgr::IconType icon_type)
{
    m_icon_type = icon_type;
}


void CEditStringListDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_list_ctrl);
}


CString CEditStringListDlg::GetDialogName() const
{
    return _T("SelectItemDlg");
}

bool CEditStringListDlg::InitializeControls()
{
    SetWindowTextW(m_title);
    // IDC_LIST1
    // IDOK
    // IDCANCEL

    RepositionTextBasedControls({
        { CtrlTextInfo::R1, IDOK, CtrlTextInfo::W32 },
        { CtrlTextInfo::R2, IDCANCEL, CtrlTextInfo::W32 }
        });
    return true;
}


BEGIN_MESSAGE_MAP(CEditStringListDlg, CBaseDialog)
END_MESSAGE_MAP()


// CEditStringListDlg 消息处理程序


BOOL CEditStringListDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    if (m_icon_type == IconMgr::IconType::IT_NO_ICON)
        SetIcon(IconMgr::IconType::IT_App, FALSE);
    else
        SetIcon(m_icon_type, FALSE);

    for (const auto& str : m_items)
        m_list_ctrl.AddString(str.c_str());

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}

void CEditStringListDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类
    m_items.clear();
    int item_count = m_list_ctrl.GetItemCount();
    for (int i = 0; i < item_count; i++)
    {
        m_items.push_back(m_list_ctrl.GetItemText(i).GetString());
    }

    CBaseDialog::OnOK();
}
