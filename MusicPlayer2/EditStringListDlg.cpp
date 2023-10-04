// SelectItemDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "MusicPlayer2.h"
#include "EditStringListDlg.h"
#include "afxdialogex.h"


// CEditStringListDlg 对话框

#define IDC_ITEM_EDITBOX 1991

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

void CEditStringListDlg::SetDlgIcon(HICON icon)
{
    m_icon = icon;
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


BEGIN_MESSAGE_MAP(CEditStringListDlg, CBaseDialog)
END_MESSAGE_MAP()


// CEditStringListDlg 消息处理程序


BOOL CEditStringListDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    SetWindowText(m_title);
    if (m_icon == NULL)
        SetIcon(theApp.m_icon_set.app.GetIcon(), FALSE);
    else
        SetIcon(m_icon, FALSE);

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
        m_items.push_back(m_list_ctrl.GetItemText(i, 0).GetString());
    }

    CBaseDialog::OnOK();
}
