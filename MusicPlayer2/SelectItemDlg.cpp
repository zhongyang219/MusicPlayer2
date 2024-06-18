// SelectItemDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "SelectItemDlg.h"


// CSelectItemDlg 对话框

IMPLEMENT_DYNAMIC(CSelectItemDlg, CBaseDialog)

CSelectItemDlg::CSelectItemDlg(const vector<wstring>& items, CWnd* pParent /*=nullptr*/)
    : CBaseDialog(IDD_SELECT_ITEM_DIALOG, pParent), m_items(items)
{

}

CSelectItemDlg::~CSelectItemDlg()
{
}

void CSelectItemDlg::SetTitle(LPCTSTR title)
{
    m_title = title;
}

void CSelectItemDlg::SetDlgIcon(IconMgr::IconType icon_type)
{
    m_icon_type = icon_type;
}

wstring CSelectItemDlg::GetSelectedItem() const
{
    if (m_item_selected >= 0 && m_item_selected < static_cast<int>(m_items.size()))
        return m_items[m_item_selected];
    return wstring();
}

CString CSelectItemDlg::GetDialogName() const
{
    return _T("SelectItemDlg");
}

bool CSelectItemDlg::InitializeControls()
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

void CSelectItemDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_list_ctrl);
}

BEGIN_MESSAGE_MAP(CSelectItemDlg, CBaseDialog)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CSelectItemDlg::OnLvnItemchangedList1)
END_MESSAGE_MAP()


// CSelectItemDlg 消息处理程序


BOOL CSelectItemDlg::OnInitDialog()
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


void CSelectItemDlg::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    m_item_selected = pNMLV->iItem;
    *pResult = 0;
}
